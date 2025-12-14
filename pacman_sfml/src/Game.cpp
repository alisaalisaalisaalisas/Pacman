#include "Game.h"

#include "Direction.h"

#include <SFML/Window/Event.hpp>
#include <SFML/Window/Joystick.hpp>

#include <cmath>
#include <filesystem>
#include <fstream>
#include <iostream>

namespace {
// Simple relative path resolution - just return the path as-is
// Assets are expected to be in the current directory's 'assets' subfolder
std::string tryResolveAsset(const std::string& relative) {
    // Just return the relative path - assets are copied to the build directory
    return relative;
}

float distSq(sf::Vector2f a, sf::Vector2f b) {
    const float dx = a.x - b.x;
    const float dy = a.y - b.y;
    return dx * dx + dy * dy;
}

TileCoord clampToMap(TileCoord t, const Map& map) {
    if (t.x < 0) t.x = 0;
    if (t.y < 0) t.y = 0;
    if (t.x >= map.width()) t.x = map.width() - 1;
    if (t.y >= map.height()) t.y = map.height() - 1;
    return t;
}
}

Game::Game()
    : mWindow(sf::VideoMode(960, 720), "Pac-Man (SFML)", sf::Style::Default),
      mRenderer(mWindow) {
    std::cerr << "[Game] Window and Renderer created" << std::endl;
    
    mWindow.setVerticalSyncEnabled(true);
    std::cerr << "[Game] VSync enabled" << std::endl;

    std::random_device rd;
    mRng.seed(rd());
    std::cerr << "[Game] RNG seeded" << std::endl;

    mMainMenu.setTitle("PAC-MAN");
    mMainMenu.setItems({"Start Game", "Options", "Exit"});
    std::cerr << "[Game] Main menu configured" << std::endl;

    mPauseMenu.setTitle("Paused");
    mPauseMenu.setItems({"Resume", "Quit to Menu"});
    std::cerr << "[Game] Pause menu configured" << std::endl;

    // Load assets (optional).
    std::string fontPath = tryResolveAsset("assets/fonts/PressStart2P.ttf");
    std::cerr << "[Game] Loading font from: " << fontPath << std::endl;
    mRenderer.loadFont(fontPath);
    std::cerr << "[Game] Font loaded" << std::endl;
    
    std::string atlasPath = tryResolveAsset("assets/sprites/atlas.bmp");
    std::string atlasJsonPath = tryResolveAsset("assets/sprites/atlas.json");
    std::cerr << "[Game] Loading atlas from: " << atlasPath << " and " << atlasJsonPath << std::endl;
    mRenderer.loadAtlas(atlasPath, atlasJsonPath);
    std::cerr << "[Game] Atlas loaded" << std::endl;

    std::string backgroundPath = tryResolveAsset("assets/fonts/BackG.jpg");
    std::cerr << "[Game] Loading background from: " << backgroundPath << std::endl;
    mRenderer.loadBackground(backgroundPath);

    std::cerr << "[Game] Loading map sprites..." << std::endl;
    mRenderer.loadMapSprites(tryResolveAsset("assets/sprites/tile.png"),
                              tryResolveAsset("assets/sprites/minicoin.png"),
                              tryResolveAsset("assets/sprites/bigcoin.png"));
    mRenderer.loadHeartTexture(tryResolveAsset("assets/sprites/heart.png"));

    mAudio.setMasterVolume(0.8f);
    std::cerr << "[Game] Audio volume set" << std::endl;
    
    std::cerr << "[Game] Loading sounds..." << std::endl;
    // Use the downloaded sound files
    mAudio.loadSound("waka", tryResolveAsset("assets/sounds/eat.mp3"));
    mAudio.loadSound("power", tryResolveAsset("assets/sounds/eat.mp3"));  // reuse eat sound
    mAudio.loadSound("eat_ghost", tryResolveAsset("assets/sounds/eat.mp3"));  // reuse eat sound
    mAudio.loadSound("death", tryResolveAsset("assets/sounds/death.wav"));
    mAudio.loadSound("gameover", tryResolveAsset("assets/sounds/Pacman-death-sound.mp3"));
    std::cerr << "[Game] Sounds loaded" << std::endl;

    std::string musicPath = tryResolveAsset("assets/sounds/music.wav");
    std::cerr << "[Game] Loading music from: " << musicPath << std::endl;
    mAudio.playMusic(musicPath, true);
    std::cerr << "[Game] Music started" << std::endl;

    // Preload a default level so the menu can render the maze as a background.
    std::cerr << "[Game] Loading level 1..." << std::endl;
    loadLevel(1);
    std::cerr << "[Game] Level loaded" << std::endl;
    
    setState(State::MainMenu);
    std::cerr << "[Game] Constructor complete!" << std::endl;
}

int Game::run() {
    sf::Clock clock;

    constexpr float fixedDt = 1.f / 60.f;
    float accumulator = 0.f;

    while (mWindow.isOpen()) {
        processEvents();

        float dt = clock.restart().asSeconds();
        if (dt > 0.25f) dt = 0.25f;
        accumulator += dt;

        while (accumulator >= fixedDt) {
            update(fixedDt);
            accumulator -= fixedDt;
        }

        render();
    }

    return 0;
}

void Game::setState(State s) {
    std::cerr << "[Game] State changed to: " << static_cast<int>(s) << std::endl;
    mState = s;
}

void Game::startNewGame() {
    std::cerr << "[Game] Starting new game..." << std::endl;
    mScore = 0;
    mLives = 3;
    mLevel = 1;
    loadLevel(mLevel);
    resetEntities();
    setState(State::Playing);
    std::cerr << "[Game] Map size: " << mMap.width() << "x" << mMap.height() << std::endl;
    std::cerr << "[Game] Player spawn at: " << mPlayer.position().x << ", " << mPlayer.position().y << std::endl;
}

void Game::loadLevel(int level) {
    (void)level;

    if (!mMap.loadFromFile(tryResolveAsset("assets/maps/level1.txt"))) {
        // Fallback: minimal map.
        std::cerr << "Using fallback map\n";
        mMap.loadFromFile(tryResolveAsset("assets/maps/fallback.txt"));
    }

    // Renderer uses a fixed native 256x288 pixel buffer with a top HUD strip.

    // Build ghosts once.
    if (mGhosts.empty()) {
        mGhosts.emplace_back(GhostId::Blinky, sf::Color(255, 60, 60));
        mGhosts.emplace_back(GhostId::Pinky, sf::Color(255, 140, 200));
        mGhosts.emplace_back(GhostId::Inky, sf::Color(60, 230, 230));
        mGhosts.emplace_back(GhostId::Clyde, sf::Color(255, 170, 40));
    }

    // Mode schedule (classic-ish, simplified). Scatter/Chase alternating; last chase is "forever".
    mModePhasesSeconds = {7.f, 20.f, 7.f, 20.f, 5.f, 20.f, 5.f, 9999.f};
    mModePhaseIndex = 0;
    mModeTimer = 0.f;
    mBaseMode = GhostMode::Scatter;

    mDotsEatenThisLevel = 0;
    mFruitActive = false;
    mFruitSpawnedThisLevel = false;
    mFruitTimer = 0.f;
    mFruitTile = mMap.fruitSpawn();

    resetEntities();
}

void Game::resetEntities() {
    mPlayer.reset(mMap.playerSpawn(), mMap, mTileSize);

    const TileCoord topLeft{1, 1};
    const TileCoord topRight{mMap.width() - 2, 1};
    const TileCoord bottomLeft{1, mMap.height() - 2};
    const TileCoord bottomRight{mMap.width() - 2, mMap.height() - 2};

    for (auto& g : mGhosts) {
        TileCoord spawn = mMap.ghostSpawnBlinky();
        TileCoord corner = topRight;

        switch (g.id()) {
        case GhostId::Blinky: spawn = mMap.ghostSpawnBlinky(); corner = topRight; break;
        case GhostId::Pinky: spawn = mMap.ghostSpawnPinky(); corner = topLeft; break;
        case GhostId::Inky: spawn = mMap.ghostSpawnInky(); corner = bottomRight; break;
        case GhostId::Clyde: spawn = mMap.ghostSpawnClyde(); corner = bottomLeft; break;
        }

        g.reset(spawn, corner, mMap, mTileSize);
        g.setMode(mBaseMode);
    }

    mFrightenedTimer = 0.f;
    mFreezeTimer = 0.f;
}

void Game::processEvents() {
    sf::Event e;
    while (mWindow.pollEvent(e)) {
        if (e.type == sf::Event::Closed) {
            mWindow.close();
            return;
        }
        if (e.type == sf::Event::Resized) {
            mRenderer.handleResize();
        }
        
        // Global F11 fullscreen toggle
        if (e.type == sf::Event::KeyPressed && e.key.code == sf::Keyboard::F11) {
            toggleFullscreen();
            continue;
        }

        if (mState == State::MainMenu) {
            // Handle mouse click for menu selection
            if (e.type == sf::Event::MouseButtonPressed && e.mouseButton.button == sf::Mouse::Left) {
                // Transform mouse position to native coordinates
                sf::Vector2i mousePos(e.mouseButton.x, e.mouseButton.y);
                sf::Vector2f nativePos = mRenderer.windowToNative(mousePos, mWindow.getSize());
                
                // Check which menu item was clicked (approximate hit boxes)
                // Menu items start at y=120 with 26px spacing, centered horizontally
                if (nativePos.x >= 40.f && nativePos.x <= 216.f) { // 256 - 40*2 = 176 width
                    for (std::size_t i = 0; i < mMainMenu.items().size(); ++i) {
                        float itemY = 120.f + static_cast<float>(i) * 26.f;
                        if (nativePos.y >= itemY - 10.f && nativePos.y <= itemY + 16.f) {
                            const auto idx = i;
                            if (idx == 0) {
                                startNewGame();
                            } else if (idx == 1) {
                                setState(State::Options);
                            } else if (idx == 2) {
                                mWindow.close();
                            }
                            break;
                        }
                    }
                }
            }
            // Handle mouse hover for menu item highlighting
            if (e.type == sf::Event::MouseMoved) {
                sf::Vector2i mousePos(e.mouseMove.x, e.mouseMove.y);
                sf::Vector2f nativePos = mRenderer.windowToNative(mousePos, mWindow.getSize());
                
                if (nativePos.x >= 40.f && nativePos.x <= 216.f) {
                    for (std::size_t i = 0; i < mMainMenu.items().size(); ++i) {
                        float itemY = 120.f + static_cast<float>(i) * 26.f;
                        if (nativePos.y >= itemY - 10.f && nativePos.y <= itemY + 16.f) {
                            mMainMenu.setSelected(i);
                            break;
                        }
                    }
                }
            }
            
            if (mMainMenu.handleEvent(e)) {
                const auto idx = mMainMenu.selectedIndex();
                if (idx == 0) {
                    startNewGame();
                } else if (idx == 1) {
                    setState(State::Options);
                } else if (idx == 2) {
                    mWindow.close();
                }
            }
        } else if (mState == State::Playing) {
            if (e.type == sf::Event::KeyPressed) {
                switch (e.key.code) {
                case sf::Keyboard::Escape: setState(State::Paused); break;
                case sf::Keyboard::Up: case sf::Keyboard::W: mPlayer.requestDirection(Direction::Up); break;
                case sf::Keyboard::Down: case sf::Keyboard::S: mPlayer.requestDirection(Direction::Down); break;
                case sf::Keyboard::Left: case sf::Keyboard::A: mPlayer.requestDirection(Direction::Left); break;
                case sf::Keyboard::Right: case sf::Keyboard::D: mPlayer.requestDirection(Direction::Right); break;
                default: break;
                }
            }
        } else if (mState == State::Paused) {
            if (e.type == sf::Event::KeyPressed && e.key.code == sf::Keyboard::Escape) {
                setState(State::Playing);
            }
            // Handle mouse click for pause menu
            if (e.type == sf::Event::MouseButtonPressed && e.mouseButton.button == sf::Mouse::Left) {
                sf::Vector2i mousePos(e.mouseButton.x, e.mouseButton.y);
                sf::Vector2f nativePos = mRenderer.windowToNative(mousePos, mWindow.getSize());
                
                if (nativePos.x >= 40.f && nativePos.x <= 216.f) {
                    for (std::size_t i = 0; i < mPauseMenu.items().size(); ++i) {
                        float itemY = 120.f + static_cast<float>(i) * 26.f;
                        if (nativePos.y >= itemY - 10.f && nativePos.y <= itemY + 16.f) {
                            if (i == 0) {
                                setState(State::Playing);
                            } else {
                                setState(State::MainMenu);
                            }
                            break;
                        }
                    }
                }
            }
            // Handle mouse hover for pause menu
            if (e.type == sf::Event::MouseMoved) {
                sf::Vector2i mousePos(e.mouseMove.x, e.mouseMove.y);
                sf::Vector2f nativePos = mRenderer.windowToNative(mousePos, mWindow.getSize());
                
                if (nativePos.x >= 40.f && nativePos.x <= 216.f) {
                    for (std::size_t i = 0; i < mPauseMenu.items().size(); ++i) {
                        float itemY = 120.f + static_cast<float>(i) * 26.f;
                        if (nativePos.y >= itemY - 10.f && nativePos.y <= itemY + 16.f) {
                            mPauseMenu.setSelected(i);
                            break;
                        }
                    }
                }
            }
            
            if (mPauseMenu.handleEvent(e)) {
                const auto idx = mPauseMenu.selectedIndex();
                if (idx == 0) {
                    setState(State::Playing);
                } else {
                    setState(State::MainMenu);
                }
            }
        } else if (mState == State::GameOver) {
            if (e.type == sf::Event::KeyPressed) {
                if (e.key.code == sf::Keyboard::Enter || e.key.code == sf::Keyboard::Space) {
                    setState(State::MainMenu);
                }
            }
            // Also allow mouse click to return to menu
            if (e.type == sf::Event::MouseButtonPressed && e.mouseButton.button == sf::Mouse::Left) {
                setState(State::MainMenu);
            }
        } else if (mState == State::Options) {
            if (e.type == sf::Event::KeyPressed) {
                if (e.key.code == sf::Keyboard::Escape) {
                    setState(State::MainMenu);
                }
                if (e.key.code == sf::Keyboard::Up) {
                    mAudio.setMasterVolume(1.0f);
                }
                if (e.key.code == sf::Keyboard::Down) {
                    mAudio.setMasterVolume(0.4f);
                }
            }
        }
    }
}

void Game::pollControllerInput() {
    if (!sf::Joystick::isConnected(0)) {
        return;
    }

    // D-pad as POV is not standardized; use axes for a basic default.
    const float x = sf::Joystick::getAxisPosition(0, sf::Joystick::X);
    const float y = sf::Joystick::getAxisPosition(0, sf::Joystick::Y);

    const float dead = 40.f;
    if (std::abs(x) > std::abs(y)) {
        if (x > dead) mPlayer.requestDirection(Direction::Right);
        if (x < -dead) mPlayer.requestDirection(Direction::Left);
    } else {
        if (y > dead) mPlayer.requestDirection(Direction::Down);
        if (y < -dead) mPlayer.requestDirection(Direction::Up);
    }
}

void Game::updateGhostMode(float dt) {
    if (mFrightenedTimer > 0.f) {
        mFrightenedTimer -= dt;
        if (mFrightenedTimer <= 0.f) {
            mFrightenedTimer = 0.f;
            // Restore base mode.
            setAllGhostModes(mBaseMode, false);
        }
        return; // Freeze scatter/chase schedule while frightened (classic behavior).
    }

    mModeTimer += dt;

    const float phaseLen = mModePhasesSeconds[std::min(mModePhaseIndex, mModePhasesSeconds.size() - 1)];
    if (mModeTimer >= phaseLen) {
        mModeTimer = 0.f;
        mModePhaseIndex = std::min(mModePhaseIndex + 1, mModePhasesSeconds.size() - 1);

        // Toggle Scatter <-> Chase.
        mBaseMode = (mBaseMode == GhostMode::Scatter) ? GhostMode::Chase : GhostMode::Scatter;
        setAllGhostModes(mBaseMode, true);
    }
}

void Game::setAllGhostModes(GhostMode mode, bool reverse) {
    for (auto& g : mGhosts) {
        if (reverse) {
            g.reverse();
        }
        // If the ghost is currently returning home (Eaten), leave it alone.
        if (g.mode() != GhostMode::Eaten) {
            g.setMode(mode);
        }
    }
}

TileCoord Game::chaseTargetFor(const Ghost& ghost) const {
    const TileCoord pac = mPlayer.currentTile(mMap, mTileSize);

    if (ghost.mode() == GhostMode::Scatter) {
        return ghost.scatterCorner();
    }

    if (ghost.mode() == GhostMode::Eaten) {
        return ghost.spawnTile();
    }

    // For Frightened we don't care; Ghost chooses random.
    if (ghost.mode() == GhostMode::Frightened) {
        return pac;
    }

    // Chase mode personalities.
    const sf::Vector2i dir = dirToGridDelta(mPlayer.direction());

    if (ghost.id() == GhostId::Blinky) {
        return pac;
    }

    if (ghost.id() == GhostId::Pinky) {
        TileCoord t{pac.x + 4 * dir.x, pac.y + 4 * dir.y};
        return clampToMap(t, mMap);
    }

    if (ghost.id() == GhostId::Inky) {
        const Ghost* blinky = nullptr;
        for (const auto& g : mGhosts) {
            if (g.id() == GhostId::Blinky) {
                blinky = &g;
                break;
            }
        }
        const TileCoord twoAhead = clampToMap({pac.x + 2 * dir.x, pac.y + 2 * dir.y}, mMap);
        if (!blinky) {
            return twoAhead;
        }
        const TileCoord bTile = blinky->currentTile(mMap, mTileSize);
        const TileCoord vec{twoAhead.x - bTile.x, twoAhead.y - bTile.y};
        return clampToMap({bTile.x + 2 * vec.x, bTile.y + 2 * vec.y}, mMap);
    }

    // Clyde
    const TileCoord cTile = ghost.currentTile(mMap, mTileSize);
    const int dx = cTile.x - pac.x;
    const int dy = cTile.y - pac.y;
    const int d2 = dx * dx + dy * dy;
    if (d2 > (8 * 8)) {
        return pac;
    }
    return ghost.scatterCorner();
}

void Game::handlePlayerTile() {
    const TileCoord t = mPlayer.currentTile(mMap, mTileSize);
    const char c = mMap.cell(t.x, t.y);

    if (c == '.') {
        mScore += 10;
        mDotsEatenThisLevel += 1;
        mMap.setCell(t.x, t.y, ' ');
        mAudio.playSound("waka");
    } else if (c == 'o') {
        mScore += 50;
        mDotsEatenThisLevel += 1;
        mMap.setCell(t.x, t.y, ' ');
        mAudio.playSound("power");

        mFrightenedTimer = 6.0f;
        for (auto& g : mGhosts) {
            if (g.mode() != GhostMode::Eaten) {
                g.setMode(GhostMode::Frightened);
                g.reverse();
            }
        }
    }
}

void Game::updateFruit(float dt) {
    // Classic-inspired: spawn a bonus fruit once per level after enough dots.
    if (!mFruitSpawnedThisLevel && mDotsEatenThisLevel >= 50) {
        mFruitSpawnedThisLevel = true;
        mFruitActive = true;
        mFruitTimer = 10.f;
    }

    if (mFruitActive) {
        mFruitTimer -= dt;
        if (mFruitTimer <= 0.f) {
            mFruitActive = false;
            mFruitTimer = 0.f;
        }

        const TileCoord t = mPlayer.currentTile(mMap, mTileSize);
        if (t == mFruitTile) {
            mFruitActive = false;
            mFruitTimer = 0.f;
            mScore += 500;
            mAudio.playSound("power");
        }
    }
}

void Game::handleCollisions() {
    const float hitR = mTileSize * 0.55f;
    const float hitR2 = hitR * hitR;

    for (auto& g : mGhosts) {
        if (distSq(g.position(), mPlayer.position()) > hitR2) {
            continue;
        }

        if (g.mode() == GhostMode::Frightened) {
            mScore += 200;
            g.setMode(GhostMode::Eaten);
            mAudio.playSound("eat_ghost");
            continue;
        }

        if (g.mode() == GhostMode::Eaten) {
            continue;
        }

        // Player dies.
        mLives -= 1;
        mAudio.playSound("death");

        if (mLives <= 0) {
            mAudio.playSound("gameover");
            setState(State::GameOver);
            return;
        }

        // Reset positions with a short freeze.
        resetEntities();
        mFreezeTimer = 0.8f;
        return;
    }
}

void Game::update(float dt) {
    if (mState != State::Playing) {
        return;
    }

    pollControllerInput();

    if (mFreezeTimer > 0.f) {
        mFreezeTimer -= dt;
        if (mFreezeTimer < 0.f) mFreezeTimer = 0.f;
        return;
    }

    updateGhostMode(dt);

    mPlayer.update(dt, mMap, mTileSize);
    handlePlayerTile();
    updateFruit(dt);

    // Update ghosts
    for (auto& g : mGhosts) {
        const TileCoord target = chaseTargetFor(g);
        g.update(dt, mMap, mTileSize, target, mRng);
    }

    handleCollisions();

    if (!mMap.hasDotsOrPellets()) {
        mLevel += 1;
        loadLevel(mLevel);
        setState(State::Playing);
    }
}

void Game::render() {
    mRenderer.beginFrame();

    mRenderer.drawMap(mMap, mTileSize);

    if (mState == State::Playing || mState == State::Paused || mState == State::GameOver) {
        mRenderer.drawPlayer(mPlayer, mTileSize);
        for (const auto& g : mGhosts) {
            mRenderer.drawGhost(g, mTileSize);
        }
        if (mFruitActive) {
            mRenderer.drawFruit(mFruitTile, mTileSize);
        }
        mRenderer.drawHUD(mScore, mLives, mLevel);
    }

    if (mState == State::MainMenu) {
        // Background already drawn. Just draw the menu on top.
        mRenderer.drawMenu(mMainMenu);
        } else if (mState == State::Paused) {
            mRenderer.drawMenu(mPauseMenu);
        } else if (mState == State::GameOver) {
            mRenderer.drawOverlayText("GAME OVER", "Press Enter/Space to return");
        } else if (mState == State::Options) {
            mRenderer.drawOverlayText("OPTIONS", "Arrows: change preset  Esc: back");
        }


    mRenderer.endFrame();
}

void Game::toggleFullscreen() {
    mIsFullscreen = !mIsFullscreen;
    
    if (mIsFullscreen) {
        // Get desktop video mode for fullscreen
        sf::VideoMode desktop = sf::VideoMode::getDesktopMode();
        mWindow.create(desktop, "Pac-Man (SFML)", sf::Style::Fullscreen);
    } else {
        // Return to windowed mode
        mWindow.create(sf::VideoMode(960, 720), "Pac-Man (SFML)", sf::Style::Default);
    }
    
    mWindow.setVerticalSyncEnabled(true);
    mRenderer.handleResize();
}
