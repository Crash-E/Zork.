#pragma once
#include "Entity.h"

class Item : public Entity {
public:
    bool isContainer;
    bool isKey;
    int Value;
    bool isFixed;
    std::string Examine;

    Item(const std::string& n, const std::string& desc, const std::string& ex,
        bool container = false, bool key = false, int val = 0, bool fix = false)
        : Entity(EntityType::ITEM, n, desc),
        isContainer(container), isKey(key), Value(val), isFixed(fix), Examine(ex) {}

    void Update() override {}
};
