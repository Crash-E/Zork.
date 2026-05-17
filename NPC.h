#pragma once
#include "Creature.h"
#include <vector>

class NPC : public Creature {
public:
    std::string dialogue;
    std::vector<Room*> patrolRooms;
    int patrolIndex;

    NPC(const std::string& n, const std::string& desc,
        int health, bool hostile, Room* loc, const std::string& talk)
        : Creature(n, desc, health, hostile, loc),
        dialogue(talk), patrolIndex(0) {
    }

    void Update() override;
    void Speak() const;
};