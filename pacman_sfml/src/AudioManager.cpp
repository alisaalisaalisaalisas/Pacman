#include "AudioManager.h"

#include <iostream>

bool AudioManager::loadSound(const std::string& id, const std::string& path) {
    sf::SoundBuffer buf;
    if (!buf.loadFromFile(path)) {
        std::cerr << "Failed to load sound: " << path << "\n";
        return false;
    }

    mBuffers[id] = std::move(buf);
    sf::Sound snd;
    snd.setBuffer(mBuffers[id]);
    snd.setVolume(mVolume);
    mSounds[id] = std::move(snd);
    return true;
}

void AudioManager::playSound(const std::string& id) {
    auto it = mSounds.find(id);
    if (it == mSounds.end()) {
        return;
    }

    // Restart sound to feel responsive.
    it->second.stop();
    it->second.play();
}

bool AudioManager::playMusic(const std::string& path, bool loop) {
    if (!mMusic.openFromFile(path)) {
        std::cerr << "Failed to open music: " << path << "\n";
        return false;
    }
    mMusic.setLoop(loop);
    mMusic.setVolume(mVolume);
    mMusic.play();
    return true;
}

void AudioManager::stopMusic() {
    mMusic.stop();
}

void AudioManager::setMasterVolume(float v01) {
    if (v01 < 0.f) v01 = 0.f;
    if (v01 > 1.f) v01 = 1.f;
    mVolume = 100.f * v01;

    for (auto& kv : mSounds) {
        kv.second.setVolume(mVolume);
    }
    mMusic.setVolume(mVolume);
}
