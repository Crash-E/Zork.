#pragma once
#include "Entity.h"
#include <functional>

class Room;
class Player;
class World;
class Item;

class Creature : public Entity {
public:
	std::vector<Room*> patrolRooms;
	std::vector<Item*> drops;
	std::function<void(Player*, World*)> onAct;
	bool  combatEnded = false;
	int   patrolIndex;
	int   actCount = 0;
	bool  peacefulResolved = false;
	int   worthCost = 0;
	int   damage;
	bool  isPacified;
	int   hp;
	int   maxHp;
	bool  isHostile;
	Room* location;

	Creature(const std::string& n, const std::string& desc,
		int health, bool hostile, Room* loc = nullptr, int dmg = 5, bool pacified = false)
		: Entity(EntityType::CREATURE, n, desc),
		hp(health), maxHp(health), isHostile(hostile), location(loc), damage(dmg), isPacified(pacified), patrolIndex(0) {
	}

	void Update() override;

	bool IsAlive() const { return hp > 0; }
	void TakeDamage(int dmg) { hp -= dmg; if (hp < 0) hp = 0; }
};