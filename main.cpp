#include <iostream>
#include <string>
#include <vector>
#include <algorithm>
#include <random>
#include <ctime>
#include "World.h"
#include "Room.h"
#include "Item.h"
#include "Exit.h"
#include "NPC.h"
#include "Player.h"

// ===== HELPER FUNCTIONS =====

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

Entity* FindInRoomAndContainers(Player* player, const std::string& target) {
	Entity* found = player->currentRoom->FindByName(target);
	if (!found) {
		for (Entity* e : player->currentRoom->contains) {
			if (e->type == EntityType::ITEM) {
				Item* container = static_cast<Item*>(e);
				if (container->isContainer) {
					found = container->FindByName(target);
					if (found) break;
				}
			}
		}
	}
	return found;
}

Entity* FindInInventory(Player* player, const std::string& target) {
	for (Item* item : player->inventory) {
		std::string lower = item->name;
		std::transform(lower.begin(), lower.end(), lower.begin(), ::tolower);
		if (lower == target) return item;
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
	if (player.xp >= player.xpToNextLevel) {
		player.level++;
		player.xp -= player.xpToNextLevel;
		player.xpToNextLevel = player.level * 20;
		std::cout << "\n=== LEVEL UP! You are now level " << player.level << " ===\n";
		std::cout << "Choose a stat to increase:\n";
		std::cout << "[fight] [act] [flee] [item] [evade]\n> ";
		std::string choice;
		std::getline(std::cin, choice);
		if (choice == "fight")      player.Fight++;
		else if (choice == "act")   player.Act++;
		else if (choice == "flee")  player.Flee++;
		else if (choice == "item")  player.ItemStat++;
		else if (choice == "evade") player.Evade++;
		else std::cout << "Invalid choice, no stat increased.\n";
		std::cout << "Stat increased!\n";
	}
}

// ===== COMMAND FUNCTIONS =====

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
	std::cout << "worthiness: " << player->Worthy << "\n";
	std::cout << "Turns played: " << player->turnsPlayed << "\n";
	std::cout << "Inventory value: " << player->TotalValue << "\n";
}

bool HandleMove(Player* player, const std::string& input) {
	std::string direction = ParseMovement(input);
	if (direction.empty()) return false;
	Exit* ex = player->currentRoom->GetExit(direction);
	if (ex) {
		player->firstVisit = false;
		player->currentRoom = ex->destination;
		player->currentRoom->Describe();
		return true;
	}
	std::cout << "No exit that way.\n";
	return false;
}

void HandleTake(Player* player, const std::string& target) {
	Entity* found = FindInRoomAndContainers(player, target);
	if (found && found->type == EntityType::ITEM) {
		Item* item = static_cast<Item*>(found);
		if (item->isFixed) {
			std::cout << item->name << " cannot be taken.\n";
		}
		else {
			player->currentRoom->RemoveEntity(item);
			player->inventory.push_back(item);
			player->TotalValue += item->Value;
			std::cout << "You pick up " << item->name << ".\n";
		}
	}
	else {
		std::cout << "You don't see that here.\n";
	}
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

	if (found && found->type == EntityType::ITEM) {
		Item* asItem = static_cast<Item*>(found);

		if (!asItem->Examine.empty())
			std::cout << asItem->Examine << "\n";
		else
			std::cout << found->description << "\n";

		if (asItem->isContainer && !found->contains.empty()) {
			std::cout << "Inside:\n";
			for (Entity* e : found->contains) {
				if (e->type == EntityType::ITEM) {
					Item* child = static_cast<Item*>(e);
					if (!child->isContainer)
						std::cout << "  - " << child->name << "\n";
					else
						std::cout << "  - " << child->name << " (examine for more)\n";
				}
			}

			for (auto it = found->contains.begin(); it != found->contains.end();) {
				if ((*it)->type == EntityType::ITEM) {
					Item* child = static_cast<Item*>(*it);
					if (!child->isContainer && !child->isFixed) {
						std::cout << "Take " << child->name << "? (yes/no)\n> ";
						std::string takeInput;
						std::getline(std::cin, takeInput);
						if (takeInput == "yes" || takeInput == "y") {
							player->inventory.push_back(child);
							player->TotalValue += child->Value;
							std::cout << "You take " << child->name << ".\n";
							it = found->contains.erase(it);
							continue;
						}
					}
				}
				++it;
			}

			if (found->contains.empty() && found->parent != nullptr) {
				found->parent->RemoveEntity(found);
			}
		}
	}
	else {
		std::cout << "You don't see that.\n";
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
	if (!found || found->type != EntityType::CREATURE) {
		std::cout << "You don't see anyone to talk to.\n";
		return;
	}
	NPC* npc = static_cast<NPC*>(found);
	if (npc->onTalk)
		npc->onTalk(player, &world, false);
	else
		std::cout << npc->name << " says: " << npc->dialogue << "\n";
}

void Combat(Player& player, Creature& enemy, World& world, bool ambush = false) {
	bool extraTurn = player.sneakMode && !ambush;

	while (player.IsAlive() && enemy.IsAlive()) {

		std::cout << "PLAYER ||| HP: " << player.hp << "/" << player.maxHp << "\n" << enemy.name << " ||| HP: " << enemy.hp << "/" << enemy.maxHp << "\n";
		std::cout << "\n[fight] [act] [flee] [item] [status]\n> ";
		std::string action;
		std::getline(std::cin, action);

		if (action == "status") {
			HandleStatus(&player);

		}
		else {

			if (action == "fight") {
				if (CoinFlip(player.Fight, "hit", "miss", "attacking", world)) {
					std::cout << "You hit " << enemy.name << "!\n";
					enemy.TakeDamage(10);
				}
				else {
					std::cout << "You missed!\n";
				}
			}
			else if (action == "flee") {
				if (CoinFlip(player.Flee, "escaping", "blocked", "escaping", world)) {
					std::cout << "You flee!\n";
					return;
				}
				else {
					std::cout << "You couldn't escape!\n";
				}
			}
			else if (action == "item") {
				HandleInventory(&player);
				std::cout << "Use which item?\n> ";
				std::string itemTarget;
				std::getline(std::cin, itemTarget);
				HandleUse(&player, itemTarget);
			}
			else if (action == "act") {
				Entity* found = player.currentRoom->FindByName(enemy.name);
				if (found && found->type == EntityType::CREATURE) {
					NPC* npc = static_cast<NPC*>(found);
					if (npc->onTalk)
						npc->onTalk(&player, &world, true);
					else
						std::cout << "there is nothing you can do\n";
				}
			}
			if (extraTurn) {
				extraTurn = false;
			}
			else if(enemy.hp > 0){
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

	if (enemy.IsAlive()) return;

	if (!enemy.peacefulResolved) {
		player.Worthy -= enemy.worthCost;
		int xpGain = 10;
		if (!player.sneakMode && !ambush) xpGain = (xpGain * 3) / 2;
		player.xp += xpGain;
		std::cout << "\nYou defeated " << enemy.name << "! +" << xpGain << " XP\n";
	}
	else {
		std::cout << "\n you have been spared \n";
	}
	CheckLevelUp(player);
}

void HandleAttack(Player* player, const std::string& target, World& world) {
	Entity* found = player->currentRoom->FindByName(target);
	if (found && found->type == EntityType::CREATURE) {
		Creature* enemy = static_cast<Creature*>(found);
		Combat(*player, *enemy, world);
	}
	else {
		std::cout << "You don't see that here.\n";
	}
}


// ===== ENDINGS =====

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
}

void GameOver(Player* player) {
	std::cout << "\n=== GAME OVER ===\n";
	std::cout << "The darkness takes you. Your story ends here, forgotten in the depths of the cave.\n";
	std::cout << "Perhaps in another life, you would have been worthy.\n";
	std::cout << "\nTurns survived: " << player->turnsPlayed << "\n";
	std::cout << "Worthiness: " << player->Worthy << "\n";
	std::cout << "Treasure collected: " << player->TotalValue << "\n";
}

// ===== MAIN =====

int main() {

	std::string helpText =
		"                               === COMMANDS ===\n \n"
		"   go / walk / move / head        +[direction]  = move in a direction\n"
		"   look / observe / l                           = look around the room\n"
		"   examine  / inspect / study     +[target]     = examine something closely\n"
		"   take  / grab / pick            +[item]       = pick up an item\n"
		"   drop  / leave / discard        +[item]       = drop an item\n"
		"   talk  / speak / greet          +[npc]        = talk to someone\n"
		"   attack  / fight / hit          +[target]     = attack something\n"
		"   use  / apply / consume         +[item]       = use an item\n"
		"   inventory / inv / i                          = show your items\n"
		"   status / stats / hp                          = show your health\n"
		"   sneak / stealth                              = toggle stealth mode (ON by default)"
		"   help / h / ?                                 = show this list\n"
		"   quit                                         = close the game\n";

	World world;


	// ===== ROOMS =====
	Room* entrance = new Room("Cave Entrance",
		"You stand at the mouth of a dark cave. A cold wind blows from within.\n"
		"The light of day barely reaches past the first few feet of stone.\n"
		"yellow plants cover the fields to your right.");

	Room* AltarEntrance = new Room("Altar Entrance",
		"The tunnel opens into a grand stone gate, its archway carved with scenes of battles long forgotten.\n"
		"Two statues flank the entrance.\n To the right, a robed figure clutching a staff, his face worn smooth by time.\n To the left, a broken warrior, sword in hand, stands vigil, missing its head and left arm.\n"
		"you stumble upon a bundle of ropes on the ground.");

	Room* ExcaliburRoom = new Room("ExcaliburRoom",
		"The chamber opens into a vast circular room, its ceiling lost in darkness above.\n"
		"At its center, a sword stands embedded in a weathered stone, surrounded by a shimmering forcefield that distorts the air around it.\n"
		"Three runes are carved into the base of the stone, each glowing faintly — A, G, and B.\n"
		"Two knight statues flank the stone. The first, tall and noble, faces the second.\n"
		"The second turns its gaze toward the sword, as if it has been watching over it for centuries.\n"
		"Behind them both, a larger statue of a crowned figure stares toward the first knight, its expression unreadable.");

	Room* Dream = new Room("Dream",
		"The cave gives way to something impossible. you find yourself in an endless grassfield.\n"
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
		"A worn leather sheath at its waist holds a dagger that catches the torchlight — oddly clean for this place.");

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
		"The smell hits you before you see it — incense, thick and sweet, masking something underneath.\n"
		"Three robed figures stand in a circle at the room's center, their backs to you.\n"
		"Candles flicker across strange symbols carved into the floor.\n"
		"As one, they turn.");

	Creature* testBoar = new Creature("Boar", "A wild boar, tusks gleaming.", 30, true, ambush);
	testBoar->damage = 5;
	testBoar->worthCost = 1;
	ambush->AddEntity(testBoar);
	world.AddEntity(testBoar);

	// ===== ITEMS =====
	Item* poisonHerbs = new Item("Yellow Herbs",
		"A cluster of yellow herbs. Often used by hunters for traps.",
		"Herbs used to sedate beasts during hunts. Strong enough to knock a person out for hours.",
		false, false, 1, "eat the herbs");

	Item* Rope = new Item("Rope",
		"A cluster of ropes, always useful.", "", false, true, 2);

	Item* skeleton = new Item("Skeleton",
		"A skeleton slumps against the wall.",
		"Old brittle bones, armor rusted to nothing. His hand seems to be clutching something. You notice a sheath around his waist.",
		true, false, 0, "", true);

	Item* hand = new Item("Hand",
		"The skeleton's hand seems to be clutching something.",
		"The fingers are curled tight around something red, a crumpled note wedged beneath the thumb.",
		true, false, 0, "", true);

	Item* sheath = new Item("Sheath",
		"A worn leather sheath at its waist.",
		"Cracked leather, but the blade inside catches the light perfectly — untouched by time.",
		true, false, 0, "", true);

	Item* potion = new Item("Potion",
		"A small vial of red liquid. Looks like it could heal wounds.", "", false, false, 2);

	Item* wornNote = new Item("Worn Note",
		"A crumpled piece of paper. The ink is smudged beyond repair.",
		"Most of the note is illegible, you can make out \"make sure to st*- safe. love Mari*-\"",
		false, false, 0);

	Item* pristineDagger = new Item("Pristine Dagger",
		"A dagger in perfect condition. Oddly clean for such a dark place.", "", false, false, 3);


	poisonHerbs->onInteract = [](Player* p) {
		std::cout << "You eat the herbs. They taste bitter.\nYou pass out for several hours.\n";
		p->turnsPlayed += 15;
		};

	// ===== ITEM PLACEMENT =====
	entrance->AddEntity(poisonHerbs);
	AltarEntrance->AddEntity(Rope);

	hand->AddEntity(potion);
	hand->AddEntity(wornNote);
	sheath->AddEntity(pristineDagger);
	skeleton->AddEntity(hand);
	skeleton->AddEntity(sheath);
	NarrowTunnel->AddEntity(skeleton);

	// ===== RANDOM ROOM SELECTION =====
	std::vector<Room*> raPool = { ambush, NarrowTunnel, Waterfall };
	std::vector<Room*> rbPool = { Catacombs, RitualRoom };

	std::shuffle(raPool.begin(), raPool.end(), world.rng);
	std::shuffle(rbPool.begin(), rbPool.end(), world.rng);

	Room* RA1 = raPool[0];
	Room* RA2 = raPool[1];
	Room* RA3 = raPool[2];
	Room* RB1 = rbPool[0];
	Room* RB2 = rbPool[1];

	// ===== EXITS =====
	Exit* e1 = new Exit(Direction::NORTH, RA1, false);
	Exit* e2 = new Exit(Direction::SOUTH, entrance, false);
	entrance->AddExit(e1);
	RA1->AddExit(e2);

	Exit* e3 = new Exit(Direction::WEST, RA2, false);
	Exit* e4 = new Exit(Direction::EAST, RA1, false);
	RA1->AddExit(e3);
	RA2->AddExit(e4);

	Exit* e5 = new Exit(Direction::NORTH, RA3, false);
	Exit* e6 = new Exit(Direction::SOUTH, RA2, false);
	Exit* e7 = new Exit(Direction::WEST, AltarEntrance, false);
	Exit* e8 = new Exit(Direction::EAST, RA2, false);
	RA2->AddExit(e5);
	RA3->AddExit(e6);
	RA2->AddExit(e7);
	AltarEntrance->AddExit(e8);

	Exit* e9 = new Exit(Direction::WEST, RB1, false);
	Exit* e10 = new Exit(Direction::EAST, AltarEntrance, false);
	AltarEntrance->AddExit(e9);
	RB1->AddExit(e10);

	Exit* e11 = new Exit(Direction::NORTH, RB2, false);
	Exit* e12 = new Exit(Direction::SOUTH, RB1, false);
	RB1->AddExit(e11);
	RB2->AddExit(e12);

	Exit* e13 = new Exit(Direction::NORTH, ExcaliburRoom, false);
	Exit* e14 = new Exit(Direction::SOUTH, RB2, false);
	RB2->AddExit(e13);
	ExcaliburRoom->AddExit(e14);

	Exit* e17 = new Exit(Direction::UP, Dream, false);

	//===== NPCs =====
	NPC* injuredMan = new NPC("Injured Man",
		"A man slumped against a rock, barely conscious.",
		30, false, Waterfall,
		"he seems to be fading in and out of consciousness.");
	injuredMan->damage = 0;
	injuredMan->worthCost = 2;
	injuredMan->onTalk = [](Player* p, World* w, bool InCombat) {
		std::cout << "he seems to be fading in and out of consciousness.";
		std::cout << "\n[wake] [give] [steal] [leave]\n> ";
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
					// TODO: give rusty shield reward
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
			// TODO: give steal loot
		}
		else if (choice == "leave") {
			std::cout << "You leave him be.\n";
		}
		};

	Waterfall->AddEntity(injuredMan);
	world.AddEntity(injuredMan);

	NPC* greenKnight = new NPC("Green Knight",
		"A towering knight clad in green armor, his axe resting on his shoulder.",
		80, true, AltarEntrance,
		"You dare approach the gate? State your purpose or prepare to fight.");
	greenKnight->damage = 15;
	greenKnight->worthCost = 3;
	AltarEntrance->AddEntity(greenKnight);
	world.AddEntity(greenKnight);
	greenKnight->onTalk = [greenKnight, AltarEntrance](Player* p, World* w, bool inCombat) {
		std::cout << "\"this is no time to talk, yield or fight!\"\n";
		std::cout << "\"[Yield] [Fight]\"\n";
		std::string choice;
		std::getline(std::cin, choice);
		if (choice == "yield" || choice == "y") {
			greenKnight->actCount++;
			if (greenKnight->actCount == 1) {
				std::cout << "\"Interesting. You hesitate. Show me more.\"\n";
				std::cout << "The knight steps back, but his axe is still raised.\n";
			}
			else {
				std::cout << "You lower your weapon. The knight nods slowly.\n";
				std::cout << "\"Wisdom. You may pass.\"\n";
				std::cout << "The knight's ghostly figure begins to fade away.\n";
				greenKnight->isHostile = false;
				greenKnight->peacefulResolved = true;
				greenKnight->hp = 0;
				AltarEntrance->RemoveEntity(greenKnight);
			}
		}
		else if (choice == "fight" || choice == "f") 
			std::cout << "You prepare to fight the Green Knight.\n";
		else 			
			std::cout << "The Green Knight doesn't understand your choice.\n";
		};

	// ===== WORLD ENTITIES =====
	world.AddEntity(hand);
	world.AddEntity(sheath);
	world.AddEntity(skeleton);
	world.AddEntity(potion);
	world.AddEntity(wornNote);
	world.AddEntity(pristineDagger);
	world.AddEntity(poisonHerbs);
	world.AddEntity(Rope);
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

	Player* player = new Player("Hero", entrance);
	player->turnsPlayed = 0;
	world.AddEntity(player);

	// ===== GAME LOOP =====
	std::cout << "\n\n\n === Try 'help' for a list of commands === \n\n\n";
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
		else {
			if (ParseMovement(input) == "south" && player->currentRoom == entrance && player->firstVisit) {
				std::cout << "\n=== Ending ?/7 - Coward's Way Out ===\n \n";
				std::cout << "  You turn back at the entrance. The cave yawns behind you, indifferent.\n";
				std::cout << "  No shame in it. The sword was never going anywhere.\n";
				std::cout << "  At least you got out with your life.\n";
				std::cout << "  Plenty of people who went in didn't.\n";
				break;
			}
			if (HandleMove(player, input)) {
				for (Entity* e : player->currentRoom->contains) {
					if (e->type == EntityType::CREATURE) {
						Creature* enemy = static_cast<Creature*>(e);
						if (enemy->isHostile) {
							std::cout << "\n" << enemy->name << " attacks!\n";
							Combat(*player, *enemy, world, true);
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
			else if (input.substr(0, 4) == "drop" || input.substr(0, 5) == "leave") {
				HandleDrop(player, target);
				turnPassed = true;
			}
			else if (input.substr(0, 7) == "examine" || input.substr(0, 7) == "inspect" || input.substr(0, 5) == "study" || input.substr(0, 7) == "look at") {
				HandleExamine(player, target);
			}
			else if (input.substr(0, 3) == "use" || input.substr(0, 5) == "apply" || input.substr(0, 4) == "wear" || input.substr(0, 8) == "interact" || input.substr(0, 5) == "touch" || input.substr(0, 5) == "press" || input.substr(0, 4) == "pull" || input.substr(0, 4) == "push") {
				HandleUse(player, target);
				turnPassed = true;
			}
			else if (input.substr(0, 4) == "talk" || input.substr(0, 5) == "speak" || input.substr(0, 5) == "greet") {
				HandleTalk(player, target, world);
				turnPassed = true;
			}
			else if (input.substr(0, 6) == "attack" || input.substr(0, 5) == "fight" || input.substr(0, 3) == "hit") {
				HandleAttack(player, target, world);
				turnPassed = true;
			}
			else if (input == "sneak" || input == "stealth") {
				player->sneakMode = !player->sneakMode;
				if (player->sneakMode)
					std::cout << "You move carefully. Sneak mode ON.\n";
				else
					std::cout << "You move boldly. Courage mode ON. \n";
			}
			else {
				std::cout << "I don't understand that.\n";
			}
		}

		if (turnPassed) {
			world.Update();
			player->turnsPlayed++;
		}
	}

	return 0;
}