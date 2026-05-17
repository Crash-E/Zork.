#include <iostream>
#include <string>
#include "World.h"
#include "Room.h"
#include "Item.h"
#include "Exit.h"
#include "NPC.h"
#include "Player.h"

int main() {
    World world;

    // Create rooms
    Room* entrance = new Room("Cave Entrance", "You stand at the mouth of a dark cave.");
    Room* tunnel = new Room("Narrow Tunnel", "A tight tunnel stretches ahead.");

    // Create exits connecting the rooms
    Exit* exitNorth = new Exit(Direction::NORTH, tunnel, false);
    Exit* exitSouth = new Exit(Direction::SOUTH, entrance, false);

    entrance->AddExit(exitNorth);
    tunnel->AddExit(exitSouth);

    // Create an item
    Item* torch = new Item("Torch", "A lit torch. It lights the way.", false, false);
    entrance->AddEntity(torch);

    // Add everything to world
    world.AddEntity(entrance);
    world.AddEntity(tunnel);
    world.AddEntity(exitNorth);
    world.AddEntity(exitSouth);
    world.AddEntity(torch);

    // Create player
    Player* player = new Player("Hero", entrance);
    world.AddEntity(player);

    // Game loop
    std::string input;
    std::cout << "Welcome to the Cave!\n";
    player->currentRoom->Describe();

    while (player->IsAlive()) {
        std::cout << "\n> ";
        std::getline(std::cin, input);

        if (input == "quit") break;

        if (input == "look") {
            player->currentRoom->Describe();
        }
        else if (input == "go north" || input == "go n") {
            Exit* ex = player->currentRoom->GetExit("north");
            if (ex) player->currentRoom = ex->destination;
            else std::cout << "No exit that way.\n";
            player->currentRoom->Describe();
        }
        else if (input == "go south" || input == "go s") {
            Exit* ex = player->currentRoom->GetExit("south");
            if (ex) player->currentRoom = ex->destination;
            else std::cout << "No exit that way.\n";
            player->currentRoom->Describe();
        }
        else {
            std::cout << "I don't understand that.\n";
        }

        world.Update();
    }

    return 0;
}