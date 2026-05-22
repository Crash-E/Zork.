#pragma once
#include <string>

class Player;
class World;
class Item;
class Creature;

void HandleLook(Player* player);
void HandleInventory(Player* player);
void HandleStatus(Player* player);
bool HandleMove(Player* player, const std::string& input, Creature* Boar);
void HandleTake(Player* player, const std::string& target);
void HandleDrop(Player* player, const std::string& target);
void HandleExamine(Player* player, const std::string& target);
void HandleUse(Player* player, const std::string& target);
void HandleTalk(Player* player, const std::string& target, World& world);
void HandleAttack(Player* player, const std::string& target, World& world);
void EquipItem(Player* p, Item* newItem);
