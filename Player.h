#pragma once
#include "Entity.h"

class Room;

class Player : public Entity {
public:
    int   hp;
    int   maxHp;
    Room* currentRoom;
    bool  hasObjective;
    int   turnsPlayed;

    Player(const std::string& n, Room* startRoom)
        : Entity(EntityType::PLAYER, n, "The player"),
        hp(100), maxHp(100), currentRoom(startRoom),
        hasObjective(false), turnsPlayed(0) {
    }

    void Update() override;

    bool IsAlive() const { return hp > 0; }
    void TakeDamage(int dmg) { hp -= dmg; if (hp < 0) hp = 0; }
};