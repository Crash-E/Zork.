#pragma once
#include "Entity.h"
#include <functional>

class Player;

class Item : public Entity {
public:
    bool isContainer;
    bool isKey;
    int Value;
    bool isFixed;
    std::string Examine;
    std::string interact;
    std::function<void(Player*)> onInteract;

    Item(const std::string& n, const std::string& desc, const std::string& ex = "",
        bool container = false, bool key = false, int val = 0, const std::string& inter = "", bool fix = false)
        : Entity(EntityType::ITEM, n, desc),
        isContainer(container), isKey(key), Value(val), Examine(ex), interact(inter), isFixed(fix) {}

    void Update() override {}
};
