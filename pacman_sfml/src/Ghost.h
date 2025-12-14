#pragma once

#include "Types.h"
#include <SFML/Graphics/Color.hpp>
#include <SFML/System/Vector2.hpp>

#include <random>

class Map;

enum class GhostId : std::uint8_t {
    Blinky,
    Pinky,
    Inky,
    Clyde,
};

enum class GhostMode : std::uint8_t {
    Scatter,
    Chase,
    Frightened,
    Eaten,
};

class Ghost {
public:
    Ghost(GhostId id, sf::Color baseColor);

    void reset(TileCoord spawn, TileCoord scatterCorner, const Map& map, float tileSize);

    void setMode(GhostMode mode);
    GhostMode mode() const { return mMode; }

    void reverse();

    void update(float dt, const Map& map, float tileSize, TileCoord target, std::mt19937& rng);

    sf::Vector2f position() const { return mPos; }
    Direction direction() const { return mDir; }

    TileCoord currentTile(const Map& map, float tileSize) const;

    sf::Color color() const;
    GhostId id() const { return mId; }

    TileCoord spawnTile() const { return mSpawn; }
    TileCoord scatterCorner() const { return mScatterCorner; }

private:
    bool canStep(Direction d, TileCoord from, const Map& map) const;

    Direction chooseDirection(TileCoord from, const Map& map, TileCoord target, std::mt19937& rng) const;

    GhostId mId;
    sf::Color mBaseColor;

    sf::Vector2f mPos{0.f, 0.f};
    Direction mDir = Direction::Left;
    GhostMode mMode = GhostMode::Scatter;

    TileCoord mSpawn{1, 1};
    TileCoord mScatterCorner{1, 1};

    float mSpeedTilesPerSecond = 7.0f;
};
