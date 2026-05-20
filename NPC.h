#pragma once
#include "Creature.h"
#include <vector>
#include <functional>

class Player;
class World;

class NPC : public Creature {
public:
    int state = 0;
    std::string dialogue;
    std::vector<Room*> patrolRooms;
    int patrolIndex;
    std::function<void(Player*, World*)> onTalk;

    NPC(const std::string& n, const std::string& desc,
        int health, bool hostile, Room* loc, const std::string& talk)
        : Creature(n, desc, health, hostile, loc),
        dialogue(talk), patrolIndex(0) {
    }

    void Update() override;
    void Speak() const;
};