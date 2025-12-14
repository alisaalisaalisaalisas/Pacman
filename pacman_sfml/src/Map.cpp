#include "Map.h"

#include "Direction.h"

#include <algorithm>
#include <array>
#include <cctype>
#include <fstream>
#include <iostream>
#include <vector>

bool Map::loadFromFile(const std::string& path) {
    std::ifstream in(path);
    if (!in) {
        std::cerr << "Failed to open map: " << path << "\n";
        return false;
    }

    mGrid.clear();
    std::string line;
    while (std::getline(in, line)) {
        // Keep spaces, ignore empty lines at file end.
        if (!line.empty() && (line.back() == '\r')) {
            line.pop_back();
        }
        if (!line.empty()) {
            mGrid.push_back(line);
        }
    }

    if (mGrid.empty()) {
        std::cerr << "Map is empty: " << path << "\n";
        return false;
    }

    normalizeToRectangle();

    if (mWidth != 28 || mHeight != 31) {
        std::cerr << "Warning: expected 28x31 map, got " << mWidth << "x" << mHeight << " (" << path << ")\n";
    }

    mWarps.clear();
    std::array<std::vector<TileCoord>, 26> warpPoints;

    mFruitSpawn = {mWidth / 2, mHeight / 2};
    bool fruitFound = false;

    // Parse spawns and warp markers; clean markers into empty space.
    for (int y = 0; y < mHeight; ++y) {
        for (int x = 0; x < mWidth; ++x) {
            const char c = cell(x, y);

            // Check for warp markers (lowercase a-z, but NOT 'o' which is power pellet)
            if (c >= 'a' && c <= 'z' && c != 'o') {
                warpPoints[static_cast<std::size_t>(c - 'a')].push_back({x, y});
                setCell(x, y, ' ');
                continue;
            }

            switch (c) {
            case 'S': mPlayerSpawn = {x, y}; setCell(x, y, ' '); break;
            case 'B': mGhostSpawnBlinky = {x, y}; setCell(x, y, ' '); break;
            case 'P': mGhostSpawnPinky = {x, y}; setCell(x, y, ' '); break;
            case 'I': mGhostSpawnInky = {x, y}; setCell(x, y, ' '); break;
            case 'C': mGhostSpawnClyde = {x, y}; setCell(x, y, ' '); break;
            case 'F': mFruitSpawn = {x, y}; fruitFound = true; setCell(x, y, ' '); break;
            default: break;
            }
        }
    }

    if (!fruitFound || !isWalkable(mFruitSpawn.x, mFruitSpawn.y)) {
        for (int y = 0; y < mHeight; ++y) {
            for (int x = 0; x < mWidth; ++x) {
                if (isWalkable(x, y)) {
                    mFruitSpawn = {x, y};
                    fruitFound = true;
                    break;
                }
            }
            if (fruitFound) {
                break;
            }
        }
    }

    for (std::size_t i = 0; i < warpPoints.size(); ++i) {
        if (warpPoints[i].empty()) {
            continue;
        }
        if (warpPoints[i].size() != 2) {
            const char id = static_cast<char>('a' + static_cast<int>(i));
            std::cerr << "Warp marker '" << id << "' must appear exactly twice (found " << warpPoints[i].size() << ")\n";
            continue;
        }

        TileCoord a = warpPoints[i][0];
        TileCoord b = warpPoints[i][1];
        const char id = static_cast<char>('a' + static_cast<int>(i));
        if (a.x <= b.x) {
            mWarps.push_back({id, a, b});
        } else {
            mWarps.push_back({id, b, a});
        }
    }

    return true;
}

void Map::normalizeToRectangle() {
    mHeight = static_cast<int>(mGrid.size());
    mWidth = 0;
    for (const auto& row : mGrid) {
        mWidth = std::max(mWidth, static_cast<int>(row.size()));
    }

    for (auto& row : mGrid) {
        if (static_cast<int>(row.size()) < mWidth) {
            row.resize(static_cast<size_t>(mWidth), ' ');
        }
    }
}

char Map::cell(int x, int y) const {
    if (x < 0 || y < 0 || x >= mWidth || y >= mHeight) {
        return '#';
    }
    return mGrid[static_cast<size_t>(y)][static_cast<size_t>(x)];
}

void Map::setCell(int x, int y, char c) {
    if (x < 0 || y < 0 || x >= mWidth || y >= mHeight) {
        return;
    }
    mGrid[static_cast<size_t>(y)][static_cast<size_t>(x)] = c;
}

bool Map::isWall(int x, int y) const {
    return cell(x, y) == '#';
}

bool Map::isWalkable(int x, int y) const {
    return !isWall(x, y);
}

bool Map::hasDotsOrPellets() const {
    for (const auto& row : mGrid) {
        for (char c : row) {
            if (c == '.' || c == 'o') {
                return true;
            }
        }
    }
    return false;
}

sf::Vector2f Map::tileCenterWorld(TileCoord t, float tileSize) const {
    return {(t.x + 0.5f) * tileSize, (t.y + 0.5f) * tileSize};
}

TileCoord Map::worldToTile(sf::Vector2f world, float tileSize) const {
    const int x = static_cast<int>(world.x / tileSize);
    const int y = static_cast<int>(world.y / tileSize);
    return {x, y};
}

bool Map::tryWarp(TileCoord from, Direction dir, TileCoord& out) const {
    if (dir != Direction::Left && dir != Direction::Right) {
        return false;
    }

    for (const auto& w : mWarps) {
        if (dir == Direction::Left && from == w.left) {
            out = w.right;
            return true;
        }
        if (dir == Direction::Right && from == w.right) {
            out = w.left;
            return true;
        }
    }

    return false;
}

TileCoord Map::nextTile(TileCoord from, Direction dir) const {
    const sf::Vector2i dd = dirToGridDelta(dir);
    TileCoord to{from.x + dd.x, from.y + dd.y};

    TileCoord warped;
    if (tryWarp(from, dir, warped)) {
        return warped;
    }

    return to;
}
