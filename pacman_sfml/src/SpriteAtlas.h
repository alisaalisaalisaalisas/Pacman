#pragma once

#include <SFML/Graphics/Rect.hpp>
#include <SFML/Graphics/Sprite.hpp>
#include <SFML/Graphics/Texture.hpp>

#include <string>
#include <unordered_map>

class SpriteAtlas {
public:
    bool loadFromFiles(const std::string& imagePath, const std::string& jsonPath);

    bool has(const std::string& id) const;
    sf::Sprite makeSprite(const std::string& id) const;

private:
    sf::Texture mTexture;
    std::unordered_map<std::string, sf::IntRect> mFrames;
};
