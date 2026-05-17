#pragma once
#include "Entity.h"

class Room;

enum class Direction { NORTH, SOUTH, EAST, WEST, UP, DOWN };

class Exit : public Entity {
public:
    Direction   direction;
    Room* destination;
    bool        isLocked;
    std::string requiredKey;

    Exit(Direction dir, Room* dest, bool locked = false, const std::string& key = "")
        : Entity(EntityType::EXIT, DirectionToString(dir), "An exit leading " + DirectionToString(dir)),
        direction(dir), destination(dest), isLocked(locked), requiredKey(key) {
    }

    void Update() override {}

    static std::string DirectionToString(Direction d) {
        switch (d) {
        case Direction::NORTH: return "north";
        case Direction::SOUTH: return "south";
        case Direction::EAST:  return "east";
        case Direction::WEST:  return "west";
        case Direction::UP:    return "up";
        case Direction::DOWN:  return "down";
        default:               return "unknown";
        }
    }

    static bool ParseDirection(const std::string& s, Direction& out) {
        if (s == "north" || s == "n") { out = Direction::NORTH; return true; }
        if (s == "south" || s == "s") { out = Direction::SOUTH; return true; }
        if (s == "east" || s == "e") { out = Direction::EAST;  return true; }
        if (s == "west" || s == "w") { out = Direction::WEST;  return true; }
        if (s == "up" || s == "u") { out = Direction::UP;    return true; }
        if (s == "down" || s == "d") { out = Direction::DOWN;  return true; }
        return false;
    }
};