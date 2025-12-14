#pragma once

#include "Types.h"
#include <SFML/System/Vector2.hpp>

class Map;

class Player {
public:
    void reset(TileCoord spawn, const Map& map, float tileSize);

    void requestDirection(Direction d) { mRequestedDirection = d; }
    void update(float dt, const Map& map, float tileSize);

    sf::Vector2f position() const { return mPos; }
    Direction direction() const { return mDir; }

    float radius(float tileSize) const { return tileSize * 0.42f; }
    float mouthOpen01() const { return mMouthOpen01; }

    TileCoord currentTile(const Map& map, float tileSize) const;

private:
    bool canStep(Direction d, TileCoord from, const Map& map) const;

    sf::Vector2f mPos{0.f, 0.f};
    Direction mDir = Direction::Left;
    Direction mRequestedDirection = Direction::Left;

    float mSpeedTilesPerSecond = 8.0f;

    float mMouthPhase = 0.f;
    float mMouthOpen01 = 1.f;
};
