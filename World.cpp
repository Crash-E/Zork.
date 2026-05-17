#include "World.h"


World::~World() {
    for (Entity* e : entities) {
        delete e;
    }
    entities.clear();
}


void World::Update() {
    for (Entity* e : entities) {
        e->Update();
    }
}