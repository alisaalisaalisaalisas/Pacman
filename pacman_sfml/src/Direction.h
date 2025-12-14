#pragma once

#include "Types.h"
#include <SFML/System/Vector2.hpp>

inline sf::Vector2i dirToGridDelta(Direction d) {
    switch (d) {
    case Direction::Up: return {0, -1};
    case Direction::Down: return {0, 1};
    case Direction::Left: return {-1, 0};
    case Direction::Right: return {1, 0};
    default: return {0, 0};
    }
}

inline sf::Vector2f dirToUnitVector(Direction d) {
    switch (d) {
    case Direction::Up: return {0.f, -1.f};
    case Direction::Down: return {0.f, 1.f};
    case Direction::Left: return {-1.f, 0.f};
    case Direction::Right: return {1.f, 0.f};
    default: return {0.f, 0.f};
    }
}

inline Direction opposite(Direction d) {
    switch (d) {
    case Direction::Up: return Direction::Down;
    case Direction::Down: return Direction::Up;
    case Direction::Left: return Direction::Right;
    case Direction::Right: return Direction::Left;
    default: return Direction::None;
    }
}
