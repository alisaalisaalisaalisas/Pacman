#include "Game.h"

#include <iostream>
#include <fstream>
#include <cstdlib>

int main() {
    // Open log file for debugging
    std::ofstream logFile("pacman_debug.log");
    if (!logFile) {
        std::cerr << "Failed to open log file!" << std::endl;
    }
    
    auto log = [&](const std::string& msg) {
        std::cout << msg << std::endl;
        logFile << msg << std::endl;
        logFile.flush();
    };
    
    log("=== Pac-Man Debug Log ===");
    log("Starting application...");
    
    try {
        log("Creating Game object...");
        Game game;
        log("Game object created successfully!");
        
        log("Starting game loop...");
        int result = game.run();
        log("Game exited with code: " + std::to_string(result));
        
        return result;
    } catch (const std::exception& e) {
        log("EXCEPTION: " + std::string(e.what()));
        return 1;
    } catch (...) {
        log("UNKNOWN EXCEPTION occurred!");
        return 1;
    }
}
