#include "decode_audio_file.hpp"

#include <cstddef>     // for byte
#include <cstring>     // for memcpy
#include <memory>      // for make_unique, unique_ptr
#include <type_traits> // for move
#include <vector>      // for vector

#if defined(__clang__)
#pragma clang diagnostic push
#pragma clang diagnostic ignored "-Wall"
#pragma clang diagnostic ignored "-Wextra"
#pragma clang diagnostic ignored "-Wshadow-all"
#else
#pragma warning(push, 0)
#endif

#include <cstdint>               // for uint8_t
#include <libnyquist/Decoders.h> // for AudioData, ConvertFromFloat32, GetF...

#if defined(__clang__)
#pragma clang diagnostic pop
#else
#pragma warning(pop)
#endif

DecodedAudioBuffer LoadAudioFile(const char* file_path)
{
    nqr::AudioData decoded_data;
    nqr::NyquistIO loader;
    loader.Load(&decoded_data, file_path);

    const auto data_size = decoded_data.samples.size() * (GetFormatBitsPerSample(decoded_data.sourceFormat) / 8);
    auto data = std::make_unique<std::byte[]>(data_size + 32); // 16 bytes padding front and back
    if (decoded_data.sourceFormat == nqr::PCM_FLT)
    {
        memcpy(data.get() + 16, decoded_data.samples.data(), data_size);
    }
    else
    {
        nqr::ConvertFromFloat32((std::uint8_t*)data.get() + 16, decoded_data.samples.data(), decoded_data.samples.size(), decoded_data.sourceFormat);
    }
    return DecodedAudioBuffer{
        decoded_data.channelCount, decoded_data.sampleRate, static_cast<SoundFormat>(decoded_data.sourceFormat - 1), std::move(data), data_size};
}
