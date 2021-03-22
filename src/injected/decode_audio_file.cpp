#include "decode_audio_file.hpp"

#include <memory>
#include <cassert>

#pragma warning(push, 0)
#include <libnyquist/Decoders.h>
#pragma warning(pop)

DecodedAudioBuffer LoadAudioFile(const char* file_path)
{
	nqr::AudioData decoded_data;
	nqr::NyquistIO loader;
	loader.Load(&decoded_data, file_path);

	const auto data_size = decoded_data.samples.size() * (GetFormatBitsPerSample(decoded_data.sourceFormat) / 8);
	auto data = std::make_unique<std::byte[]>(data_size);
	if (decoded_data.sourceFormat == nqr::PCM_FLT) {
		memcpy(data.get(), decoded_data.samples.data(), data_size);
	}
	else {
		nqr::ConvertFromFloat32((std::uint8_t*)data.get(), decoded_data.samples.data(), decoded_data.samples.size(), decoded_data.sourceFormat);
	}
	return DecodedAudioBuffer{
		decoded_data.channelCount,
		decoded_data.sampleRate,
		static_cast<SoundFormat>(decoded_data.sourceFormat - 1),
		std::move(data),
		data_size
	};
}
