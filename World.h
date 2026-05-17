#pragma once
#include "Entity.h"
#include <vector>

class World {
public:
    std::vector<Entity*> entities;

    World() {}
    ~World();

    void AddEntity(Entity* e) { entities.push_back(e); }
    void Update();
};