#pragma once

#include <string_view>
#include <vector>

#include "fmod.hpp"
#include "audio_buffer.hpp"

struct CustomSound {
    void play();
    
    FMOD::Sound* fmod_sound{ nullptr };
    class SoundManager* sound_manager{ nullptr };
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

    CustomSound get_sound(const char* path);
    void release_sound(CustomSound sound);
    void play_sound(CustomSound sound);

private:
    DecodeAudioFile* m_DecodeFunction{ nullptr };

    void* m_FmodSystem{ nullptr };

    FMOD::CreateSound* m_CreateSound{ nullptr };
    FMOD::ReleaseSound* m_ReleaseSound{ nullptr };
    FMOD::PlaySound* m_PlaySound{ nullptr };

    struct Sound;
    std::vector<Sound> m_SoundStorage;
};
