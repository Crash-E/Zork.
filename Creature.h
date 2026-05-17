#pragma once
#include "Entity.h"

class Room;

class Creature : public Entity {
public:
    int  hp;
    int  maxHp;
    bool isHostile;
    Room* location;

    Creature(const std::string& n, const std::string& desc,
        int health, bool hostile, Room* loc)
        : Entity(EntityType::CREATURE, n, desc),
        hp(health), maxHp(health), isHostile(hostile), location(loc) {
    }

    void Update() override;

    bool IsAlive() const { return hp > 0; }
    void TakeDamage(int dmg) { hp -= dmg; if (hp < 0) hp = 0; }
};