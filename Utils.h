#pragma once
#include <string>

class Entity;
class Player;
class Item;
class World;

std::string ParseMovement(const std::string& input);
std::string ParseTarget(const std::string& input);
bool MatchesPartial(const std::string& name, const std::string& target);
Entity* FindInRoomAndContainers(Player* player, const std::string& target);
Entity* FindInInventory(Player* player, const std::string& target);
bool CoinFlip(int coins, const std::string& win, const std::string& lose, const std::string& action, World& world);
bool RequireInInventory(Player* p, Item* item);
void PressEnter();
