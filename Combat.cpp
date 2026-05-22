#include "Combat.h"
#include "Player.h"
#include "Creature.h"
#include "NPC.h"
#include "Room.h"
#include "Item.h"
#include "World.h"
#include "Utils.h"
#include "Commands.h"
#include <iostream>
#include <string>

Creature* currentEnemy = nullptr;

void CheckLevelUp(Player& player) {
	while (player.xp >= player.xpToNextLevel) {
		player.level++;
		player.xp -= player.xpToNextLevel;
		player.xpToNextLevel += 20;
		std::cout << "\n=== LEVEL UP! You are now level " << player.level << " ===\n";
		std::cout << "Choose a stat to increase:\n";
		std::cout << "[fight] [act] [flee] [item] [evade]\n> ";
		std::string choice;
		while (true) {
			std::getline(std::cin, choice);
			if (choice == "fight") { player.Fight++; break; }
			else if (choice == "act") { player.Act++; break; }
			else if (choice == "flee") { player.Flee++; break; }
			else if (choice == "item") { player.ItemStat++; break; }
			else if (choice == "evade") { player.Evade++; break; }
			else std::cout << "Invalid choice. Try again.\n> ";
		}
		std::cout << "Stat increased!\n";
	}
}

void Combat(Player& player, Creature& enemy, World& world) {
	currentEnemy = &enemy;

	bool hasExcalibur = false;
	for (Item* item : player.inventory) {
		if (item->name == "Excalibur") { hasExcalibur = true; break; }
	}
	if (hasExcalibur && enemy.name != "Mordred") {
		std::cout << "\nYour enemy cowers at the might of Excalibur. The fight ends before it even begins.\n";
		enemy.isHostile = false;
		enemy.peacefulResolved = true;
		enemy.combatEnded = true;
		currentEnemy = nullptr;
		PressEnter();
		return;
	}
	else if (enemy.name == "Mordred") {
		HandleTalk(&player, "Mordred", world);
	}

	player.inCombat = true;
	enemy.combatEnded = false;
	enemy.peacefulResolved = false;
	bool extraTurn = player.sneakMode;
	if (extraTurn) std::cout << "!!!=====You have the drop on " << enemy.name << "! You get an extra turn on your first round.=====!!!\n\n\n";

	while (player.IsAlive() && enemy.IsAlive() && !enemy.combatEnded) {
		PressEnter();
		std::cout << "PLAYER ||| HP: " << player.hp << "/" << player.maxHp << "\n" << enemy.name << " ||| HP: " << enemy.hp << "/" << enemy.maxHp << "\n";
		std::cout << "\n[attack] [act] [item] [flee] [status]\n> ";
		std::string action;
		std::getline(std::cin, action);

		if (action == "status" || action == "s") {
			HandleStatus(&player);
		}
		else {
			if (action == "attack" || action == "a") {
				if (CoinFlip(player.Fight, "hit", "miss", "attacking", world)) {
					std::cout << "You hit " << enemy.name << "!\n";
					enemy.TakeDamage(10);
				}
				else {
					std::cout << "You missed!\n";
				}
			}
			else if (action == "flee" || action == "f") {
				if (CoinFlip(player.Flee, "escaping", "Stumbling", "running away", world)) {
					std::cout << "You flee";
					if (player.previousRoom != nullptr) {
						player.currentRoom = player.previousRoom;
						std::cout << " back the way you came";
					}
					std::cout << "!\n";
					currentEnemy = nullptr;
					return;
				}
				else {
					std::cout << "You couldn't escape!\n";
				}
			}
			else if (action == "item" || action == "i") {
				if (CoinFlip(player.ItemStat, "opening inventory", "failed to use item", "using an item", world)) {
					HandleInventory(&player);
					std::cout << "Use which item?\n> ";
					std::string itemTarget;
					std::getline(std::cin, itemTarget);
					HandleUse(&player, itemTarget);
				}
				else std::cout << "You fumble with your items!\n";
			}
			else if (action == "act") {
				if (CoinFlip(player.Act, "successfully acting", "failing to act", "acting", world)) {
					Entity* found = player.currentRoom->FindByName(enemy.name);
					if (found && found->type == EntityType::CREATURE) {
						NPC* npc = dynamic_cast<NPC*>(static_cast<Creature*>(found));
						if (npc && npc->onAct)
							npc->onAct(&player, &world);
						else if (enemy.onAct)
							enemy.onAct(&player, &world);
						else
							std::cout << "There is nothing you can do.\n";
					}
				}
			}
			else {
				std::cout << "Invalid action.\n";
				extraTurn = true;
			}
			world.Update();
			if (extraTurn) {
				std::cout << "Do a different action!\n";
				extraTurn = false;
			}
			else if (enemy.hp > 0 && enemy.damage != 0) {
				std::cout << "Enemy attacks \n";
				if (CoinFlip(player.Evade, "dodging", "getting attacked", "defending", world)) {
					std::cout << "You blocked the attack!\n";
				}
				else {
					std::cout << "\n" << enemy.name << " attacks for " << enemy.damage << " damage!\n";
					player.TakeDamage(enemy.damage);
					if (!player.IsAlive()) break;
				}
			}
		}
	}
	currentEnemy = nullptr;
	player.inCombat = false;
	if (!player.IsAlive()) return;

	if (!enemy.peacefulResolved) {
		player.Worthy -= enemy.worthCost;
		int xpGain = 10;
		if (!player.sneakMode) xpGain = (xpGain * 3) / 2;
		player.xp += xpGain;
		std::cout << "\nYou defeated " << enemy.name << "! +" << xpGain << " XP\n";
		player.currentRoom->RemoveEntity(&enemy);
		for (Item* drop : enemy.drops) {
			player.currentRoom->AddEntity(drop);
			std::cout << "The " << enemy.name << " dropped " << drop->name << ".\n";
		}
	}
	else {
		std::cout << "\nYou have been spared.\n";
	}
	CheckLevelUp(player);
	PressEnter();
}
