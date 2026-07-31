#include "CSoundSystem.h"
#include <iostream>

std::unique_ptr<CSoundSystem> g_pSoundSystem = std::make_unique<CSoundSystem>();

void CSoundSystem::Init() {
    InitAudioDevice();
    if (!IsAudioDeviceReady()) {
        std::cerr << "[CSoundSystem] Failed to initialize audio device!" << std::endl;
    } else {
        std::cout << "[CSoundSystem] Audio device initialized successfully." << std::endl;
    }

    LoadActionSound("footstep_walk", "assets/sounds/footstep_walk.mp3");
    LoadActionSound("footstep_run", "assets/sounds/footstep_run.mp3");
    LoadActionSound("white_noise", "assets/sounds/white_noise.wav");
    LoadActionSound("cosmic_noise", "assets/sounds/cosmic_noise.wav");
    LoadActionSound("click", "assets/sounds/click.wav");
    LoadActionSound("scream", "assets/sounds/scream.mp3");
    LoadActionSound("mumbling", "assets/sounds/mumbling.wav");
}

void CSoundSystem::Shutdown() {
    // Unload all sounds
    for (auto& pair : m_sounds) {
        UnloadSound(pair.second);
    }
    m_sounds.clear();

    // Unload all music streams
    for (auto& pair : m_musicStreams) {
        UnloadMusicStream(pair.second);
    }
    m_musicStreams.clear();

    CloseAudioDevice();
    std::cout << "[CSoundSystem] Audio device closed." << std::endl;
}

void CSoundSystem::Update() {
    // Music streams need to be updated every frame to keep buffering
    for (auto& pair : m_musicStreams) {
        if (IsMusicStreamPlaying(pair.second)) {
            UpdateMusicStream(pair.second);
        }
    }
}

bool CSoundSystem::LoadActionSound(const std::string& name, const std::string& path) {
    if (!IsAudioDeviceReady()) return false;
    
    Sound sound = LoadSound(path.c_str());
    if (sound.stream.buffer == nullptr) {
        std::cerr << "[CSoundSystem] Failed to load sound: " << path << std::endl;
        return false;
    }
    
    m_sounds[name] = sound;
    return true;
}

void CSoundSystem::PlayActionSound(const std::string& name) {
    auto it = m_sounds.find(name);
    if (it != m_sounds.end()) {
        StopSound(it->second);
        PlaySound(it->second);
    }
}

bool CSoundSystem::IsActionSoundPlaying(const std::string& name) {
    auto it = m_sounds.find(name);
    if (it != m_sounds.end()) {
        return IsSoundPlaying(it->second);
    }
    return false;
}

void CSoundSystem::StopActionSound(const std::string& name) {
    auto it = m_sounds.find(name);
    if (it != m_sounds.end()) {
        StopSound(it->second);
    }
}

void CSoundSystem::SetActionSoundVolume(const std::string& name, float volume) {
    auto it = m_sounds.find(name);
    if (it != m_sounds.end()) {
        SetSoundVolume(it->second, volume);
    }
}

bool CSoundSystem::LoadLoopingMusic(const std::string& name, const std::string& path) {
    if (!IsAudioDeviceReady()) return false;
    
    Music music = LoadMusicStream(path.c_str());
    if (music.stream.buffer == nullptr) {
        std::cerr << "[CSoundSystem] Failed to load music stream: " << path << std::endl;
        return false;
    }
    
    music.looping = true; // Set to loop automatically
    m_musicStreams[name] = music;
    return true;
}

void CSoundSystem::PlayLoopingMusic(const std::string& name) {
    auto it = m_musicStreams.find(name);
    if (it != m_musicStreams.end()) {
        PlayMusicStream(it->second);
    }
}

void CSoundSystem::StopLoopingMusic(const std::string& name) {
    auto it = m_musicStreams.find(name);
    if (it != m_musicStreams.end()) {
        StopMusicStream(it->second);
    }
}

void CSoundSystem::PauseLoopingMusic(const std::string& name) {
    auto it = m_musicStreams.find(name);
    if (it != m_musicStreams.end()) {
        PauseMusicStream(it->second);
    }
}

void CSoundSystem::ResumeLoopingMusic(const std::string& name) {
    auto it = m_musicStreams.find(name);
    if (it != m_musicStreams.end()) {
        ResumeMusicStream(it->second);
    }
}

void CSoundSystem::SetLoopingMusicVolume(const std::string& name, float volume) {
    auto it = m_musicStreams.find(name);
    if (it != m_musicStreams.end()) {
        SetMusicVolume(it->second, volume);
    }
}
