#pragma once

#include <cstdint>
#include <memory>

enum class SoundFormat
{
    PCM_8,
    PCM_16,
    PCM_24,
    PCM_32,
    PCM_64,
    PCM_FLOAT,
    PCM_DOUBLE
};

struct DecodedAudioBuffer
{
    std::int32_t num_channels;
    std::int32_t frequency;
    SoundFormat format;
    std::unique_ptr<const std::byte[]> data;
    std::size_t data_size;
};

using DecodeAudioFile = DecodedAudioBuffer(const char* file_path);
