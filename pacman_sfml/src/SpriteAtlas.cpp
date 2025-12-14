#include "SpriteAtlas.h"

#include <nlohmann/json.hpp>

#include <fstream>
#include <iostream>

bool SpriteAtlas::loadFromFiles(const std::string& imagePath, const std::string& jsonPath) {
    mFrames.clear();

    sf::Image img;
    if (!img.loadFromFile(imagePath)) {
        std::cerr << "Failed to load atlas image: " << imagePath << "\n";
        return false;
    }
    // Treat black as transparent to remove solid backgrounds in atlas BMP.
    img.createMaskFromColor(sf::Color::Black);

    if (!mTexture.loadFromImage(img)) {
        std::cerr << "Failed to create atlas texture from image: " << imagePath << "\n";
        return false;
    }
    mTexture.setSmooth(false);

    std::ifstream in(jsonPath);
    if (!in) {
        std::cerr << "Failed to open atlas json: " << jsonPath << "\n";
        return false;
    }

    nlohmann::json j;
    in >> j;

    if (!j.contains("frames") || !j["frames"].is_object()) {
        std::cerr << "Atlas JSON missing 'frames' object: " << jsonPath << "\n";
        return false;
    }

    for (auto it = j["frames"].begin(); it != j["frames"].end(); ++it) {
        const std::string id = it.key();
        const auto& f = it.value();
        if (!f.contains("x") || !f.contains("y") || !f.contains("w") || !f.contains("h")) {
            continue;
        }

        const int x = f["x"].get<int>();
        const int y = f["y"].get<int>();
        const int w = f["w"].get<int>();
        const int h = f["h"].get<int>();
        mFrames.emplace(id, sf::IntRect(x, y, w, h));
    }

    if (mFrames.empty()) {
        std::cerr << "Atlas JSON contained no frames: " << jsonPath << "\n";
        return false;
    }

    return true;
}

bool SpriteAtlas::has(const std::string& id) const {
    return mFrames.find(id) != mFrames.end();
}

sf::Sprite SpriteAtlas::makeSprite(const std::string& id) const {
    sf::Sprite s;
    s.setTexture(mTexture);

    const auto it = mFrames.find(id);
    if (it != mFrames.end()) {
        s.setTextureRect(it->second);
        s.setOrigin(it->second.width * 0.5f, it->second.height * 0.5f);
    }

    return s;
}
