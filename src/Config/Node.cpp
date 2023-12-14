#include "Node.h"

bool Node::IsLimitExcept() const {
    if (main_[0] == "limit_except")
        return true;
    return false;
}

bool Node::IsLocation() const {
    if (main_[0] == "location")
        return true;
    return false;
}

// todo more checks
bool Node::LocationContextHasPropperAddress() const {
    return main_.size() == 2;
}

bool Node::LocationContextIsNotEmpty() const {
    return !(directives_.empty() && child_nodes_.empty());
}

const std::string & Node::LocationContextGetAddress() const {
    return main_[1];
}
