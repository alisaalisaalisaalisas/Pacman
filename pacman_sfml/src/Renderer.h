#pragma once

#include "BitmapFont.h"
#include "SpriteAtlas.h"
#include "Types.h"

#include <SFML/Graphics/Font.hpp>
#include <SFML/Graphics/RenderTexture.hpp>
#include <SFML/Graphics/RenderWindow.hpp>
#include <SFML/Graphics/Sprite.hpp>
#include <SFML/Graphics/Texture.hpp>
#include <SFML/Graphics/Image.hpp>

#include <string>

class Map;
class Player;
class Ghost;
class Menu;

class Renderer {
public:
    explicit Renderer(sf::RenderWindow& window);

    bool loadFont(const std::string& path);
    bool loadAtlas(const std::string& imagePath, const std::string& jsonPath);
    bool loadBackground(const std::string& path);
    bool loadMapSprites(const std::string& tilePath, const std::string& smallDotPath, const std::string& bigDotPath);
    bool loadHeartTexture(const std::string& path);

    // Native pixel-art frame (rendered to a RenderTexture, then integer-scaled).
    int nativeWidth() const { return mNativeWidth; }
    int nativeHeight() const { return mNativeHeight; }

    void handleResize();

    void beginFrame();
    void endFrame();

    void drawMap(const Map& map, float tileSize);
    void drawPlayer(const Player& player, float tileSize);
    void drawGhost(const Ghost& ghost, float tileSize);
    void drawFruit(TileCoord tile, float tileSize);

    void drawHUD(int score, int lives, int level);
    void drawOverlayText(const std::string& title, const std::string& subtitle);
    void drawMenu(const Menu& menu);

    // Coordinate transformation from window to native resolution
    sf::Vector2f windowToNative(sf::Vector2i windowPos, sf::Vector2u windowSize) const;

private:
    sf::RenderWindow& mWindow;

    sf::RenderTexture mNative;
    sf::Sprite mPresent;
    int mNativeWidth = 256;
    int mNativeHeight = 288;
    int mHudHeight = 32;

    bool mHasFont = false;
    sf::Font mFont;
    BitmapFont mBitmapFont;

    bool mHasAtlas = false;
    SpriteAtlas mAtlas;

    bool mHasBackground = false;
    sf::Texture mBackgroundTexture;
    sf::Sprite mBackgroundSprite;

    bool mHasTileTexture = false;
    bool mHasMiniCoinTexture = false;
    bool mHasBigCoinTexture = false;
    bool mHasHeartTexture = false;
    sf::Texture mTileTexture;
    sf::Texture mMiniCoinTexture;
    sf::Texture mBigCoinTexture;
    sf::Texture mHeartTexture;

    sf::Vector2f playfieldOffset(const Map& map, float tileSize) const;
    void presentToWindow();

    // Cached after drawMap() for entity rendering.
    sf::Vector2f mCachedPlayfieldOffset{0.f, 0.f};
    float mCachedTileSize = 8.f;
};
