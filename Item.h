#pragma once
#include "Entity.h"

class Item : public Entity {
public:
    bool isContainer;
    bool isKey;
    int Value;

    Item(const std::string& n, const std::string& desc,
        bool container = false, bool key = false, int val = 0)
        : Entity(EntityType::ITEM, n, desc),
        isContainer(container), isKey(key), Value(val) {}

    void Update() override {}
};
