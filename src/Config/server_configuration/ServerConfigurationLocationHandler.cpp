#include <algorithm>
#include <iostream>
#include "ServerConfiguration.h"

void        ServerConfiguration::HandleLocationContext(const Node &context) {
    LocSearchResult result = FindLocation(context.main_[1]);
    CheckLocationContextIsCorrect(context);
    if (result.status_ == "found") {
        OverrideLocation(context, result.location_);
    } else if (result.status_ == "not found") {
        AddNew(context, result);
    } else {
        ThrowServerConfigError("Location address contains invalid characters");
    }
}

void ServerConfiguration::CheckLocationContextIsCorrect(const Node &context) {
    if (!context.LocationContextHasPropperAddress())
        ThrowServerConfigError("Location path is incorrect or missing");
    if (!context.ContextDoesntHaveSubLocationsWithSameAddress())
        ThrowServerConfigError("Location context can't have subcontexts with "
                               "same address");
}

void        ServerConfiguration::OverrideLocation(const Node &context,
                                                  l_loc_it current) {
//    save old indexes, in if case they will not be overridden
    bool before_owerride = current->has_own_index_defined_;
    l_str list = current->own_index_;
//    allow index override
    current->has_own_index_defined_ = false;
    current->own_index_.clear();
    current->ProcessDirectives(context.directives_);
//    restore old index if it wasn't overridden
    if (!current->has_own_index_defined_) {
        current->has_own_index_defined_ = before_owerride;
        current->own_index_ = list;
    }

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

void ServerConfiguration::AddNew(const Node &context,
                                 const LocSearchResult &result) {

    v_str nonexisting_path = Location::SplitAddress(result.leftower_address_);
    l_loc_it parent = result.location_;

    for (v_str_c_it it = nonexisting_path.begin();
         *it != nonexisting_path.back(); ++it) {
        parent->sublocations_.push_front(
                Location::GhostLocation(*it));
        parent->sublocations_.begin()->parent_ = parent;
        parent = parent->sublocations_.begin();
    }
    RecurseLocations(context, parent, Location(result.full_address_, parent));
}

void        ServerConfiguration::RecurseLocations(const Node &context,
                                                  l_loc_it parent,
                                                  const Location& current) {
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