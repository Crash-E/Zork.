#pragma once
#include <string>
#include <list>

enum class EntityType {
    ROOM,
    ITEM,
    CREATURE,
    EXIT,
    PLAYER
};

class Entity {
public:
    EntityType  type;
    std::string name;
    std::string description;
    std::list<Entity*> contains;
    Entity* parent = nullptr;

    Entity(EntityType t, const std::string& n, const std::string& desc)
        : type(t), name(n), description(desc) {
    }

    virtual ~Entity() {}

    virtual void Update() = 0;

    void AddEntity(Entity* e) { 
        e->parent = this;
        contains.push_back(e); }

    bool RemoveEntity(Entity* e);

    Entity* FindByName(const std::string& n) const;
};
