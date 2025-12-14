#pragma once

#include "Types.h"
#include <SFML/System/Vector2.hpp>
#include <string>
#include <vector>

class Map {
public:
    bool loadFromFile(const std::string& path);

    int width() const { return mWidth; }
    int height() const { return mHeight; }

    char cell(int x, int y) const;
    void setCell(int x, int y, char c);

    bool isWall(int x, int y) const;
    bool isWalkable(int x, int y) const;

    bool hasDotsOrPellets() const;

    TileCoord playerSpawn() const { return mPlayerSpawn; }
    TileCoord ghostSpawnBlinky() const { return mGhostSpawnBlinky; }
    TileCoord ghostSpawnPinky() const { return mGhostSpawnPinky; }
    TileCoord ghostSpawnInky() const { return mGhostSpawnInky; }
    TileCoord ghostSpawnClyde() const { return mGhostSpawnClyde; }
    TileCoord fruitSpawn() const { return mFruitSpawn; }

    // World <-> grid helpers (map-local coordinates)
    sf::Vector2f tileCenterWorld(TileCoord t, float tileSize) const;
    TileCoord worldToTile(sf::Vector2f world, float tileSize) const;

    // Warp tunnels (authored in the map file).
    // Rule: when standing on the left endpoint and moving Left => teleport to right endpoint.
    //       when standing on the right endpoint and moving Right => teleport to left endpoint.
    bool tryWarp(TileCoord from, Direction dir, TileCoord& out) const;
    TileCoord nextTile(TileCoord from, Direction dir) const;

private:
    struct Warp {
        char id = 0;
        TileCoord left{0, 0};
        TileCoord right{0, 0};
    };

    void normalizeToRectangle();

    std::vector<std::string> mGrid;
    int mWidth = 0;
    int mHeight = 0;

    std::vector<Warp> mWarps;

    TileCoord mPlayerSpawn{1, 1};
    TileCoord mGhostSpawnBlinky{1, 1};
    TileCoord mGhostSpawnPinky{1, 1};
    TileCoord mGhostSpawnInky{1, 1};
    TileCoord mGhostSpawnClyde{1, 1};
    TileCoord mFruitSpawn{13, 17};
};
