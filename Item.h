#pragma once
#include "Entity.h"

class Item : public Entity {
public:
    bool isContainer;
    bool isKey;

    Item(const std::string& n, const std::string& desc,
        bool container = false, bool key = false)
        : Entity(EntityType::ITEM, n, desc),
        isContainer(container), isKey(key) {
    }

    void Update() override {}
};
