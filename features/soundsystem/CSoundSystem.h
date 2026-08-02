#pragma once
#include <string>
#include <unordered_map>
#include <vector>
#include <memory>
#include "raylib.h"
class CSoundSystem {
public:
    CSoundSystem() = default;
    ~CSoundSystem() = default;
    void Init();
    void Shutdown();
    void Update();
    bool LoadActionSound(const std::string& name, const std::string& path);
    void PlayActionSound(const std::string& name);
    bool IsActionSoundPlaying(const std::string& name);
    void StopActionSound(const std::string& name);
    void SetActionSoundVolume(const std::string& name, float volume);
    bool LoadLoopingMusic(const std::string& name, const std::string& path);
    void PlayLoopingMusic(const std::string& name);
    void StopLoopingMusic(const std::string& name);
    void PauseLoopingMusic(const std::string& name);
    void ResumeLoopingMusic(const std::string& name);
    void SetLoopingMusicVolume(const std::string& name, float volume);
private:
    std::unordered_map<std::string, Sound> m_sounds;
    std::unordered_map<std::string, Music> m_musicStreams;
};
extern std::unique_ptr<CSoundSystem> g_pSoundSystem;