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

    // Инициализация и завершение работы аудио-девайса
    void Init();
    void Shutdown();

    // Обновление потоков музыки (необходимо вызывать каждый кадр)
    void Update();

    // --- Короткие звуки (выстрелы, шаги, клики) ---
    // Загрузить звук в память
    bool LoadActionSound(const std::string& name, const std::string& path);
    // Проиграть загруженный звук
    void PlayActionSound(const std::string& name);
    // Проверить, играет ли звук сейчас
    bool IsActionSoundPlaying(const std::string& name);
    // Остановить звук (если он еще играет)
    void StopActionSound(const std::string& name);
    // Установить громкость (0.0f - 1.0f)
    void SetActionSoundVolume(const std::string& name, float volume);

    // --- Длинные/зацикленные потоки (фоновый эмбиент, музыка, гудение ламп) ---
    // Загрузить потоковую музыку (стримится с диска)
    bool LoadLoopingMusic(const std::string& name, const std::string& path);
    // Начать играть потоковую музыку
    void PlayLoopingMusic(const std::string& name);
    // Остановить
    void StopLoopingMusic(const std::string& name);
    // Поставить на паузу
    void PauseLoopingMusic(const std::string& name);
    // Снять с паузы
    void ResumeLoopingMusic(const std::string& name);
    // Установить громкость (0.0f - 1.0f)
    void SetLoopingMusicVolume(const std::string& name, float volume);

private:
    std::unordered_map<std::string, Sound> m_sounds;
    std::unordered_map<std::string, Music> m_musicStreams;
};

extern std::unique_ptr<CSoundSystem> g_pSoundSystem;
