#pragma once
#include "Entity.h"
#include "Item.h"
class Room;

class Player : public Entity {
public:
    Item* equipped = nullptr; 
    Room* previousRoom = nullptr;
    int   xpToNextLevel;
    int   Fight;
    int   Act;
    int   Evade;
    int   Flee;
    int   ItemStat;
    bool  sneakMode;
    int   xp;
    int   level;
    int   Worthy;
    int   hp;
    int   maxHp;
    Room* currentRoom;
    bool  hasObjective;
	bool  hasGrail;
	bool  firstVisit;
    int   turnsPlayed;
    std::list<Item*> inventory;
    int   TotalValue;
	bool inCombat = false;

    Player(const std::string& n, Room* startRoom)
        : Entity(EntityType::PLAYER, n, "The player"),
        hp(100), maxHp(100), currentRoom(startRoom),
        hasObjective(false), hasGrail(false), firstVisit(true), turnsPlayed(0), TotalValue(0), Worthy(10), Fight(1), Act(1), Evade(1), Flee(1), ItemStat(1), sneakMode(true), xp(0), level(1), xpToNextLevel(20) {
    }

    void Update() override;

    bool IsAlive() const { return hp > 0; }
    void TakeDamage(int dmg) { hp -= dmg; if (hp < 0) hp = 0; }
};