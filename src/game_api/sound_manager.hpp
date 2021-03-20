#pragma once

#include <string>
#include <vector>

#include "fmod.hpp"
#include "audio_buffer.hpp"

class SoundManager;

struct CustomSound
{
    friend class SoundManager;

public:
    CustomSound(const CustomSound& rhs);
    CustomSound(CustomSound&& rhs) noexcept;
    CustomSound operator=(const CustomSound& rhs) = delete;
    CustomSound operator=(CustomSound&& rhs) = delete;
    ~CustomSound();

    operator bool() { return m_SoundManager != nullptr; }

    void play();
    
private:
    CustomSound(FMOD::Sound* fmod_sound, SoundManager* sound_manager);

    FMOD::Sound* m_FmodSound{ nullptr };
    SoundManager* m_SoundManager{ nullptr };
};

class SoundManager
{
public:
    SoundManager(DecodeAudioFile* decode_function);
    ~SoundManager();

    SoundManager(const SoundManager&) = delete;
    SoundManager(SoundManager&&) = delete;
    SoundManager& operator=(const SoundManager&) = delete;
    SoundManager& operator=(SoundManager&&) = delete;

    CustomSound get_sound(std::string path, bool loop);
    CustomSound get_sound(const char* path, bool loop);
    void acquire_sound(FMOD::Sound* fmod_sound);
    void release_sound(FMOD::Sound* fmod_sound);
    void play_sound(FMOD::Sound* fmod_sound);

private:
    DecodeAudioFile* m_DecodeFunction{ nullptr };

    void* m_FmodSystem{ nullptr };

    FMOD::CreateSound* m_CreateSound{ nullptr };
    FMOD::ReleaseSound* m_ReleaseSound{ nullptr };
    FMOD::PlaySound* m_PlaySound{ nullptr };

    struct Sound;
    std::vector<Sound> m_SoundStorage;
};
