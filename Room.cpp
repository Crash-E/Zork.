#include "Room.h"
#include "Item.h"
#include "Creature.h"
#include <iostream>

void Room::Describe() const {
    std::cout << "\n=== " << name << " ===\n";
    std::cout << description << "\n";

    bool hasItems = false;
    for (Entity* e : contains) {
        if (e->type == EntityType::ITEM) {
            if (!hasItems) { std::cout << "\nYou see:\n"; hasItems = true; }
            std::cout << "  - " << e->name << ": " << e->description << "\n";
        }
    }

    for (Entity* e : contains) {
        if (e->type == EntityType::CREATURE) {
            std::cout << "\n" << e->name << " is here.\n";
        }
    }

    std::cout << "\nExits: ";
    if (exits.empty()) {
        std::cout << "none";
    }
    else {
        bool first = true;
        for (Exit* ex : exits) {
            if (!first) std::cout << ", ";
            std::cout << ex->name;
            if (ex->isLocked) std::cout << " (locked)";
            first = false;
        }
    }
    std::cout << "\n";
}