#pragma once

#include "AudioManager.h"
#include "Ghost.h"
#include "Map.h"
#include "Menu.h"
#include "Player.h"
#include "Renderer.h"

#include <SFML/Graphics/RenderWindow.hpp>

#include <random>
#include <string>
#include <vector>

class Game {
public:
    Game();
    int run();

private:
    enum class State {
        MainMenu,
        Playing,
        Paused,
        GameOver,
        Options,
    };

    void processEvents();
    void update(float dt);
    void render();

    void startNewGame();
    void loadLevel(int level);
    void resetEntities();

    void updateGhostMode(float dt);
    void setAllGhostModes(GhostMode mode, bool reverse);

    void handlePlayerTile();
    void updateFruit(float dt);
    void handleCollisions();

    void pollControllerInput();

    TileCoord chaseTargetFor(const Ghost& ghost) const;

    void setState(State s);

    sf::RenderWindow mWindow;
    Renderer mRenderer;
    AudioManager mAudio;

    State mState = State::MainMenu;

    Menu mMainMenu;
    Menu mPauseMenu;

    Map mMap;
    Player mPlayer;
    std::vector<Ghost> mGhosts;

    float mTileSize = 8.f;
    float mHudHeight = 32.f;

    int mScore = 0;
    int mLives = 3;
    int mLevel = 1;

    int mDotsEatenThisLevel = 0;

    bool mFruitActive = false;
    bool mFruitSpawnedThisLevel = false;
    float mFruitTimer = 0.f;
    TileCoord mFruitTile{0, 0};

    // Ghost mode schedule (Scatter/Chase).
    GhostMode mBaseMode = GhostMode::Scatter;
    float mModeTimer = 0.f;
    std::vector<float> mModePhasesSeconds;
    std::size_t mModePhaseIndex = 0;

    // Frightened
    float mFrightenedTimer = 0.f;

    // Death / reset pacing
    float mFreezeTimer = 0.f;

    std::mt19937 mRng;

    // Fullscreen toggle
    bool mIsFullscreen = false;
    void toggleFullscreen();
};
