#include "Commands.h"
#include "Player.h"
#include "Room.h"
#include "Item.h"
#include "Exit.h"
#include "Creature.h"
#include "NPC.h"
#include "World.h"
#include "Utils.h"
#include "Combat.h"
#include <iostream>
#include <string>

void HandleLook(Player* player) {
	player->currentRoom->Describe();
}

void HandleInventory(Player* player) {
	if (player->inventory.empty()) {
		std::cout << "You are carrying nothing.\n";
	}
	else {
		std::cout << "          === Inventory ===\n \n";
		for (Item* item : player->inventory) {
			std::cout << "  = " << item->name << ": " << item->description << "\n";
		}
	}
}

void HandleStatus(Player* player) {
	std::cout << "          === Status ===\n";
	std::cout << "HP: " << player->hp << "/" << player->maxHp << "\n";
	std::cout << "Level: " << player->level << " (XP: " << player->xp << "/" << player->xpToNextLevel << ")\n";
	std::cout << "Fight: " << player->Fight << "\n";
	std::cout << "Act: " << player->Act << "\n";
	std::cout << "Evade: " << player->Evade << "\n";
	std::cout << "Flee: " << player->Flee << "\n";
	std::cout << "Item: " << player->ItemStat << "\n";
	std::cout << "Turns played: " << player->turnsPlayed << "\n";
	std::cout << "Inventory value: " << player->TotalValue << "\n";
	if (player->equipped != nullptr)
		std::cout << "Equipped: " << player->equipped->name << "\n";
}

bool HandleMove(Player* player, const std::string& input, Creature* Boar) {
	std::string direction = ParseMovement(input);
	if (direction.empty()) return false;
	Exit* ex = player->currentRoom->GetExit(direction);
	if (ex) {
		if (ex->isLocked) {
			Item* key = nullptr;
			for (Item* item : player->inventory) {
				if (item->name == ex->requiredKey) { key = item; break; }
			}
			if (!key) {
				std::cout << "You need a " << ex->requiredKey << " to go that way.\n";
				return false;
			}
			std::cout << "Use the " << key->name << "? (y/n)\n> ";
			std::string choice;
			std::getline(std::cin, choice);
			if (choice != "y" && choice != "yes") return false;
			ex->isLocked = false;
			player->inventory.remove(key);
		}
		if (Boar->IsAlive() && !Boar->peacefulResolved && !Boar->patrolRooms.empty()) {
			Boar->patrolIndex = (Boar->patrolIndex + 1) % Boar->patrolRooms.size();
			Room* newLoc = Boar->patrolRooms[Boar->patrolIndex];
			Boar->location->RemoveEntity(Boar);
			Boar->location = newLoc;
			newLoc->AddEntity(Boar);
		}
		player->firstVisit = false;
		player->previousRoom = player->currentRoom;
		player->currentRoom = ex->destination;
		player->currentRoom->Describe();
		return true;
	}
	std::cout << "No exit that way.\n";
	return false;
}

void HandleTake(Player* player, const std::string& target) {
	if (player->hasObjective && target != "excalibur") {
		std::cout << "nothing interests you anymore, it's time to leave.\n";
		return;
	}
	Entity* found = FindInRoomAndContainers(player, target);
	if (!found || found->type != EntityType::ITEM) {
		std::cout << "You don't see that here.\n";
		return;
	}

	Item* item = static_cast<Item*>(found);
	if (item->isFixed) {
		std::cout << item->name << " cannot be taken.\n";
		if (item->onInteract) {
			std::cout << "Would you like to interact with it instead? (y/n)\n> ";
			std::string confirm;
			std::getline(std::cin, confirm);
			if (confirm == "y" || confirm == "yes") {
				item->onInteract(player);
			}
		}
		return;
	}

	Entity* parent = item->parent;
	if (parent != nullptr) {
		parent->RemoveEntity(item);
		if (parent->type == EntityType::ITEM) {
			Item* parentItem = static_cast<Item*>(parent);
			if (parentItem->isContainer && parent->contains.empty() && parent->parent != nullptr) {
				parent->parent->RemoveEntity(parent);
				std::cout << parentItem->name << " is empty.\n";
			}
		}
	}

	player->inventory.push_back(item);
	player->TotalValue += item->Value;
	std::cout << "You grab " << item->name << ".\n";
	if (item->onTake) item->onTake(player);
}

void HandleDrop(Player* player, const std::string& target) {
	Entity* found = FindInInventory(player, target);
	if (found && found->type == EntityType::ITEM) {
		Item* item = static_cast<Item*>(found);
		player->inventory.remove(item);
		player->TotalValue -= item->Value;
		player->currentRoom->AddEntity(item);
		std::cout << "You drop " << item->name << ".\n";
	}
	else {
		std::cout << "You don't have that.\n";
	}
}

void HandleExamine(Player* player, const std::string& target) {
	Entity* found = FindInRoomAndContainers(player, target);
	if (!found) found = FindInInventory(player, target);

	if (!found) {
		std::cout << "You don't see that.\n";
		return;
	}
	if (found->type != EntityType::ITEM) {
		std::cout << found->description << "\n";
		return;
	}
	Item* asItem = static_cast<Item*>(found);

	if (!asItem->Examine.empty())
		std::cout << asItem->Examine << "\n";
	else
		std::cout << found->description << "\n";

	if (asItem->onExamine)
		asItem->onExamine(player);

	if (asItem->isContainer && !found->contains.empty()) {
		std::cout << "Inside:\n";
		for (Entity* e : found->contains) {
			std::cout << "  - " << e->name << "\n";
		}
	}
}

void HandleUse(Player* player, const std::string& target) {
	Entity* found = FindInRoomAndContainers(player, target);
	if (!found) found = FindInInventory(player, target);

	if (found && found->type == EntityType::ITEM) {
		Item* asItem = static_cast<Item*>(found);
		if (!asItem->interact.empty())
			std::cout << "Do you want to " << asItem->interact << "? (y/n)\n> ";
		else
			std::cout << "Do you want to interact with " << asItem->name << "? (y/n)\n> ";

		std::string confirm;
		std::getline(std::cin, confirm);
		if (confirm == "y" || confirm == "yes") {
			if (asItem->onInteract)
				asItem->onInteract(player);
			else
				std::cout << "Nothing happens.\n";
		}
	}
	else {
		std::cout << "You don't see that.\n";
	}
	PressEnter();
}

void HandleTalk(Player* player, const std::string& target, World& world) {
	Entity* found = player->currentRoom->FindByName(target);
	if (!found) {
		for (Entity* e : player->currentRoom->contains) {
			if (e->type == EntityType::CREATURE && MatchesPartial(e->name, target)) {
				found = e;
				break;
			}
		}
	}
	if (!found || found->type != EntityType::CREATURE) {
		std::cout << "You don't see anyone to talk to.\n";
		return;
	}
	NPC* npc = dynamic_cast<NPC*>(static_cast<Creature*>(found));
	if (npc == nullptr) {
		std::cout << found->name << " doesn't respond.\n";
		return;
	}
	if (npc->onTalk)
		npc->onTalk(player, &world);
	else
		std::cout << npc->name << " says: " << npc->dialogue << "\n";
}

void HandleAttack(Player* player, const std::string& target, World& world) {
	Entity* found = player->currentRoom->FindByName(target);
	if (!found) {
		for (Entity* e : player->currentRoom->contains) {
			if (e->type == EntityType::CREATURE && MatchesPartial(e->name, target)) {
				found = e;
				break;
			}
		}
	}
	if (found && found->type == EntityType::CREATURE) {
		Creature* enemy = static_cast<Creature*>(found);
		Combat(*player, *enemy, world);
	}
	else {
		std::cout << "You don't see that here.\n";
	}
}

void EquipItem(Player* p, Item* newItem) {
	if (p->equipped == newItem) {
		std::cout << "Already equipped.\n";
		return;
	}
	if (p->equipped != nullptr) {
		Item* old = p->equipped;
		p->Fight -= old->buffFight;
		p->Act -= old->buffAct;
		p->Evade -= old->buffEvade;
		p->Flee -= old->buffFlee;
		p->ItemStat -= old->buffItem;
		p->maxHp -= old->buffHealth;
		std::cout << "You unequip the " << old->name << ".\n";
	}
	p->equipped = newItem;
	p->Fight += newItem->buffFight;
	p->Act += newItem->buffAct;
	p->Evade += newItem->buffEvade;
	p->Flee += newItem->buffFlee;
	p->ItemStat += newItem->buffItem;
	p->maxHp += newItem->buffHealth;
	std::cout << "You equip the " << newItem->name << ".\n";
}
