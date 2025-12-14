#pragma once

#include <SFML/Audio/Music.hpp>
#include <SFML/Audio/Sound.hpp>
#include <SFML/Audio/SoundBuffer.hpp>

#include <string>
#include <unordered_map>

class AudioManager {
public:
    bool loadSound(const std::string& id, const std::string& path);
    void playSound(const std::string& id);

    bool playMusic(const std::string& path, bool loop = true);
    void stopMusic();

    void setMasterVolume(float v01);

private:
    std::unordered_map<std::string, sf::SoundBuffer> mBuffers;
    std::unordered_map<std::string, sf::Sound> mSounds;

    sf::Music mMusic;
    float mVolume = 100.f;
};
