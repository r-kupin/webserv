#include <algorithm>
#include <iostream>
#include "ServerConfiguration.h"

Location    &add_or_update(Location &child, Location &parent) {
    return (child.HasSameAddressAs(parent)) ? parent : child;
}

// todo: or maybe we can update non-root locations?
bool        parent_have_it_already(const Location &current,
                                   const Location &parent) {
    return !current.HasSameAddressAs(parent) &&
           current.HasSameAddressAsOneOfSublocationsOf(parent);
}

bool need_to_add_current_to_parent_first(const l_loc_it &parent,
                                         const Location &current,
                                         const Node &context) {
    return !(context.LocationContextGetAddress() == current.address_) &&
           parent->address_ != current.address_ &&
           !parent->HasAsSublocation(current);//todo: or !parent_have_it_already(current, *parent);
}

void ServerConfiguration::CheckLocationContextIsCorrect(const Node &context) {
    if (!context.LocationContextHasPropperAddress())
        ThrowServerConfigError("Location path is incorrect or missing");
    if (!context.LocationContextIsNotEmpty())
        ThrowServerConfigError("Location context can't be empty !");
    if (!context.ContextDoesntHaveSubLocationsWithSameAddress())
        ThrowServerConfigError("Location context can't have subcontexts with "
                               "same address");
}

void        ServerConfiguration::ApplyLocationContext(const Node &context,
                                                      l_loc_it parent,
                                                      Location &current) {
    CheckLocationContextIsCorrect(context);
    if (need_to_add_current_to_parent_first(parent, current, context)) {
        parent->sublocations_.push_front(current);
        parent->sublocations_.begin()->parent_ = parent;
    } if (parent->HasSameAddressAs(current)) {
        RecurseLocations(context, parent);
    } else {
        RecurseLocations(context, parent->sublocations_.begin());
    }
}

void        ServerConfiguration::RecurseLocations(const Node &context,
                                                  l_loc_it parent) {
    CheckLocationContextIsCorrect(context);
    Location maybe_current;
    try {
        maybe_current = Location(context.LocationContextGetAddress(), parent);
    } catch (Location::LocationException &) {
        ThrowServerConfigError("Location address contains invalid characters");
    }
    if(parent_have_it_already(maybe_current, *parent))
        ThrowServerConfigError("Each location needs unique address inside "
                               "each context");
    Location &current = add_or_update(maybe_current, *parent);
    current.ProcessDirectives(context.directives_);
    for (v_node_c_it it = context.child_nodes_.begin();
         it != context.child_nodes_.end(); ++it) {
        if (it->IsLimitExcept()) {
//            ApplyLimitExceptContext(*it, current);
        } else if (it->IsLocation()) {
            ApplyLocationContext(*it, parent, current);
        }
    }
    if (!parent->HasSameAddressAs(current) &&
        !parent->HasAsSublocation(current)) {
        parent->sublocations_.push_front(current);
        parent->sublocations_.begin()->parent_ = parent;
    }
}












void        ServerConfiguration::RecurseLocations(const Node &context,
                                                  l_loc_it parent,
                                                  Location current) {
    parent->sublocations_.push_front(current);
    l_loc_it current_ptr = parent->sublocations_.begin();
    current_ptr->ProcessDirectives(context.directives_);
    current_ptr->parent_ = parent;
    
    for (v_node_c_it it = context.child_nodes_.begin();
         it != context.child_nodes_.end(); ++it) {
        if (it->IsLimitExcept()) {
            current_ptr->HandleLimitExcept(*it);
        } else if (it->IsLocation()) {
            Location::CheckSublocationsAddress(it->LocationContextGetAddress(),
                                               current_ptr->full_address_);
            HandleLocationContext(*it);
        }
    }
}

void        ServerConfiguration::OverrideLocation(const Node &context,
                                                  l_loc_it current) {
   current->ProcessDirectives(context.directives_);

   for (v_node_c_it it = context.child_nodes_.begin();
         it != context.child_nodes_.end(); ++it) {
        if (it->IsLimitExcept()) {
            current->HandleLimitExcept(*it);
        } else if (it->IsLocation()) {
            Location::CheckSublocationsAddress(it->LocationContextGetAddress(),
                                               current->full_address_);
            HandleLocationContext(*it);
        }
    }
}

void        ServerConfiguration::HandleLocationContext(const Node &context) {
    LocSearchResult result = FindLocation(context.main_[1]);
    CheckLocationContextIsCorrect(context);
    if (result.status_ == "found") {
        std::cout << "overriding " + result.full_address_ << std::endl;
        OverrideLocation(context, result.location_);
    } else if (result.status_ == "not found") {
//        only last step is missing
        v_str nonexisting_path = Location::SplitAddress(result.leftower_address_);
        l_loc_it parent = result.location_;
//        create ghosts
        for (v_str_c_it it = nonexisting_path.begin();
            *it != nonexisting_path.back(); ++it) {
            parent->sublocations_.push_front(
                    Location::GhostLocation(*it)); //todo: check parent!!
            parent->sublocations_.begin()->parent_ = parent;
            parent = parent->sublocations_.begin();
        }
        RecurseLocations(context, parent, Location(result.full_address_, parent));
    } else {
        ThrowServerConfigError("Location address contains invalid characters");
    }
}