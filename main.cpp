#include <iostream>
#include <string>
#include <vector>
#include <algorithm>
#include <random>
#include <limits>
#include <ctime>
#include "World.h"
#include "Room.h"
#include "Item.h"
#include "Exit.h"
#include "NPC.h"
#include "Player.h"


// HELPER FUNCTIONS
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
	// First try exact match
	Entity* found = player->currentRoom->FindByName(target);
	if (found) return found;

	// Try partial match on direct room contents
	for (Entity* e : player->currentRoom->contains) {
		if (MatchesPartial(e->name, target)) return e;
	}

	// Try inside containers in the room
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

bool CoinFlip(int coins, std::string win, std::string lose, std::string action, World& world) {
	bool success = false;
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

void PressEnter() {
	std::cout << "\n[press enter to continue]";
	std::cin.ignore(std::numeric_limits<std::streamsize>::max(), '\n');
}

// COMMAND FUNCTIONS
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

void Combat(Player& player, Creature& enemy, World& world) {
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
			else if (action == "flee" || action =="f") {
				if (CoinFlip(player.Flee, "escaping", "Stumbling", "running away", world)) {
					std::cout << "You flee";
					if (player.previousRoom != nullptr) {
						player.currentRoom = player.previousRoom;
						std::cout << " back the way you came";
					}
					std::cout << "!\n";
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
						NPC* npc = static_cast<NPC*>(found);
						if (npc->onAct)
							npc->onAct(&player, &world);
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
		std::cout << "You unequip the " << old->name << ".\n";
	}
	p->equipped = newItem;
	p->Fight += newItem->buffFight;
	p->Act += newItem->buffAct;
	p->Evade += newItem->buffEvade;
	p->Flee += newItem->buffFlee;
	p->ItemStat += newItem->buffItem;
	std::cout << "You equip the " << newItem->name << ".\n";
}

// ENDINGS
void CheckEndingPathA(Player* player) {
	bool isRich = player->TotalValue >= 15;
	int w = player->Worthy;

	if (w == 10 && isRich) {
		std::cout << "\n=== Ending 3/7 - Rich King ===\n\n";
		std::cout << "  You emerge from the cave, Excalibur gleaming at your side and your pack heavy with treasure.\n";
		std::cout << "  Word spreads faster than you walk. By the time you reach the nearest village, they already know your name.\n";
		std::cout << "  Not long after, your influence grows. Lords bend the knee. People look to you for protection.\n";
		std::cout << "  You take your rightful place as king. An era of peace is sure to come.\n";
	}
	else if (w == 10 && !isRich) {
		std::cout << "\n=== Ending 4/7 - Worthy! ===\n\n";
		std::cout << "  The sword felt weightless the moment you touched it. As if it had been waiting.\n";
		std::cout << "  You walk out of the cave into the light, Excalibur in hand, and for the first time in a long while, you feel certain.\n";
		std::cout << "  Through many trials, you proved yourself worthy. Once you return, you will be hailed as a hero.\n";
		std::cout << "  Many people will soon look to you for protection. You are ready.\n";
	}
	else if (w <= 5 && isRich) {
		std::cout << "\n=== Ending 6/7 - Rich ===\n\n";
		std::cout << "  The sword rejected you. But your pack is heavy.\n";
		std::cout << "  The cave gave you other things. Good things. Things you can sell, spend, and live on.\n";
		std::cout << "  You walk out into the daylight and count your earnings.\n";
		std::cout << "  Not the legend you came for. But enough to build something real.\n";
	}
	else if (w <= 5 && !isRich) {
		std::cout << "\n=== Ending 7/7 - Unworthy ===\n\n";
		std::cout << "  You stood before the stone. The runes lit. The forcefield dropped.\n";
		std::cout << "  Your hand closed around the hilt and the sword burned cold. It didn't move.\n";
		std::cout << "  The chamber was silent. The statues watched.\n";
		std::cout << "  After all you have done, you return empty handed. The sword has judged you. You failed.\n";
	}
	else {
		std::cout << "\n=== Ending ?/7 - Coward's Way Out ===\n\n";
		std::cout << "  You turn back at the entrance. The cave yawns behind you, indifferent.\n";
		std::cout << "  No shame in it. The sword was never going anywhere.\n";
		std::cout << "  At least you got out with your life.\n";
		std::cout << "  Plenty of people who went in didn't.\n";
	}
	PressEnter();
	std::cout << "\nTurns survived: " << player->turnsPlayed << "\n";
	std::cout << "Worthiness: " << player->Worthy << "\n";
	std::cout << "Treasure collected: " << player->TotalValue << "\n";
}

void CheckEndingPathB(Player* player) {
	bool isRich = player->TotalValue >= 15;
	bool hasGrail = player->hasGrail;
	int w = player->Worthy;

	if (w >= 6 && w <= 9 && !hasGrail) {
		std::cout << "\n=== Ending 5/7 - Worthy? ===\n\n";
		std::cout << "  You hold the sword. It felt real. The lady smiled and placed it in your hands.\n";
		std::cout << "  You returned home. People celebrated. Life went on.\n";
		std::cout << "  But ever since that cave, nothing has truly felt real.\n";
		std::cout << "  The food has no taste. The laughter sounds distant. You wonder sometimes if you ever left.\n";
	}
	else if (w >= 6 && w <= 9 && hasGrail && !isRich) {
		std::cout << "\n=== Ending 5/7-S - Dream No More ===\n\n";
		std::cout << "  The lake appeared. The lady smiled. And then the Grail burned bright and the dream fell apart at the seams.\n";
		std::cout << "  You blinked. Stone walls. Darkness. The sword still in the rock.\n";
		std::cout << "  You knew it wouldn't let you take it. But you walk out with your mind your own.\n";
		std::cout << "  That is more than most who come here can say.\n";
	}
	else if (w >= 6 && w <= 9 && hasGrail && isRich) {
		std::cout << "\n=== Ending 6/7-S - Real Wealth ===\n\n";
		std::cout << "  The lake shimmered. The figure reached out. But the Grail pulsed warm and the illusion dissolved.\n";
		std::cout << "  You stood in an empty cave. The sword remained in the stone, unmoved, unjudged.\n";
		std::cout << "  Your pack is heavy. Your mind is clear.\n";
		std::cout << "  Some people chase legends. You walk out with something better. Real wealth, and the clarity to enjoy it.\n";
	}
	else if (w <= -1 && !hasGrail) {
		std::cout << "\n=== Ending 2/7 - Corrupted ===\n\n";
		std::cout << "  The dream took you before you could resist it.\n";
		std::cout << "  A dark version of everything you wanted. A throne built on ash. A sword that drips black.\n";
		std::cout << "  You never woke up.\n";
		std::cout << "  Somewhere in the cave, your body still stands at the edge of a lake, smiling at nothing.\n";
		std::cout << "  Your infamy spreads regardless. People fear what you became.\n";
		std::cout << "  But you are not there to enjoy it.\n";
	}
	else if (w <= -1 && hasGrail) {
		std::cout << "\n=== Ending 1/7 - Devil of the Lake ===\n\n";
		std::cout << "  The Grail shattered the dream before it could take hold.\n";
		std::cout << "  The lady stood at the center of the lake, unsmiling, watching you with ancient eyes.\n";
		std::cout << "  She knew why you were there.\n";
		std::cout << "  The fight was long. She was not weak.\n";
		std::cout << "  But when it was over, you took what was hers. Her power. Her sword. Corrupted now, darkened by what you are.\n";
		std::cout << "  You walk out of the cave a legend. The wrong kind.\n";
		std::cout << "  Kingdoms will whisper your name like a curse for generations to come.\n";
	}
	PressEnter();
	std::cout << "\nTurns survived: " << player->turnsPlayed << "\n";
	std::cout << "Worthiness: " << player->Worthy << "\n";
	std::cout << "Treasure collected: " << player->TotalValue << "\n";

}

void GameOver(Player* player) {
	std::cout << "\n=== GAME OVER ===\n";
	std::cout << "The darkness takes you. Your story ends here, forgotten in the depths of the cave.\n";
	std::cout << "Perhaps in another life, you would have been worthy.\n";
	PressEnter();
	std::cout << "\nTurns survived: " << player->turnsPlayed << "\n";
	std::cout << "Worthiness: " << player->Worthy << "\n";
	std::cout << "Treasure collected: " << player->TotalValue << "\n";
}

// MAIN
int main() {

	std::string runeSequence = "";
	bool forcefieldDown = false;
	int rich = 15;
	std::string helpText =
		"                               === COMMANDS ===\n \n"
		"   go / walk / move / head        +[direction]  = move in a direction\n"
		"   look / observe / l                           = look around the room\n"
		"   examine / inspect / study      +[target]     = examine something closely\n"
		"   take / grab / pick             +[item]       = pick up an item\n"
		"   drop / discard                 +[item]       = drop an item\n"
		"   talk / speak / greet           +[npc]        = talk to someone\n"
		"   attack / hit                   +[target]     = attack something\n"
		"   use / apply / consume          +[item]       = use an item\n"
		"   inventory / inv / i                          = show your items\n"
		"   status / stats / hp                          = show your health\n"
		"   sneak / stealth                              = toggle stealth mode (ON by default)\n"
		"   help / h / ?                                 = show this list\n"
		"   quit                                         = close the game\n"
		"\n                               === COMBAT COMMANDS ===\n \n"
		"   fight                                        = attempt to strike the enemy\n"
		"   act                                          = attempt a non-violent action\n"
		"   flee                                         = attempt to escape combat\n"
		"   item                                         = use an item from your inventory\n"
		"   status                                       = check your status (no turn used)\n";

	World world;

	// ROOMS
	Room* entrance = new Room("Cave Entrance",
		"You stand at the mouth of a dark cave. A cold wind blows from within.\n"
		"The light of day barely reaches past the first few feet of stone.\n"
		"Yellow plants cover the fields to your right.");

	Room* AltarEntrance = new Room("Altar Entrance",
		"The tunnel opens into a grand stone gate, its archway carved with scenes of battles long forgotten.\n"
		"Two statues flank the entrance.\n"
		"To the right, a robed figure clutching a staff, his face worn smooth by time.\n"
		"To the left, a broken warrior, sword in hand, stands vigil, missing its head and left arm.\n"
		"You stumble upon a bundle of ropes on the ground.");

	Room* ExcaliburRoom = new Room("Excalibur Chamber",
		"The chamber opens into a vast circular room, its ceiling lost in darkness above.\n"
		"At its center, a sword stands embedded in a weathered stone, surrounded by a shimmering forcefield that distorts the air around it.\n"
		"Three runes are carved into the base of the stone, each glowing faintly - A, G, and B.\n"
		"Two knight statues flank the stone. The first, tall and noble, faces the second.\n"
		"The second turns its gaze toward the sword, as if it has been watching over it for centuries.\n"
		"Behind them both, a larger statue of a crowned figure stares toward the first knight, its expression unreadable.");

	Room* Dream = new Room("Dream",
		"The cave gives way to something impossible. You find yourself in an endless grassfield.\n"
		"An open sky stretches above you, pale and still, stars unmoving as if painted there.\n"
		"A calm lake spreads before you, its surface like glass, reflecting nothing.\n"
		"At its center, a figure stands upon the water, draped in white, watching you in silence.");

	Room* Quarters = new Room("Quarters",
		"A narrow passage opens into a hidden chamber, warm and dry compared to the cave outside.\n"
		"Shelves line the walls, crowded with scrolls, tomes, and artifacts wrapped in cloth.\n"
		"Someone has been living here. A bedroll sits in the corner, a half eaten meal beside it.\n"
		"At the center of the room, a stone pedestal holds a single golden cup, glowing faintly in the dark.");

	Room* ambush = new Room("Ambush",
		"The tunnel widens without warning.\n"
		"A thunderous rumble echoes from ahead, growing louder by the second.\n"
		"Before you can react, a group of boars burst from the darkness, charging straight at you.");

	Room* NarrowTunnel = new Room("Narrow Tunnel",
		"The passage narrows until your shoulders nearly brush the walls on both sides.\n"
		"The air is stale and still. Nothing moves here.\n"
		"A skeleton slumps against the wall ahead, one hand clutching something to its chest.\n"
		"A worn leather sheath at its waist holds a dagger that catches the torchlight - oddly clean for this place.");

	Room* Waterfall = new Room("Waterfall",
		"The cave opens into a wide grotto, the sound of rushing water filling the air.\n"
		"Cracks in the ceiling feed a steady stream that cascades down the far wall into a shallow pool below.\n"
		"Herbs grow along the water's edge, fed by the mist.\n"
		"Slumped against a rock near the pool, a man lies motionless, his chest rising and falling slowly.");

	Room* Catacombs = new Room("Catacombs",
		"The walls here are not stone but coffins, stacked from floor to ceiling in every direction.\n"
		"Skulls line the gaps between them, their hollow eyes watching your every step.\n"
		"The air is thick and cold.\n"
		"A figure crouches near the far wall, rifling through a coffin, freezing the moment he hears you enter.");

	Room* RitualRoom = new Room("Ritual Room",
		"The smell hits you before you see it - incense, thick and sweet, masking something underneath.\n"
		"Three robed figures stand in a circle at the room's center, their backs to you.\n"
		"Candles flicker across strange symbols carved into the floor.\n"
		"As one, they turn.");

	// Add all rooms to world
	world.AddEntity(entrance);
	world.AddEntity(AltarEntrance);
	world.AddEntity(ExcaliburRoom);
	world.AddEntity(Dream);
	world.AddEntity(Quarters);
	world.AddEntity(ambush);
	world.AddEntity(NarrowTunnel);
	world.AddEntity(Waterfall);
	world.AddEntity(Catacombs);
	world.AddEntity(RitualRoom);

	// CAVE ENTRANCE CONTENT
	Item* poisonHerbs = new Item("Yellow Herbs",
		"A cluster of yellow herbs. Often used by hunters for traps.",
		"Herbs used to sedate beasts during hunts. Strong enough to knock a person out for hours.",
		false, false, 1, "eat the herbs");

	poisonHerbs->onInteract = [](Player* p) {
		std::cout << "You eat the herbs. They taste bitter.\nYou pass out for several hours.\n";
		p->turnsPlayed += 15;
		};

	NPC* mordred = new NPC("Mordred",
		"A figure in dark armor stands at the cave mouth, blocking your path.",
		100, true, entrance,
		"So. You found it. Hand it over and walk free.");
	mordred->damage = 18;
	mordred->worthCost = 0;
	world.AddEntity(mordred);

	entrance->AddEntity(poisonHerbs);
	world.AddEntity(poisonHerbs);

	// ALTAR ENTRANCE CONTENT
	Item* leverStatue = new Item("Broken Statue",
		"A broken statue stands guard.",
		"The pose matches the Arthur statue in the excalibur chamber.\na lever besides it looks operable.",
		false, false, 0, "pull the lever", true);
	world.AddEntity(leverStatue);

	Item* Rope = new Item("Rope",
		"A cluster of ropes, always useful.", "", false, true, 2);

	AltarEntrance->AddEntity(Rope);
	world.AddEntity(Rope);

	NPC* greenKnight = new NPC("Green Knight",
		"A towering knight clad in green armor, his axe resting on his shoulder.",
		80, true, AltarEntrance,
		"You dare approach the gate? State your purpose or prepare to fight.");
	greenKnight->damage = 15;
	greenKnight->worthCost = 3;

	greenKnight->onAct = [greenKnight, AltarEntrance](Player* p, World* w) {
		std::cout << "\"This is no time to talk, yield or fight!\"\n";
		std::cout << "[yield] [fight]\n> ";
		std::string choice;
		std::getline(std::cin, choice);
		if (choice == "yield" || choice == "y") {
			greenKnight->actCount++;
			if (greenKnight->actCount == 1) {
				std::cout << "\"Interesting. You hesitate. Show me more.\"\n";
				std::cout << "The knight steps back, but his axe is still raised.\n";
				PressEnter();
			}
			else {
				std::cout << "You lower your weapon. The knight nods slowly.\n";
				std::cout << "\"Wisdom. You may pass.\"\n";
				std::cout << "The knight's ghostly figure begins to fade away.\n";
				greenKnight->isHostile = false;
				greenKnight->peacefulResolved = true;
				greenKnight->hp = 0;
				AltarEntrance->RemoveEntity(greenKnight);
				PressEnter();
			}
		}
		else if (choice == "fight" || choice == "f") {
			std::cout << "You prepare to fight the Green Knight.\n";
		}
		else {
			std::cout << "The Green Knight respects your choice.\n";
		}
		};

	AltarEntrance->AddEntity(greenKnight);
	world.AddEntity(greenKnight);

	// NARROW TUNNEL CONTENT
	Item* skeleton = new Item("Skeleton",
		"A skeleton slumps against the wall.",
		"Old brittle bones, armor rusted to nothing. His hand seems to be clutching something. You notice a sheath around his waist.",
		false, false, 0, "", true);

	Item* hand = new Item("Skeleton's Hand",
		"The skeleton's hand seems to be clutching something.",
		"The fingers are curled tight around something red, a crumpled note wedged beneath the thumb.",
		true, false, 0, "", true);

	Item* sheath = new Item("Sheath",
		"A worn leather sheath at its waist.",
		"Cracked leather, but the blade inside catches the light perfectly - untouched by time.",
		true, false, 0, "", true);

	Item* potion = new Item("Potion",
		"A small vial of red liquid. Looks like it could heal wounds.", "", false, false, 2);

	Item* wornNote = new Item("Worn Note",
		"A crumpled piece of paper. The ink is smudged beyond repair.",
		"Most of the note is illegible, you can make out \"make sure to st*- safe. love Mari*-\"",
		false, false, 0);

	Item* pristineDagger = new Item("Pristine Dagger",
		"A dagger in perfect condition. Oddly clean for such a dark place.", "equip the dagger", false, false, 3);
	pristineDagger->buffFight = 2;

	hand->AddEntity(potion);
	hand->AddEntity(wornNote);
	sheath->AddEntity(pristineDagger);

	NarrowTunnel->AddEntity(skeleton);

	skeleton->onExamine = [hand, sheath, NarrowTunnel](Player* p) {
		static bool revealed = false;
		if (!revealed) {
			NarrowTunnel->AddEntity(hand);
			NarrowTunnel->AddEntity(sheath);
			revealed = true;
		}
		};

	world.AddEntity(skeleton);
	world.AddEntity(hand);
	world.AddEntity(sheath);
	world.AddEntity(potion);
	world.AddEntity(wornNote);
	world.AddEntity(pristineDagger);


	// CATACOMBS CONTENT
	Item* bejeweledSkull = new Item("Bejeweled Skull",
		"A skull encrusted with jewels.", "", false, false, 5);
	Item* ornamentalSword = new Item("Ornamental Sword",
		"An ornate sword. Looks ceremonial, not practical.", "", false, false, 5);
	Item* ancientCloak = new Item("Ancient Cloak",
		"A cloak woven with strange symbols.", "", false, false, 4);
	Item* burnedGrimoire = new Item("Burned Grimoire",
		"A burnt book. One line is still legible.", "", false, false, 4);
	world.AddEntity(bejeweledSkull);
	world.AddEntity(ornamentalSword);
	world.AddEntity(ancientCloak);
	world.AddEntity(burnedGrimoire);

	std::vector<Item*> coffinLoot = { bejeweledSkull, ornamentalSword, ancientCloak, burnedGrimoire };
	std::shuffle(coffinLoot.begin(), coffinLoot.end(), world.rng);

	Item* coffins = new Item("Coffins",
		"Four stone coffins are stacked along the walls.",
		"Four ancient coffins. Some look disturbed already.",
		false, false, 0, "loot a coffin", true);

	NPC* graveRobber = new NPC("Grave Robber",
		"A thin man crouches in the shadows, knife in hand. \nhe eyes you suspiciously, better not approach.",
		50, false, Catacombs,
		"");
	graveRobber->damage = 10;
	graveRobber->worthCost = 2;
	graveRobber->onTalk = [graveRobber](Player* p, World* w) {
		if (!graveRobber->isPacified) {
			std::cout << "The grave robber looks at you. \"should have left me alone!\"\n he takes a combat position.\n";
			Combat(*p, *graveRobber, *w);
		}
		else {
			std::cout << "I heard king arthur was buried here after he died. a shame he got betrayed.";
		}
		};

	static std::vector<int> coffinState = { 0, 0, 0, 0 };

	coffins->onInteract = [coffins, &coffinLoot, graveRobber, Catacombs](Player* p) {
		std::cout << "Which coffin do you loot? (1/2/3/4)\n> ";
		std::string choice;
		std::getline(std::cin, choice);
		int idx = -1;
		if (choice == "1") idx = 0;
		else if (choice == "2") idx = 1;
		else if (choice == "3") idx = 2;
		else if (choice == "4") idx = 3;
		else { std::cout << "Not a coffin number.\n"; return; }

		if (coffinState[idx] != 0) {
			std::cout << "That coffin is already empty.\n";
			return;
		}

		Item* loot = coffinLoot[idx];
		p->inventory.push_back(loot);
		p->TotalValue += loot->Value;
		coffinState[idx] = 1;
		std::cout << "You loot the coffin and find: " << loot->name << ".\n";

		if (graveRobber->IsAlive()) {
			for (int i = 3; i > 0; --i) {
				if (coffinState[i] == 0) {
					Item* robberLoot = coffinLoot[i];
					graveRobber->drops.push_back(robberLoot);
					coffinState[i] = 2;
					std::cout << "The grave robber opens another coffin, stuffing its content into his bag.\n";
					break;
				}
			}
		}


		bool allEmpty = true;
		for (int s : coffinState) if (s == 0) { allEmpty = false; break; }
		if (allEmpty && graveRobber->IsAlive()) {
			graveRobber->peacefulResolved = true;
			graveRobber->description = "A fellow graverobber.";
			std::cout << "\nthe grave robber relaxes at your unified intentions. \"I guess I'm not the only one after the kings tomb\"\n";
		}
		};

	Item* loneSkull = new Item("Lone Skull",
		"A single skull sits in the middle of the room, alone.",
		"An ordinary skull, oddly placed in the center of the room.",
		false, false, 0);
	loneSkull->onTake = [loneSkull, Catacombs](Player* p) {
		p->inventory.remove(loneSkull);
		std::cout << "The skull crumbles to dust the moment you touch it.\n";
		};

	Catacombs->AddEntity(coffins);
	Catacombs->AddEntity(graveRobber);
	Catacombs->AddEntity(loneSkull);
	world.AddEntity(coffins);
	world.AddEntity(graveRobber);
	world.AddEntity(loneSkull);

	// Roaming Boar
	Creature* Boar = new Creature("Boar", "A wild boar, tusks gleaming.", 40, true);
	Boar->damage = 7;
	Boar->worthCost = 1;
	world.AddEntity(Boar);
	Boar->onAct = [Boar](Player* p, World* w) {
		Item* herbs = nullptr;
		for (Item* item : p->inventory) {
			if (item->name == "Yellow Herbs") { herbs = item; break; }
		}
		if (!herbs) {
			std::cout << "The boar snarls. There is nothing you can do. if you only had any hunting experience.\n";
			return;
		}
		std::cout << "Try to sedate the boar with Yellow Herbs? (y/n)\n> ";
		std::string choice;
		std::getline(std::cin, choice);
		if (choice != "y" && choice != "yes") return;

		if (CoinFlip(p->ItemStat, "applying", "fumbling", "sedating", *w)) {
			std::cout << "The boar staggers, then collapses, snoring softly.\n";
			Boar->name = "Sleeping Boar";
			Boar->damage = 0;
			p->inventory.remove(herbs);
			p->TotalValue -= herbs->Value;
			Boar->isHostile = false;
			Boar->peacefulResolved = true;
			Boar->combatEnded = true;
			Boar->patrolRooms.clear();
		}
		else {
			std::cout << "You fail to apply the herbs! The boar snorts angrily.\n";
		}
		};
	Item* boarMeat = new Item("Boar Meat", "Tough but edible meat.", "", false, false, 0);
	Item* tusks = new Item("Tusks", "Sharp boar tusks.", "", false, false, 1);
	world.AddEntity(boarMeat);
	world.AddEntity(tusks);

	Boar->drops.push_back(boarMeat);
	Boar->drops.push_back(tusks);

	// WATERFALL CONTENT
	NPC* injuredMan = new NPC("Injured Man",
		"A man slumped against a rock, barely conscious.",
		30, false, Waterfall,
		"He seems to be fading in and out of consciousness.");
	injuredMan->damage = 0;
	injuredMan->worthCost = 2;

	injuredMan->onTalk = [](Player* p, World* w) {
		std::cout << "He seems to be fading in and out of consciousness.\n";
		std::cout << "[wake] [give] [steal] [leave]\n> ";
		std::string choice;
		std::getline(std::cin, choice);

		if (choice == "wake") {
			std::cout << "\nThe man stirs as you wake him.\n";
			std::cout << "\"Please... do you have anything to help?\"\n";
		}
		else if (choice == "give") {
			std::cout << "You offer something to help. What do you give?\n> ";
			std::string itemName;
			std::getline(std::cin, itemName);
			Entity* item = FindInInventory(p, itemName);
			if (item && item->type == EntityType::ITEM) {
				Item* given = static_cast<Item*>(item);
				if (given->Value >= 1) {
					p->inventory.remove(given);
					p->TotalValue -= given->Value;
					std::cout << "You give him the " << given->name << ". His eyes light up with gratitude.\n";
					std::cout << "\"Thank you... take this.\"\n";
				}
				else {
					std::cout << "He looks at it and shakes his head weakly.\n";
				}
			}
			else {
				std::cout << "You don't have that.\n";
			}
		}
		else if (choice == "steal") {
			std::cout << "You rifle through his belongings while he drifts in and out of consciousness.\n";
			p->Worthy -= 1;
		}
		else if (choice == "leave") {
			std::cout << "You leave him be.\n";
		}
		};

	Waterfall->AddEntity(injuredMan);
	world.AddEntity(injuredMan);

	// RITUAL ROOM CONTENT
	Item* incense = new Item("Incense",
		"A bundle of fragrant incense.", "", false, false, 3, "burn the incense");

	world.AddEntity(incense);

	Item* ritualisticMask = new Item("Ritualistic Mask",
		"A carved wooden mask with hollow eyes.", "", false, false, 5);
	ritualisticMask->buffAct = 1;
	ritualisticMask->buffItem = 1;
	ritualisticMask->buffFlee = 1;
	ritualisticMask->interact = "wear the mask";
	world.AddEntity(ritualisticMask);

	Item* moonPendant = new Item("Moon Pendant",
		"A pendant shaped like a crescent moon.",
		"Cold to the touch. Probably valuable.",
		false, false, 4);
	world.AddEntity(moonPendant);
	RitualRoom->AddEntity(moonPendant);

	Item* ritualisticDrawings = new Item("Ritualistic Drawings",
		"Strange carvings cover the walls.",
		"These carvings depict a lady clad in white cursing a sword. Her tears feed the blade.",
		false, false, 0, "", true);
	world.AddEntity(ritualisticDrawings);
	RitualRoom->AddEntity(ritualisticDrawings);

	Creature* cultists = new Creature("Cultists",
		"Three robed figures, faces hidden by hoods, chant in unison.",
		70, true, RitualRoom);
	cultists->damage = 12;
	cultists->worthCost = 3;
	cultists->drops.push_back(incense);
	cultists->drops.push_back(ritualisticMask);
	RitualRoom->AddEntity(cultists);
	world.AddEntity(cultists);

	cultists->onAct = [cultists, incense](Player* p, World* w) {
		Item* dagger = nullptr;
		for (Item* item : p->inventory) {
			if (item->name == "Pristine Dagger") { dagger = item; break; }
		}
		if (!dagger) {
			std::cout << "The cultists continue their chant. There is nothing you can do.\n";
			return;
		}
		std::cout << "You raise the Pristine dagger. The cultists pause, watching intently.\n";
		std::cout << "Offer your blood with the Pristine Dagger (-25 hp)? (y/n)\n> ";
		std::string choice;
		std::getline(std::cin, choice);
		if (choice != "y" && choice != "yes") {
			std::cout << "You lower the dagger. They resume their chant.\n";
			return;
		}
		std::cout << "You draw blood with the dagger. The cultists nod approvingly.\n";
		std::cout << "\"It is enough. Leave us to our ritual.\"\n";
		auto it = std::find(cultists->drops.begin(), cultists->drops.end(), incense);
		if (it != cultists->drops.end()) {
			cultists->drops.erase(it);
			std::cout << "one of the cultists silently hands you a bundle of incense.\n";
			p->inventory.push_back(incense);
			p->TotalValue += incense->Value;
		}
		p->TakeDamage(25);
		cultists->isHostile = false;
		cultists->peacefulResolved = true;
		cultists->combatEnded = true;
		};

	// EXCALIBUR CHAMBER CONTENT
	Item* arthurStatue = new Item("Arthur Statue",
		"A grand statue of a crowned figure stands behind the stone.",
		"King Arthur stands tall, crown upon his brow, his stone gaze fixed on the knight to his right.\n"
		"There's something deliberate in where he looks.",
		false, false, 0, "", true);

	Item* galahadStatue = new Item("Galahad Statue",
		"A knight statue stands to the right, tall and noble.",
		"Sir Galahad, the pure knight. His head is turned, eyes fixed on the knight beside him.\n"
		"He does not look at the sword.",
		false, false, 0, "", true);

	Item* bedivereStatue = new Item("Bedivere Statue",
		"A knight statue stands to the left, his gaze locked on the stone.",
		"Sir Bedivere, the loyal. His eyes are fixed firmly on the sword in the stone,\n"
		"as if waiting to see it drawn.",
		false, false, 0, "", true);

	Item* runeA = new Item("Rune A",
		"A glowing rune marked with the letter A.",
		"The rune pulses faintly, etched into the base of the stone.",
		false, false, 0, "press the rune", true);

	Item* runeG = new Item("Rune G",
		"A glowing rune marked with the letter G.",
		"The rune pulses faintly, etched into the base of the stone.",
		false, false, 0, "press the rune", true);

	Item* runeB = new Item("Rune B",
		"A glowing rune marked with the letter B.",
		"The rune pulses faintly, etched into the base of the stone.",
		false, false, 0, "press the rune", true);

	Item* excalibur = new Item("Excalibur",
		"The legendary sword, embedded in the stone, surrounded by a shimmering forcefield.",
		"Excalibur. The blade hums with quiet power, untouchable through the shimmering barrier around the stone.",
		false, false, 0, "touch Excalibur", false);

	runeA->onInteract = [&runeSequence, &forcefieldDown](Player* p) {
		if (forcefieldDown) { std::cout << "Nothing happens.\n"; return; }
		runeSequence += "A";
		std::cout << "Rune A glows brighter.\n";
		if (runeSequence == "AGB") {
			std::cout << "The runes flare in unison. The forcefield shatters with a low hum.\n";
			forcefieldDown = true;
		}
		else if (runeSequence.length() >= 3) {
			std::cout << "The runes dim. The sequence resets.\n";
			runeSequence = "";
		}
		};

	runeG->onInteract = [&runeSequence, &forcefieldDown](Player* p) {
		if (forcefieldDown) { std::cout << "Nothing happens.\n"; return; }
		runeSequence += "G";
		std::cout << "Rune G glows brighter.\n";
		if (runeSequence == "AGB") {
			std::cout << "The runes flare in unison. The forcefield shatters with a low hum.\n";
			forcefieldDown = true;
		}
		else if (runeSequence.length() >= 3) {
			std::cout << "The runes dim. The sequence resets.\n";
			runeSequence = "";
		}
		};

	runeB->onInteract = [&runeSequence, &forcefieldDown](Player* p) {
		if (forcefieldDown) { std::cout << "Nothing happens.\n"; return; }
		runeSequence += "B";
		std::cout << "Rune B glows brighter.\n";
		if (runeSequence == "AGB") {
			std::cout << "The runes flare in unison. The forcefield shatters with a low hum.\n";
			forcefieldDown = true;
		}
		else if (runeSequence.length() >= 3) {
			std::cout << "The runes dim. The sequence resets.\n";
			runeSequence = "";
		}
		};

	excalibur->onTake = [&forcefieldDown, &rich, Dream, entrance, excalibur, mordred, ExcaliburRoom](Player* p) {
		if (!forcefieldDown) {
			p->inventory.remove(excalibur);
			ExcaliburRoom->AddEntity(excalibur);
			std::cout << "But The forcefield blocks your hand. The sword remains untouchable.\n";
			return;
		}
		std::cout << "You reach for Excalibur...\n";
		p->hasObjective = true;
		entrance->AddExit(new Exit(Direction::SOUTH, ExcaliburRoom, false));
		entrance->AddEntity(mordred);
		if (p->Worthy == 10 || p->TotalValue >= rich) {
			mordred->isHostile = true;
			mordred->dialogue = "So. You found it. Hand it over and walk free.";
		}
		else {
			mordred->isHostile = false;
			mordred->dialogue = "Pathetic. Not even worth my blade. Crawl back to whatever hole you came from.";
		}
		if (p->Worthy < 10) {
			p->inventory.remove(excalibur);
			ExcaliburRoom->AddEntity(excalibur);
		}
		if (p->Worthy == 10) {
			std::cout << "The sword feels solid. Real.\n";
			std::cout << "It is time to leave.\n";
		}
		else if (p->Worthy <= 5 && p->Worthy >= 1) {
			std::cout << "The sword burns cold. It does not move.\n";
			std::cout << "The chamber rejects you. It is time to leave.\n";
		}
		else if (p->Worthy >= 6 && p->Worthy <= 9) {
			std::cout << "The world dissolves around you...\n";
			p->currentRoom = Dream;
			PressEnter();
			HandleLook(p);
		}
		else if (p->Worthy <= 0) {
			std::cout << "Darkness floods your vision. You are pulled down into a dream.\n";
			p->currentRoom = Dream;
			PressEnter();
			HandleLook(p);
		}
		};

	arthurStatue->onExamine = [leverStatue, AltarEntrance](Player* p) {
		static bool revealed = false;
		if (!revealed) {
			AltarEntrance->AddEntity(leverStatue);
			revealed = true;
			std::cout << "\nSomething clicks in your mind. The broken statue at the entrance...\n";
		}
		};

	ExcaliburRoom->AddEntity(arthurStatue);
	ExcaliburRoom->AddEntity(galahadStatue);
	ExcaliburRoom->AddEntity(bedivereStatue);
	ExcaliburRoom->AddEntity(runeA);
	ExcaliburRoom->AddEntity(runeG);
	ExcaliburRoom->AddEntity(runeB);
	ExcaliburRoom->AddEntity(excalibur);

	world.AddEntity(arthurStatue);
	world.AddEntity(galahadStatue);
	world.AddEntity(bedivereStatue);
	world.AddEntity(runeA);
	world.AddEntity(runeG);
	world.AddEntity(runeB);
	world.AddEntity(excalibur);

	//Quarters Content
	Item* holyGrail = new Item("Holy Grail",
		"A golden cup rests on a pedestal, glowing faintly.",
		"The Holy Grail. Ancient power radiates from its surface. The world feels clearer just looking at it.",
		false, false, 6);

	NPC* merlin = new NPC("Merlin",
		"An old man stands before you, eyes burning with anger.",
		100, true, Quarters,
		"Thief! Return what is not yours!");
	merlin->damage = 20;
	merlin->worthCost = 4;

	world.AddEntity(merlin);
	world.AddEntity(holyGrail);
	Quarters->AddEntity(holyGrail);

	Item* crystalBall = new Item("Crystal Ball",
		"A crystal ball rests on a small table.",
		"The crystal ball sits quietly. A note on the desk beside it reads:\n\"I found a way to measure worthiness.\"",
		false, false, 0, "touch the ball", true);

	crystalBall->onInteract = [](Player* p) {
		std::cout << "The ball glows ";
		if (p->Worthy == 10) std::cout << "gold";
		else if (p->Worthy >= 6) std::cout << "yellow";
		else if (p->Worthy >= 0) std::cout << "grey";
		else std::cout << "red";
		std::cout << " at your touch.\n";
		};

	Item* scroll = new Item("Scroll",
		"A scroll lies on the floor near the bookshelf.",
		"The scroll is brittle. The writing trails off mid-sentence:\n\"...some deemed unworthy do not leave the cave.\nThe sword pulls them into a dream they never wake from.\nOnly the Grail can shatter the illusion...\"",
		false, false, 0);
	world.AddEntity(scroll);

	Item* bookshelf = new Item("Bookshelf",
		"A bookshelf crammed with old tomes and scrolls.",
		"You reach for a book but an invisible barrier stops you.\nLooking down, you notice a scroll on the floor that must have slipped out.",
		false, false, 0, "", true);

	bookshelf->onExamine = [scroll, Quarters](Player* p) {
		static bool revealed = false;
		if (!revealed) {
			Quarters->AddEntity(scroll);
			revealed = true;
		}
		};

	holyGrail->onTake = [merlin, Quarters, &world](Player* p) {
		p->hasGrail = true;
		merlin->dialogue = "Long have I waited for one such as you. Now we may speak in peace.";
		Quarters->AddEntity(merlin);
		std::cout << "\nAs you lift the Grail, the air shifts. An old man steps from the shadows.\n";
		std::cout << "\"Thief! Return what is not yours!\"\n";
		Combat(*p, *merlin, world);
		if (p->IsAlive())
			p->currentRoom->Describe();
		};

	merlin->onAct = [merlin, holyGrail, Quarters](Player* p, World* w) {
		Item* grail = nullptr;
		for (Item* item : p->inventory) {
			if (item->name == "Holy Grail") { grail = item; break; }
		}
		if (!grail) {
			std::cout << "You have nothing to offer him.\n";
			return;
		}
		std::cout << "Offer the Holy Grail back to Merlin? (y/n)\n> ";
		std::string confirm;
		std::getline(std::cin, confirm);
		if (confirm != "y" && confirm != "yes") {
			std::cout << "You keep the Grail clutched tight.\n";
			return;
		}
		std::cout << "You hold out the Grail.\n";
		std::cout << "Merlin's expression softens. He takes it gently from your hands.\n";
		p->inventory.remove(grail);
		p->TotalValue -= grail->Value;
		p->hasGrail = false;
		Quarters->AddEntity(grail);
		merlin->peacefulResolved = true;
		merlin->isHostile = false;
		merlin->combatEnded = true;
		PressEnter();
		};

	Quarters->AddEntity(bookshelf);
	world.AddEntity(bookshelf);

	Quarters->AddEntity(crystalBall);
	world.AddEntity(crystalBall);

	// RANDOM ROOM SELECTION
	std::vector<Room*> raPool = { ambush, NarrowTunnel, Waterfall };
	std::vector<Room*> rbPool = { Catacombs, RitualRoom };

	std::shuffle(raPool.begin(), raPool.end(), world.rng);
	std::shuffle(rbPool.begin(), rbPool.end(), world.rng);

	Room* RA1 = raPool[0];
	Room* RA2 = raPool[1];
	Room* RA3 = raPool[2];
	Room* RB1 = rbPool[0];
	Room* RB2 = rbPool[1];

	leverStatue->onInteract = [Quarters, RB2](Player* p) {
		static bool pulled = false;
		if (pulled) {
			std::cout << "The lever is already pulled.\n";
			return;
		}
		pulled = true;
		RB2->AddExit(new Exit(Direction::EAST, Quarters, false));
		Quarters->AddExit(new Exit(Direction::WEST, RB2, false));
		std::cout << "A deep rumble echoes through the cave. Something has opened, somewhere.\n";
		};

	Boar->patrolRooms = { RA1, RA2, RA3 };
	Boar->location = RA1;
	RA1->AddEntity(Boar);

	//Ending
	NPC* ladyOfLake = new NPC("Lady of the Lake",
		"A figure stands upon the water, draped in white, watching you in silence.",
		150, false, Dream,
		"Come closer, traveler.");
	ladyOfLake->damage = 25;
	ladyOfLake->worthCost = 0;

	ladyOfLake->onTalk = [ladyOfLake, &world](Player* p, World* w) {
		int worth = p->Worthy;
		bool hasGrail = p->hasGrail;

		if (worth >= 6 && worth <= 9 && !hasGrail) {
			std::cout << "\nThe Lady smiles softly. She extends a hand, offering you a gleaming sword.\n";
			std::cout << "\"You have proven yourself, hero. Take it.\"\n";
			std::cout << "Your hand closes around the hilt. The world feels warm. Real.\n";
			std::cout << "...too real.\n";
			PressEnter();
		}
		else if (worth >= 6 && worth <= 9 && hasGrail) {
			std::cout << "\nThe Lady smiles softly. She extends a hand, offering you a gleaming sword.\n";
			std::cout << "Then the Grail in your pack pulses with light.\n";
			std::cout << "The lake ripples. The Lady falters. The dream tears like wet paper.\n";
			std::cout << "You stand alone in an empty chamber. The sword still in the stone.\n";
			PressEnter();
		}
		else if (worth <= 0 && !hasGrail) {
			std::cout << "\nThe Lady's smile turns cold. A throne rises from the water.\n";
			std::cout << "\"Yes... yes, this is what you wanted, isn't it? Sit. Rule. Forget.\"\n";
			std::cout << "You take the throne. The dream closes around you like a fist.\n";
			PressEnter();
		}
		else if (worth <= 0 && hasGrail) {
			std::cout << "\nThe Grail flares. The dream shatters.\n";
			std::cout << "The Lady's face twists. She knows why you are here.\n";
			std::cout << "\"So be it.\"\n";
			ladyOfLake->isHostile = true;
			PressEnter();
			p->sneakMode = false;
			Combat(*p, *ladyOfLake, world);
			if (p->IsAlive()) {
				std::cout << "\nYou stand over what remains. Her power flows into you.\n";
				PressEnter();
			}
		}
		else {
			std::cout << "The Lady looks at you sadly. \"You should not be here.\"\n";
			return;
		}
		CheckEndingPathB(p);
		};

	Dream->AddEntity(ladyOfLake);
	world.AddEntity(ladyOfLake);

	//INVENTORY ITEMS

	incense->onInteract = [incense](Player* p) {
		std::cout << "You burn the incense. you feel your sins lifting off your shoulders.\n";
		p->Worthy += 2;
		p->inventory.remove(incense);
		std::cout << "(+2 Worthy)\n";
		};

	ritualisticMask->onInteract = [ritualisticMask](Player* p) {
		EquipItem(p, ritualisticMask);
		};

	pristineDagger->onInteract = [pristineDagger](Player* p) {
		EquipItem(p, pristineDagger);
		};

	// EXITS

	// Entrance <-> RA1
	entrance->AddExit(new Exit(Direction::NORTH, RA1, false));
	RA1->AddExit(new Exit(Direction::SOUTH, entrance, false));

	// RA1 <-> RA2
	RA1->AddExit(new Exit(Direction::WEST, RA2, false));
	RA2->AddExit(new Exit(Direction::EAST, RA1, false));

	// RA2 <-> RA3
	RA2->AddExit(new Exit(Direction::NORTH, RA3, false));
	RA3->AddExit(new Exit(Direction::SOUTH, RA2, false));

	// RA2 <-> AltarEntrance
	RA2->AddExit(new Exit(Direction::WEST, AltarEntrance, false));
	AltarEntrance->AddExit(new Exit(Direction::EAST, RA2, false));

	// AltarEntrance <-> RB1
	AltarEntrance->AddExit(new Exit(Direction::WEST, RB1, false));
	RB1->AddExit(new Exit(Direction::EAST, AltarEntrance, false));

	// RB1 <-> RB2
	RB1->AddExit(new Exit(Direction::NORTH, RB2, false));
	RB2->AddExit(new Exit(Direction::SOUTH, RB1, false));

	// RB2 <-> ExcaliburRoom
	RB2->AddExit(new Exit(Direction::NORTH, ExcaliburRoom, false));
	ExcaliburRoom->AddExit(new Exit(Direction::SOUTH, entrance, false));

	// PLAYER
	Player* player = new Player("Hero", entrance);
	player->turnsPlayed = 0;
	world.AddEntity(player);


	// GAME LOOP
	std::cout << "\n\n\n=== Try 'help' for a list of commands ===\n\n";
	std::cout << "=== Cave of Excalibur ===\n";
	player->currentRoom->Describe();

	std::string input;
	bool turnPassed = false;

	while (player->IsAlive()) {
		std::cout << "\n\n> ";
		std::getline(std::cin, input);
		turnPassed = false;

		std::transform(input.begin(), input.end(), input.begin(), ::tolower);
		std::string target = ParseTarget(input);
		Room* roomBefore = player->currentRoom;

		if (input == "quit") {
			break;
		}
		else if (input == "help" || input == "h" || input == "?") {
			std::cout << helpText;
		}
		else if (input == "look" || input == "l" || input == "observe" || input == "survey") {
			HandleLook(player);
		}
		else if (input == "inventory" || input == "inv" || input == "i" || input == "items") {
			HandleInventory(player);
		}
		else if (input == "status" || input == "stats" || input == "hp") {
			HandleStatus(player);
		}
		else if (input == "sneak" || input == "stealth") {
			player->sneakMode = !player->sneakMode;
			if (player->sneakMode)
				std::cout << "You move carefully. Sneak mode ON.\n";
			else
				std::cout << "You move boldly. Courage mode ON.\n";
		}
		else {
			if (ParseMovement(input) == "south" && player->currentRoom == entrance && (player->firstVisit || player->hasObjective)) {
				CheckEndingPathA(player);
				break;
			}
			if (HandleMove(player, input,Boar)) {
				for (Entity* e : player->currentRoom->contains) {
					if (e->type == EntityType::CREATURE) {
						Creature* enemy = static_cast<Creature*>(e);
						if (enemy->isHostile) {
							std::cout << "\n" << enemy->name << " attacks!\n";
							Combat(*player, *enemy, world);
							if (player->IsAlive())
								player->currentRoom->Describe();
							break;
						}
					}
				}
				turnPassed = true;
			}
			else if (input.substr(0, 4) == "take" || input.substr(0, 4) == "grab" || input.substr(0, 3) == "get") {
				HandleTake(player, target);
				turnPassed = true;
			}
			else if (input.substr(0, 4) == "drop" || input.substr(0, 7) == "discard") {
				HandleDrop(player, target);
				turnPassed = true;
			}
			else if (input.substr(0, 7) == "examine" || input.substr(0, 7) == "inspect" || input.substr(0, 5) == "study") {
				HandleExamine(player, target);
			}
			else if (input.substr(0, 3) == "use" || input.substr(0, 5) == "apply" || input.substr(0, 7) == "consume" ||
				input.substr(0, 8) == "interact" || input.substr(0, 5) == "touch" || input.substr(0, 5) == "press" ||
				input.substr(0, 4) == "pull" || input.substr(0, 4) == "push") {
				HandleUse(player, target);
				// Re-describe room after use
				if (player->currentRoom == roomBefore)
					player->currentRoom->Describe();
				turnPassed = true;
			}
			else if (input.substr(0, 4) == "talk" || input.substr(0, 5) == "speak" || input.substr(0, 5) == "greet") {
				HandleTalk(player, target, world);
				// Re-describe room after talk
				player->currentRoom->Describe();
				turnPassed = true;
			}
			else if (input.substr(0, 6) == "attack" || input.substr(0, 3) == "hit") {
				HandleAttack(player, target, world);
				if (player->IsAlive())
					player->currentRoom->Describe();
				turnPassed = true;
			}
			else {
				std::cout << "I don't understand that.\n";
			}
		}

		if (turnPassed) {
			world.Update();
		}
	}

	return 0;
}