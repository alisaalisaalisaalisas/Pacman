#include "Player.h"

#include "Direction.h"
#include "Map.h"

#include <algorithm>
#include <cmath>
#include <iostream>


void Player::reset(TileCoord spawn, const Map& map, float tileSize) {
    mPos = map.tileCenterWorld(spawn, tileSize);
    mDir = Direction::Left;
    mRequestedDirection = Direction::Left;
    mMouthPhase = 0.f;
    mMouthOpen01 = 1.f;
}

TileCoord Player::currentTile(const Map& map, float tileSize) const {
    return map.worldToTile(mPos, tileSize);
}

bool Player::canStep(Direction d, TileCoord from, const Map& map) const {
    if (d == Direction::None) {
        return false;
    }

    const TileCoord to = map.nextTile(from, d);
    return map.isWalkable(to.x, to.y);
}

void Player::update(float dt, const Map& map, float tileSize) {
    const TileCoord tile = currentTile(map, tileSize);
    const sf::Vector2f center = map.tileCenterWorld(tile, tileSize);
    const sf::Vector2f delta = {mPos.x - center.x, mPos.y - center.y};

    const float epsilon = tileSize * 0.10f;
    const bool nearCenter = (std::abs(delta.x) < epsilon) && (std::abs(delta.y) < epsilon);

    if (nearCenter) {
        // Snap to center to avoid drift.
        mPos = center;

        if (canStep(mRequestedDirection, tile, map)) {
            mDir = mRequestedDirection;
        }

        if (!canStep(mDir, tile, map)) {
            mDir = Direction::None;
        }

        // Apply tunnel warp only from explicit endpoints.
        TileCoord warped;
        if (map.tryWarp(tile, mDir, warped)) {
            mPos = map.tileCenterWorld(warped, tileSize);
        }
    }

    const sf::Vector2f dir = dirToUnitVector(mDir);
    const float speed = mSpeedTilesPerSecond * tileSize;

    // Probe slightly ahead to simulate a smaller hitbox against walls.
    const float probe = tileSize * 0.20f;
    sf::Vector2f newPos = mPos;
    newPos.x += dir.x * speed * dt;
    newPos.y += dir.y * speed * dt;
    const sf::Vector2f probePos{newPos.x + dir.x * probe, newPos.y + dir.y * probe};
    const TileCoord newTile = map.worldToTile(probePos, tileSize);

    if (map.isWalkable(newTile.x, newTile.y)) {
        mPos = newPos;
    } else if (nearCenter) {
        mPos = center;
        mDir = Direction::None;
    }

    // Simple mouth animation, faster while moving.
    const float moveFactor = (mDir == Direction::None) ? 0.5f : 1.0f;
    mMouthPhase += dt * 12.0f * moveFactor;
    const float s = std::sin(mMouthPhase);
    mMouthOpen01 = std::clamp((s + 1.0f) * 0.5f, 0.0f, 1.0f);
}
