#pragma once
#include "Entity.h"
#include <vector>
#include <random>
#include <algorithm>

class Room;

class World {
public:
    std::vector<Entity*> entities;
    std::mt19937 rng;

    World() : rng(static_cast<unsigned int>(time(0))) {}
    ~World();

    void AddEntity(Entity* e) { entities.push_back(e); }
    void Update();

    std::vector<Room*> SelectRooms(std::vector<Room*>& pool, int count) {
        std::shuffle(pool.begin(), pool.end(), rng);
        return std::vector<Room*>(pool.begin(), pool.begin() + count);
    }
};