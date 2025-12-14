#pragma once

#include <cstdint>

struct TileCoord {
    int x = 0;
    int y = 0;
};

inline bool operator==(const TileCoord& a, const TileCoord& b) {
    return a.x == b.x && a.y == b.y;
}

enum class Direction : std::uint8_t {
    None,
    Up,
    Down,
    Left,
    Right,
};
