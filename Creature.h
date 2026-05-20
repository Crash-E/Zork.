#pragma once
#include "Entity.h"
#include <functional>

class Room;
class Player;
class World;

class Creature : public Entity {
public:
	bool  combatEnded = false;
	std::function<void(Player*, World*)> onAct;
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
		int health, bool hostile, Room* loc, int dmg = 5, bool pacified = false)
		: Entity(EntityType::CREATURE, n, desc),
		hp(health), maxHp(health), isHostile(hostile), location(loc), damage(dmg), isPacified(pacified) {
	}

	void Update() override;

	bool IsAlive() const { return hp > 0; }
	void TakeDamage(int dmg) { hp -= dmg; if (hp < 0) hp = 0; }
};