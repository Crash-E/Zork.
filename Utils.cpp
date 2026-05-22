#include "Utils.h"
#include "Entity.h"
#include "Player.h"
#include "Room.h"
#include "Item.h"
#include "World.h"
#include <iostream>
#include <random>
#include <algorithm>
#include <limits>

std::string ParseMovement(const std::string& input) {
	if (input == "go north" || input == "go n" || input == "walk north" ||
		input == "move north" || input == "head north" || input == "north" || input == "n")
		return "north";
	if (input == "go south" || input == "go s" || input == "walk south" ||
		input == "move south" || input == "head south" || input == "south" || input == "s")
		return "south";
	if (input == "go east" || input == "go e" || input == "walk east" ||
		input == "move east" || input == "head east" || input == "east" || input == "e")
		return "east";
	if (input == "go west" || input == "go w" || input == "walk west" ||
		input == "move west" || input == "head west" || input == "west" || input == "w")
		return "west";
	if (input == "go up" || input == "go u" || input == "walk up" ||
		input == "move up" || input == "head up" || input == "up" || input == "u")
		return "up";
	if (input == "go down" || input == "go d" || input == "walk down" ||
		input == "move down" || input == "head down" || input == "down" || input == "d")
		return "down";
	return "";
}

std::string ParseTarget(const std::string& input) {
	size_t pos = input.find(' ');
	if (pos == std::string::npos) return "";
	return input.substr(pos + 1);
}

bool MatchesPartial(const std::string& name, const std::string& target) {
	if (target.empty()) return false;
	std::string lowerName = name;
	std::transform(lowerName.begin(), lowerName.end(), lowerName.begin(), ::tolower);
	if (lowerName.find(target) != std::string::npos) return true;
	return false;
}

Entity* FindInRoomAndContainers(Player* player, const std::string& target) {
	Entity* found = player->currentRoom->FindByName(target);
	if (found) return found;

	for (Entity* e : player->currentRoom->contains) {
		if (MatchesPartial(e->name, target)) return e;
	}

	for (Entity* e : player->currentRoom->contains) {
		if (e->type == EntityType::ITEM) {
			Item* container = static_cast<Item*>(e);
			if (container->isContainer) {
				for (Entity* child : container->contains) {
					if (MatchesPartial(child->name, target)) return child;
				}
			}
		}
	}
	return nullptr;
}

Entity* FindInInventory(Player* player, const std::string& target) {
	for (Item* item : player->inventory) {
		if (MatchesPartial(item->name, target)) return item;
	}
	return nullptr;
}

bool CoinFlip(int coins, const std::string& win, const std::string& lose, const std::string& action, World& world) {
	bool success = false;
	if (coins <= 0) {
		std::cout << "You cannot do That!\n";
		return false;
	}
	else if (coins >= 4) coins = 4;
	std::uniform_int_distribution<int> dist(0, 1);
	std::cout << "\n" << action << " " << coins << " times";
	for (int i = 0; i < coins; ++i) {
		if (dist(world.rng) == 1) {
			std::cout << "\n> " << win;
			success = true;
		}
		else {
			std::cout << "\n> " << lose;
		}
	}
	std::cout << "\n\n\n";
	return success;
}

bool RequireInInventory(Player* p, Item* item) {
	for (Item* i : p->inventory) {
		if (i == item) return false;
	}
	std::cout << "You need to pick it up first.\n";
	return true;
}

void PressEnter() {
	std::cout << "\n[press enter to continue]";
	std::cin.ignore(std::numeric_limits<std::streamsize>::max(), '\n');
}
