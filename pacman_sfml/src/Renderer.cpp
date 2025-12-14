#include "Renderer.h"

#include "Ghost.h"
#include "Map.h"
#include "Menu.h"
#include "Player.h"

#include <SFML/Graphics/CircleShape.hpp>
#include <SFML/Graphics/ConvexShape.hpp>
#include <SFML/Graphics/RectangleShape.hpp>
#include <SFML/Graphics/Text.hpp>
#include <SFML/Graphics/Image.hpp>

#include <algorithm>
#include <cmath>
#include <iostream>

Renderer::Renderer(sf::RenderWindow& window) : mWindow(window) {
    if (!mNative.create(static_cast<unsigned>(mNativeWidth), static_cast<unsigned>(mNativeHeight))) {
        std::cerr << "Failed to create native render target\n";
    }
}

bool Renderer::loadFont(const std::string& path) {
    if (!mFont.loadFromFile(path)) {
        std::cerr << "Failed to load font: " << path << "\n";
        mHasFont = false;
        return false;
    }
    mHasFont = true;
    return true;
}

bool Renderer::loadAtlas(const std::string& imagePath, const std::string& jsonPath) {
    mHasAtlas = mAtlas.loadFromFiles(imagePath, jsonPath);
    return mHasAtlas;
}

bool Renderer::loadBackground(const std::string& path) {
    if (!mBackgroundTexture.loadFromFile(path)) {
        std::cerr << "Failed to load background: " << path << "\n";
        mHasBackground = false;
        return false;
    }

    mBackgroundTexture.setSmooth(true);
    mBackgroundSprite.setTexture(mBackgroundTexture);
    mHasBackground = true;
    return true;
}

bool Renderer::loadMapSprites(const std::string& tilePath, const std::string& smallDotPath, const std::string& bigDotPath) {
    mHasTileTexture = mTileTexture.loadFromFile(tilePath);
    mHasMiniCoinTexture = mMiniCoinTexture.loadFromFile(smallDotPath);
    mHasBigCoinTexture = mBigCoinTexture.loadFromFile(bigDotPath);

    if (!mHasTileTexture) {
        std::cerr << "Failed to load tile texture: " << tilePath << "\n";
    }
    if (!mHasMiniCoinTexture) {
        std::cerr << "Failed to load small dot texture: " << smallDotPath << "\n";
    }
    if (!mHasBigCoinTexture) {
        std::cerr << "Failed to load big dot texture: " << bigDotPath << "\n";
    }

    mTileTexture.setSmooth(false);
    mMiniCoinTexture.setSmooth(false);
    mBigCoinTexture.setSmooth(false);

    return mHasTileTexture && mHasMiniCoinTexture && mHasBigCoinTexture;
}

bool Renderer::loadHeartTexture(const std::string& path) {
    sf::Image img;
    if (!img.loadFromFile(path)) {
        std::cerr << "Failed to load heart texture: " << path << "\n";
        mHasHeartTexture = false;
        return false;
    }
    img.createMaskFromColor(sf::Color::Black);

    if (!mHeartTexture.loadFromImage(img)) {
        std::cerr << "Failed to create heart texture from image: " << path << "\n";
        mHasHeartTexture = false;
        return false;
    }

    mHeartTexture.setSmooth(false);
    mHasHeartTexture = true;
    return true;
}

void Renderer::handleResize() {
    // Presentation scaling is computed every frame.
}

sf::Vector2f Renderer::playfieldOffset(const Map& map, float tileSize) const {
    const float playW = static_cast<float>(map.width()) * tileSize;
    const float x = std::floor((static_cast<float>(mNativeWidth) - playW) * 0.5f);
    const float y = static_cast<float>(mHudHeight);
    return {x, y};
}

void Renderer::beginFrame() {
    mNative.clear(sf::Color::Black);

    // HUD strip background
    sf::RectangleShape hud({static_cast<float>(mNativeWidth), static_cast<float>(mHudHeight)});
    hud.setPosition(0.f, 0.f);
    hud.setFillColor(sf::Color(10, 10, 10));
    mNative.draw(hud);
}

void Renderer::endFrame() {
    presentToWindow();
}

void Renderer::presentToWindow() {
    mNative.display();

    const sf::Vector2u win = mWindow.getSize();
    const float nativeW = static_cast<float>(mNativeWidth);
    const float nativeH = static_cast<float>(mNativeHeight);

    // Calculate scale to fit the window while preserving aspect ratio
    const float scaleX = static_cast<float>(win.x) / nativeW;
    const float scaleY = static_cast<float>(win.y) / nativeH;
    const float scale = std::min(scaleX, scaleY);

    const float dstW = nativeW * scale;
    const float dstH = nativeH * scale;

    const float offsetX = std::floor((static_cast<float>(win.x) - dstW) * 0.5f);
    const float offsetY = std::floor((static_cast<float>(win.y) - dstH) * 0.5f);

    mPresent.setTexture(mNative.getTexture(), true);
    mPresent.setScale(scale, scale);
    mPresent.setPosition(offsetX, offsetY);

    mWindow.setView(mWindow.getDefaultView());
    mWindow.clear(sf::Color::Black);
    mWindow.draw(mPresent);
    mWindow.display();
}

void Renderer::drawMap(const Map& map, float tileSize) {
    const sf::Vector2f off = playfieldOffset(map, tileSize);
    mCachedPlayfieldOffset = off;
    mCachedTileSize = tileSize;

    if (mHasBackground) {
        const auto texSize = mBackgroundTexture.getSize();
        if (texSize.x > 0 && texSize.y > 0) {
            const float scaleX = static_cast<float>(mNativeWidth) / static_cast<float>(texSize.x);
            const float scaleY = static_cast<float>(mNativeHeight - mHudHeight) / static_cast<float>(texSize.y);
            mBackgroundSprite.setScale(scaleX, scaleY);
            mBackgroundSprite.setPosition(0.f, static_cast<float>(mHudHeight));
            mNative.draw(mBackgroundSprite);
        }
    }

    sf::Sprite wallSprite;
    sf::Sprite dotSprite;
    sf::Sprite pelletSprite;

    // Ensure no leftover background colors on tiles/coins
    if (mHasTileTexture) {
        wallSprite.setTexture(mTileTexture);
        const auto sz = mTileTexture.getSize();
        if (sz.x > 0 && sz.y > 0) {
            const float scaleX = tileSize / static_cast<float>(sz.x);
            const float scaleY = tileSize / static_cast<float>(sz.y);
            wallSprite.setScale(scaleX, scaleY);
        }
    }

    if (mHasMiniCoinTexture) {
        dotSprite.setTexture(mMiniCoinTexture);
        const auto sz = mMiniCoinTexture.getSize();
        if (sz.x > 0 && sz.y > 0) {
            const float scale = (tileSize * 0.5f) / static_cast<float>(sz.x);
            dotSprite.setScale(scale, scale);
            dotSprite.setOrigin(static_cast<float>(sz.x) * 0.5f, static_cast<float>(sz.y) * 0.5f);
        }
    }

    if (mHasBigCoinTexture) {
        pelletSprite.setTexture(mBigCoinTexture);
        const auto sz = mBigCoinTexture.getSize();
        if (sz.x > 0 && sz.y > 0) {
            const float scale = (tileSize * 0.7f) / static_cast<float>(sz.x);
            pelletSprite.setScale(scale, scale);
            pelletSprite.setOrigin(static_cast<float>(sz.x) * 0.5f, static_cast<float>(sz.y) * 0.5f);
        }
    }

    sf::RectangleShape wall({tileSize, tileSize});
    wall.setFillColor(sf::Color(20, 20, 200));

    // Pixel-ish dots
    sf::RectangleShape dot({2.f, 2.f});
    dot.setOrigin(1.f, 1.f);
    dot.setFillColor(sf::Color(255, 220, 50));

    sf::RectangleShape pellet({4.f, 4.f});
    pellet.setOrigin(2.f, 2.f);
    pellet.setFillColor(sf::Color(255, 220, 50));

    for (int y = 0; y < map.height(); ++y) {
        for (int x = 0; x < map.width(); ++x) {
            const char c = map.cell(x, y);
            const float px = off.x + static_cast<float>(x) * tileSize;
            const float py = off.y + static_cast<float>(y) * tileSize;

            if (c == '#') {
                if (mHasTileTexture) {
                    wallSprite.setPosition(px, py);
                    mNative.draw(wallSprite);
                } else {
                    wall.setPosition(px, py);
                    mNative.draw(wall);
                }
            } else if (c == '.') {
                if (mHasMiniCoinTexture) {
                    dotSprite.setPosition(px + tileSize * 0.5f, py + tileSize * 0.5f);
                    mNative.draw(dotSprite);
                } else {
                    dot.setPosition(px + tileSize * 0.5f, py + tileSize * 0.5f);
                    mNative.draw(dot);
                }
            } else if (c == 'o') {
                if (mHasBigCoinTexture) {
                    pelletSprite.setPosition(px + tileSize * 0.5f, py + tileSize * 0.5f);
                    mNative.draw(pelletSprite);
                } else {
                    pellet.setPosition(px + tileSize * 0.5f, py + tileSize * 0.5f);
                    mNative.draw(pellet);
                }
            }
        }
    }
}

void Renderer::drawPlayer(const Player& player, float tileSize) {
    const sf::Vector2f off = mCachedPlayfieldOffset;
    const sf::Vector2f p{player.position().x + off.x, player.position().y + off.y};

    float rotation = 0.f;
    switch (player.direction()) {
    case Direction::Right: rotation = 0.f; break;
    case Direction::Left: rotation = 180.f; break;
    case Direction::Up: rotation = 270.f; break;
    case Direction::Down: rotation = 90.f; break;
    default: rotation = 0.f; break;
    }

    if (mHasAtlas) {
        const char* frame = (player.mouthOpen01() > 0.5f) ? "pacman_open" : "pacman_closed";
        sf::Sprite s = mAtlas.makeSprite(frame);
        const float scale = tileSize / 8.f;
        s.setScale(scale, scale);
        s.setRotation(rotation);
        s.setPosition(p);
        mNative.draw(s);
        return;
    }

    const float r = player.radius(tileSize);

    sf::CircleShape body(r, 32);
    body.setOrigin(r, r);
    body.setPosition(p);
    body.setFillColor(sf::Color(255, 230, 40));
    mNative.draw(body);

    // Mouth: a black triangle over the circle.
    const float open01 = player.mouthOpen01();
    const float mouthAngle = (18.f + 55.f * open01) * 3.1415926f / 180.f;

    sf::ConvexShape mouth;
    mouth.setPointCount(3);
    mouth.setPoint(0, {0.f, 0.f});
    mouth.setPoint(1, {r * 1.25f, -std::tan(mouthAngle) * (r * 0.9f)});
    mouth.setPoint(2, {r * 1.25f, std::tan(mouthAngle) * (r * 0.9f)});
    mouth.setFillColor(sf::Color::Black);
    mouth.setPosition(p);
    mouth.setRotation(rotation);
    mNative.draw(mouth);
}

void Renderer::drawGhost(const Ghost& ghost, float tileSize) {
    const sf::Vector2f off = mCachedPlayfieldOffset;
    const sf::Vector2f p{ghost.position().x + off.x, ghost.position().y + off.y};

    if (mHasAtlas) {
        const char* frame = "ghost_red";
        if (ghost.mode() == GhostMode::Frightened) {
            frame = "ghost_fright";
        } else if (ghost.mode() == GhostMode::Eaten) {
            frame = "ghost_eaten";
        } else {
            switch (ghost.id()) {
            case GhostId::Blinky: frame = "ghost_red"; break;
            case GhostId::Pinky: frame = "ghost_pink"; break;
            case GhostId::Inky: frame = "ghost_cyan"; break;
            case GhostId::Clyde: frame = "ghost_orange"; break;
            }
        }

        sf::Sprite s = mAtlas.makeSprite(frame);
        const float scale = tileSize / 8.f;
        s.setScale(scale, scale);
        s.setPosition(p);
        mNative.draw(s);
        return;
    }

    const float r = tileSize * 0.42f;

    sf::CircleShape head(r, 18);
    head.setOrigin(r, r);
    head.setPosition(p);
    head.setFillColor(ghost.color());

    sf::RectangleShape body({r * 2.f, r});
    body.setOrigin(r, 0.f);
    body.setPosition(p.x, p.y);
    body.setFillColor(ghost.color());

    mNative.draw(body);
    mNative.draw(head);

    // Eyes (simple placeholder)
    sf::CircleShape eyeWhite(r * 0.22f, 12);
    eyeWhite.setFillColor(sf::Color::White);
    eyeWhite.setOrigin(eyeWhite.getRadius(), eyeWhite.getRadius());

    sf::CircleShape pupil(r * 0.10f, 10);
    pupil.setFillColor(sf::Color::Blue);
    pupil.setOrigin(pupil.getRadius(), pupil.getRadius());

    eyeWhite.setPosition(p.x - r * 0.25f, p.y - r * 0.12f);
    mNative.draw(eyeWhite);
    eyeWhite.setPosition(p.x + r * 0.25f, p.y - r * 0.12f);
    mNative.draw(eyeWhite);

    pupil.setPosition(p.x - r * 0.25f, p.y - r * 0.12f);
    mNative.draw(pupil);
    pupil.setPosition(p.x + r * 0.25f, p.y - r * 0.12f);
    mNative.draw(pupil);
}

void Renderer::drawFruit(TileCoord tile, float tileSize) {
    const sf::Vector2f off = mCachedPlayfieldOffset;

    sf::RectangleShape fruit({tileSize * 0.75f, tileSize * 0.75f});
    fruit.setOrigin(fruit.getSize().x * 0.5f, fruit.getSize().y * 0.5f);
    fruit.setFillColor(sf::Color(255, 60, 200));

    const float x = off.x + (static_cast<float>(tile.x) + 0.5f) * tileSize;
    const float y = off.y + (static_cast<float>(tile.y) + 0.5f) * tileSize;
    fruit.setPosition(x, y);
    mNative.draw(fruit);
}

void Renderer::drawHUD(int score, int lives, int level) {
    const std::string sScore = "SCORE " + std::to_string(score);
    const std::string sLives = "LIVES " + std::to_string(lives);
    const std::string sLevel = "LVL " + std::to_string(level);

    if (mHasFont) {
        sf::Text t;
        t.setFont(mFont);
        t.setCharacterSize(10);
        t.setLetterSpacing(1.2f);
        t.setFillColor(sf::Color(240, 240, 240));

        t.setString(sScore);
        t.setPosition(4.f, 6.f);
        mNative.draw(t);

        // Draw hearts for lives
        if (mHasHeartTexture) {
            sf::Sprite heart;
            heart.setTexture(mHeartTexture);
            const auto sz = mHeartTexture.getSize();
            if (sz.x > 0) {
                const float target = 12.f;
                const float scale = target / static_cast<float>(sz.x);
                heart.setScale(scale, scale);
                const float spacing = 4.f;
                const float totalW = (static_cast<float>(lives) * target) + (static_cast<float>(lives - 1) * spacing);
                float startX = std::floor((static_cast<float>(mNativeWidth) - totalW) * 0.5f);
                const float y = 4.f;
                for (int i = 0; i < lives; ++i) {
                    heart.setPosition(startX + static_cast<float>(i) * (target + spacing), y);
                    mNative.draw(heart);
                }
            }
        } else {
            t.setString(sLives);
            const auto mid = t.getLocalBounds();
            t.setPosition(std::floor((static_cast<float>(mNativeWidth) - mid.width) * 0.5f), 6.f);
            mNative.draw(t);
        }

        t.setString(sLevel);
        const auto rb = t.getLocalBounds();
        t.setPosition(static_cast<float>(mNativeWidth) - rb.width - 4.f, 6.f);
        mNative.draw(t);
        return;
    }

    // BitmapFont fallback - use scale 1 to fit in 256px width
    const int scale = 1;
    const sf::Color hudColor(240, 240, 240);
    
    // Draw SCORE on left
    mBitmapFont.draw(mNative, sScore, {4.f, 8.f}, scale, hudColor);

    // Draw LIVES in center
    const sf::Vector2f m = mBitmapFont.measure(sLives, scale);
    mBitmapFont.draw(mNative, sLives, {std::floor((static_cast<float>(mNativeWidth) - m.x) * 0.5f), 8.f}, scale, hudColor);

    // Draw LVL on right
    const sf::Vector2f r = mBitmapFont.measure(sLevel, scale);
    mBitmapFont.draw(mNative, sLevel, {static_cast<float>(mNativeWidth) - r.x - 4.f, 8.f}, scale, hudColor);
}

void Renderer::drawOverlayText(const std::string& title, const std::string& subtitle) {
    sf::RectangleShape dim({static_cast<float>(mNativeWidth), static_cast<float>(mNativeHeight)});
    dim.setFillColor(sf::Color(0, 0, 0, 210));
    dim.setPosition(0.f, 0.f);
    mNative.draw(dim);

    if (mHasFont) {
        sf::Text t;
        t.setFont(mFont);
        t.setFillColor(sf::Color::White);
        t.setLetterSpacing(1.0f);

        t.setCharacterSize(18);
        t.setString(title);
        auto tb = t.getLocalBounds();
        t.setOrigin(tb.left + tb.width * 0.5f, tb.top + tb.height * 0.5f);
        t.setPosition(static_cast<float>(mNativeWidth) * 0.5f, static_cast<float>(mNativeHeight) * 0.40f);
        mNative.draw(t);

        t.setCharacterSize(8);
        t.setString(subtitle);
        auto sb = t.getLocalBounds();
        t.setOrigin(sb.left + sb.width * 0.5f, sb.top + sb.height * 0.5f);
        t.setPosition(static_cast<float>(mNativeWidth) * 0.5f, static_cast<float>(mNativeHeight) * 0.53f);
        mNative.draw(t);
        return;
    }

    const int titleScale = 3;
    const sf::Vector2f tsize = mBitmapFont.measure(title, titleScale);
    mBitmapFont.draw(mNative, title, {std::floor((static_cast<float>(mNativeWidth) - tsize.x) * 0.5f), 110.f}, titleScale, sf::Color::White);

    const int subScale = 2;
    const sf::Vector2f ssize = mBitmapFont.measure(subtitle, subScale);
    mBitmapFont.draw(mNative, subtitle, {std::floor((static_cast<float>(mNativeWidth) - ssize.x) * 0.5f), 150.f}, subScale, sf::Color(220, 220, 220));
}

void Renderer::drawMenu(const Menu& menu) {
    // Inspired by provided menu mockup: tinted panel with neon accent and centered items
    sf::RectangleShape dim({static_cast<float>(mNativeWidth), static_cast<float>(mNativeHeight)});
    dim.setFillColor(sf::Color(12, 30, 55, 235));
    dim.setPosition(0.f, 0.f);
    mNative.draw(dim);

    sf::RectangleShape frame({static_cast<float>(mNativeWidth) - 14.f, static_cast<float>(mNativeHeight) - 14.f});
    frame.setPosition(7.f, 7.f);
    frame.setFillColor(sf::Color(0, 0, 0, 0));
    frame.setOutlineColor(sf::Color(50, 184, 198));
    frame.setOutlineThickness(2.f);
    mNative.draw(frame);

    const sf::Color normal(160, 200, 220);
    const sf::Color selected(255, 196, 80);

    if (mHasFont) {
        sf::Text t;
        t.setFont(mFont);
        t.setLetterSpacing(1.05f);

        t.setCharacterSize(24);
        t.setFillColor(sf::Color::White);
        t.setString(menu.title());
        auto b = t.getLocalBounds();
        t.setOrigin(b.left + b.width * 0.5f, b.top + b.height * 0.5f);
        t.setPosition(static_cast<float>(mNativeWidth) * 0.5f, 60.f);
        mNative.draw(t);

        float y = 120.f;
        for (std::size_t i = 0; i < menu.items().size(); ++i) {
            t.setCharacterSize(16);
            const bool isSel = (i == menu.selectedIndex());
            t.setFillColor(isSel ? selected : normal);
            t.setString(menu.items()[i]);
            auto ib = t.getLocalBounds();
            t.setOrigin(ib.left + ib.width * 0.5f, ib.top + ib.height * 0.5f);
            t.setPosition(static_cast<float>(mNativeWidth) * 0.5f, y);
            mNative.draw(t);

            // no selection braces or arrows for a cleaner look

            y += 30.f;
        }
        return;
    }

    const int titleScale = 3;
    const sf::Vector2f titleSize = mBitmapFont.measure(menu.title(), titleScale);
    mBitmapFont.draw(mNative, menu.title(), {std::floor((static_cast<float>(mNativeWidth) - titleSize.x) * 0.5f), 60.f}, titleScale, sf::Color::White);

    const int itemScale = 2;
    float y = 120.f;
    for (std::size_t i = 0; i < menu.items().size(); ++i) {
        const bool isSel = (i == menu.selectedIndex());
        const std::string label = isSel ? "> " + menu.items()[i] : "  " + menu.items()[i];
        const sf::Vector2f itemSize = mBitmapFont.measure(label, itemScale);
        mBitmapFont.draw(mNative, label, {std::floor((static_cast<float>(mNativeWidth) - itemSize.x) * 0.5f), y}, itemScale, isSel ? selected : normal);
        y += 28.f;
    }
}

sf::Vector2f Renderer::windowToNative(sf::Vector2i windowPos, sf::Vector2u windowSize) const {
    const float nativeW = static_cast<float>(mNativeWidth);
    const float nativeH = static_cast<float>(mNativeHeight);

    // Calculate scale (same logic as presentToWindow)
    const float scaleX = static_cast<float>(windowSize.x) / nativeW;
    const float scaleY = static_cast<float>(windowSize.y) / nativeH;
    const float scale = std::min(scaleX, scaleY);

    const float dstW = nativeW * scale;
    const float dstH = nativeH * scale;

    const float offsetX = (static_cast<float>(windowSize.x) - dstW) * 0.5f;
    const float offsetY = (static_cast<float>(windowSize.y) - dstH) * 0.5f;

    // Transform window coordinates to native coordinates
    const float nativeX = (static_cast<float>(windowPos.x) - offsetX) / scale;
    const float nativeY = (static_cast<float>(windowPos.y) - offsetY) / scale;

    return {nativeX, nativeY};
}
