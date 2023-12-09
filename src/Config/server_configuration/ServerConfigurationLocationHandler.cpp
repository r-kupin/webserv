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
}

void        ServerConfiguration::ApplyLimitExceptContext(const Node &context,
                                                         Location &current) {
    if (current.HasDefinedLimitExcept()) {
        ThrowServerConfigError("Limit_except context is already defined");
    } else {
        current.HandleLimitExcept(context);
    }
}

void        ServerConfiguration::ApplyLocationContext(const Node &context,
                                                      l_loc_it parent,
                                                      Location &current) {
    CheckLocationContextIsCorrect(context);
    if (need_to_add_current_to_parent_first(parent, current, context))
        parent->sublocations_.push_front(current);
    if (parent->HasSameAddressAs(current)) {
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
            ApplyLimitExceptContext(*it, current);
        } else if (it->IsLocation()) {
            ApplyLocationContext(*it, parent, current);
        }
    }
    if (!parent->HasSameAddressAs(current) &&
        !parent->HasAsSublocation(current))
        parent->sublocations_.push_front(current);
}

//void Config::IsCorrectLocation(Node &loc_node, Location &current_l) {
//    if (loc_node.main_[0] == "location") {
//        bool        ret = false, limit = false, root = false,
//                    index = false, err = false;
//
//        if (loc_node.main_.size() != 2)
//            ThrowSyntaxError("Location path is incorrect or missing");
//        current_l.address_ = loc_node.main_[1];
//        for (size_t i = 0; i < loc_node.child_nodes_.size(); ++i) {
//            HandleLimitExceptContext(loc_node.child_nodes_[i],
//                                     current_l);
//        }
//        ProcessLocationDirectives(loc_node, current_l, root, index, ret,
//                                err);
//        if (!index && !ret && !limit && !root && !err)
//            ThrowSyntaxError("Location context should contain at least one of "
//                             "following directives (directly or in "
//                             "subcontext): root, index, or return!");
//    }
//}

//            if (node_.directives_[0][0] == "deny" &&
//                node_.directives_[0][1] == "all" &&
//                node_.directives_[0].size() == 2 ) {
//                curr_limit.return_code_ = 403;
//            } else if (node_.directives_[0][0] == "return" &&
//                        (node_.directives_[0].size() == 2 ||
//                        node_.directives_[0].size() == 3)) {
//                curr_limit.return_code_ = std::atoi(node_.directives_[0][1].c_str());
//                if (Location::kHttpOkCodes.find(curr_limit.return_code_) ==
//                    Location::kHttpOkCodes.end() &&
//                    ErrPage::kHttpErrCodes.find(curr_limit.return_code_) ==
//                    ErrPage::kHttpErrCodes.end()) {
//                    ThrowSyntaxError("return directive is wrong");
//                }
//                if (node_.directives_[0].size() == 3) {
//                    if (node_.directives_[0][2].find_first_of('/') !=
//                        std::string::npos) {
//                        curr_limit.return_address_ = node_.directives_[0][2];
//                    } else {
//                        ThrowSyntaxError("Limit_except context needs 1 of these "
//                                         "directives: return or deny !");
//                    }
//                }
//        }