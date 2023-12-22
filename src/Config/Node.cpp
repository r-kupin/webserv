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
    return main_.size() == 2 && main_[1][0] == '/';
}

bool Node::LocationContextIsNotEmpty() const {
    return !(directives_.empty() && child_nodes_.empty());
}

const std::string & Node::LocationContextGetAddress() const {
    return main_[1];
}

bool Node::ContextDoesntHaveSubLocationsWithSameAddress() const {
    for (size_t i = 0; i < child_nodes_.size(); ++i) {
        if (child_nodes_[i].IsLocation()) {
            std::string address = child_nodes_[i].LocationContextGetAddress();
            for (size_t j = i + 1; j < child_nodes_.size(); ++j) {
                if (child_nodes_[j].IsLocation() &&
                    child_nodes_[j].LocationContextGetAddress() == address)
                    return false;
            }
        }
    }
    return true;
}

Node::Node(const v_str &main, const std::vector<v_str> &directives,
           const v_node &childNodes) : main_(main), directives_(directives),
                                       child_nodes_(childNodes) {}

Node::Node() {}

Node::Node(const v_str &main, const std::vector<v_str> &directives) : main_(
        main), directives_(directives) {}

Node::Node(const v_str &main, const v_node &childNodes) : main_(main),
                                                          child_nodes_(
                                                                  childNodes) {}
