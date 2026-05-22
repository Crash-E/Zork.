#pragma once

class Player;
class Creature;
class World;

extern Creature* currentEnemy;

void Combat(Player& player, Creature& enemy, World& world);
void CheckLevelUp(Player& player);
