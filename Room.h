#pragma once
#include "Entity.h"
#include "Exit.h"
#include <vector>

class Room : public Entity {
public:
    std::vector<Exit*> exits;
    bool visited;

    Room(const std::string& n, const std::string& desc)
        : Entity(EntityType::ROOM, n, desc), visited(false) {
    }

    void Update() override {}

    void AddExit(Exit* exit) { exits.push_back(exit); }

    Exit* GetExit(const std::string& dirStr) const {
        Direction dir;
        if (!Exit::ParseDirection(dirStr, dir)) return nullptr;
        for (Exit* e : exits) {
            if (e->direction == dir) return e;
        }
        return nullptr;
    }

    void Describe() const;
};