#include "Entity.h"
#include <algorithm>
#include <cctype>

bool Entity::RemoveEntity(Entity* e) {
    for (auto it = contains.begin(); it != contains.end(); ++it) {
        if (*it == e) {
            contains.erase(it);
            return true;
        }
    }
    return false;
}

Entity* Entity::FindByName(const std::string& n) const {
    std::string lower_n = n;
    std::transform(lower_n.begin(), lower_n.end(), lower_n.begin(), ::tolower);

    for (Entity* e : contains) {
        std::string lower_name = e->name;
        std::transform(lower_name.begin(), lower_name.end(), lower_name.begin(), ::tolower);
        if (lower_name == lower_n) return e;
    }
    return nullptr;
}