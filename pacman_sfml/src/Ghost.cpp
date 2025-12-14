#include "Ghost.h"

#include "Direction.h"
#include "Map.h"

#include <algorithm>
#include <cmath>
#include <limits>
#include <queue>
#include <vector>

namespace {
int bfsDistance(const Map& map, TileCoord start, TileCoord target) {
    if (start == target) {
        return 0;
    }

    const int w = map.width();
    const int h = map.height();
    if (w <= 0 || h <= 0) {
        return -1;
    }

    auto idx = [w](TileCoord t) {
        return t.y * w + t.x;
    };

    std::vector<int> dist(static_cast<std::size_t>(w * h), -1);
    std::queue<TileCoord> q;

    if (!map.isWalkable(start.x, start.y) || !map.isWalkable(target.x, target.y)) {
        return -1;
    }

    dist[static_cast<std::size_t>(idx(start))] = 0;
    q.push(start);

    const Direction dirs[4] = {Direction::Up, Direction::Left, Direction::Down, Direction::Right};

    while (!q.empty()) {
        const TileCoord cur = q.front();
        q.pop();

        const int base = dist[static_cast<std::size_t>(idx(cur))];

        for (Direction d : dirs) {
            const TileCoord nxt = map.nextTile(cur, d);
            if (nxt.x < 0 || nxt.y < 0 || nxt.x >= w || nxt.y >= h) {
                continue;
            }
            if (!map.isWalkable(nxt.x, nxt.y)) {
                continue;
            }

            const std::size_t ni = static_cast<std::size_t>(idx(nxt));
            if (dist[ni] != -1) {
                continue;
            }

            dist[ni] = base + 1;
            if (nxt == target) {
                return dist[ni];
            }
            q.push(nxt);
        }
    }

    return -1;
}
}

Ghost::Ghost(GhostId id, sf::Color baseColor) : mId(id), mBaseColor(baseColor) {}

void Ghost::reset(TileCoord spawn, TileCoord scatterCorner, const Map& map, float tileSize) {
    mSpawn = spawn;
    mScatterCorner = scatterCorner;
    mPos = map.tileCenterWorld(spawn, tileSize);
    mDir = Direction::Left;
    mMode = GhostMode::Scatter;
}

void Ghost::setMode(GhostMode mode) {
    // Do not allow leaving Eaten until we reach spawn.
    if (mMode == GhostMode::Eaten && mode != GhostMode::Eaten) {
        return;
    }
    mMode = mode;
}

void Ghost::reverse() {
    if (mMode == GhostMode::Eaten) {
        return;
    }
    mDir = opposite(mDir);
}

TileCoord Ghost::currentTile(const Map& map, float tileSize) const {
    return map.worldToTile(mPos, tileSize);
}

bool Ghost::canStep(Direction d, TileCoord from, const Map& map) const {
    if (d == Direction::None) {
        return false;
    }

    const TileCoord to = map.nextTile(from, d);
    return map.isWalkable(to.x, to.y);
}

Direction Ghost::chooseDirection(TileCoord from, const Map& map, TileCoord target, std::mt19937& rng) const {
    Direction candidates[4] = {Direction::Up, Direction::Down, Direction::Left, Direction::Right};

    std::vector<Direction> possible;
    possible.reserve(4);

    for (Direction d : candidates) {
        if (!canStep(d, from, map)) {
            continue;
        }
        possible.push_back(d);
    }

    if (possible.empty()) {
        return Direction::None;
    }

    // Avoid reversing unless forced.
    if (possible.size() > 1) {
        const Direction rev = opposite(mDir);
        possible.erase(std::remove(possible.begin(), possible.end(), rev), possible.end());
        if (possible.empty()) {
            possible.push_back(rev);
        }
    }

    if (mMode == GhostMode::Frightened) {
        std::uniform_int_distribution<int> dist(0, static_cast<int>(possible.size()) - 1);
        return possible[static_cast<size_t>(dist(rng))];
    }

    // Modernized classic: choose direction by shortest-path distance (BFS) to target.
    // Tie-break order uses the classic preference: Up, Left, Down, Right.
    const Direction pref[4] = {Direction::Up, Direction::Left, Direction::Down, Direction::Right};

    Direction best = possible.front();
    int bestDist = std::numeric_limits<int>::max();

    for (Direction dPref : pref) {
        auto it = std::find(possible.begin(), possible.end(), dPref);
        if (it == possible.end()) {
            continue;
        }

        const Direction d = *it;
        const TileCoord to = map.nextTile(from, d);
        const int dist = bfsDistance(map, to, target);

        const int score = (dist < 0) ? std::numeric_limits<int>::max() : dist;
        if (score < bestDist) {
            bestDist = score;
            best = d;
        }
    }

    return best;
}

void Ghost::update(float dt, const Map& map, float tileSize, TileCoord target, std::mt19937& rng) {
    // Speed by mode.
    float speedTiles = mSpeedTilesPerSecond;
    if (mMode == GhostMode::Frightened) speedTiles = 4.0f;
    if (mMode == GhostMode::Eaten) speedTiles = 8.5f;

    const TileCoord tile = currentTile(map, tileSize);
    const sf::Vector2f center = map.tileCenterWorld(tile, tileSize);
    const sf::Vector2f delta = {mPos.x - center.x, mPos.y - center.y};

    const float epsilon = tileSize * 0.10f;
    const bool nearCenter = (std::abs(delta.x) < epsilon) && (std::abs(delta.y) < epsilon);

    if (nearCenter) {
        mPos = center;

        if (mMode == GhostMode::Eaten) {
            target = mSpawn;
        }

        // Reached home: leave Eaten and resume Scatter by default.
        if (mMode == GhostMode::Eaten && tile == mSpawn) {
            mMode = GhostMode::Scatter;
        }

        const Direction newDir = chooseDirection(tile, map, target, rng);
        if (newDir != Direction::None) {
            mDir = newDir;
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
    const float speed = speedTiles * tileSize;

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
}

sf::Color Ghost::color() const {
    if (mMode == GhostMode::Frightened) {
        return sf::Color(60, 60, 255);
    }
    if (mMode == GhostMode::Eaten) {
        return sf::Color(220, 220, 255);
    }
    return mBaseColor;
}
