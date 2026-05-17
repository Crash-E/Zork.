#include "NPC.h"
#include "Room.h"
#include <iostream>

void NPC::Update() {
    // Patrol behavior - move to next room in patrol list each turn
    if (!patrolRooms.empty() && IsAlive()) {
        patrolIndex = (patrolIndex + 1) % patrolRooms.size();
        location = patrolRooms[patrolIndex];
    }
}

void NPC::Speak() const {
    std::cout << name << " says: " << dialogue << "\n";
}