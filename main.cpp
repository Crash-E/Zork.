#include <iostream>
#include <string>
#include <algorithm>
#include "World.h"
#include "Room.h"
#include "Player.h"
#include "Creature.h"
#include "Exit.h"
#include "WorldBuilder.h"
#include "Commands.h"
#include "Combat.h"
#include "Endings.h"
#include "Utils.h"

int main() {
	const std::string helpText =
		"                               === COMMANDS ===\n \n"
		"   go / walk / move / head        +[direction]  = move in a direction\n"
		"   look / observe / l                           = look around the room\n"
		"   examine / inspect / study      +[target]     = examine something closely\n"
		"   take / grab / pick             +[item]       = pick up an item\n"
		"   drop / discard                 +[item]       = drop an item\n"
		"   talk / speak / greet           +[npc]        = talk to someone\n"
		"   attack / hit                   +[target]     = attack something\n"
		"   use / apply / consume          +[item]       = use an item or equips them\n"
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
	WorldBuilder builder;
	builder.Build(world);

	Player* player = builder.GetPlayer();
	Creature* Boar = builder.GetBoar();

	// Resolve the entrance room (player starts there)
	Room* entrance = player->currentRoom;

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
			else if ((input.substr(0, 4) == "talk" || input.substr(0, 5) == "speak" || input.substr(0, 5) == "greet") && player->currentRoom->name == "Dream") {
				HandleTalk(player, target, world);
				break;
			}
			if (HandleMove(player, input, Boar)) {
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
				if (player->currentRoom == roomBefore)
					player->currentRoom->Describe();
				turnPassed = true;
			}
			else if (input.substr(0, 4) == "talk" || input.substr(0, 5) == "speak" || input.substr(0, 5) == "greet") {
				HandleTalk(player, target, world);
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

	if (!player->IsAlive()) GameOver(player);

	return 0;
}
