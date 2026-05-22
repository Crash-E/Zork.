#pragma once
#include "Entity.h"
#include <functional>

class Player;

class Item : public Entity {
public:
    bool isContainer;
    int Value;
    bool isFixed;
    std::function<void(Player*)> onTake;
    std::string Examine;
    std::string interact;
    std::function<void(Player*)> onInteract;
    std::function<void(Player*)> onExamine;
    int buffFight = 0;
    int buffAct = 0;
    int buffEvade = 0;
    int buffFlee = 0;
    int buffItem = 0;
    int buffHealth = 0;

    Item(const std::string& n, const std::string& desc, const std::string& ex = "",
        bool container = false, int val = 0, const std::string& inter = "", bool fix = false)
        : Entity(EntityType::ITEM, n, desc),
        isContainer(container), Value(val), Examine(ex), interact(inter), isFixed(fix) {}

    void Update() override {}
};
