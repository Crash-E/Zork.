#pragma once
#include <string>

class World;
class Player;
class Creature;

class WorldBuilder {
private:
	std::string runeSequence;
	bool forcefieldDown;
	bool ringFound;
	Player* player;
	Creature* boar;

public:
	WorldBuilder()
		: runeSequence(""), forcefieldDown(false), ringFound(false),
		player(nullptr), boar(nullptr) {
	}

	void Build(World& world);
	Player* GetPlayer() const { return player; }
	Creature* GetBoar() const { return boar; }
};
