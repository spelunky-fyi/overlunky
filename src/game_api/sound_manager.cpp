#include "sound_manager.hpp"

#include "logger.h"
#include "memory.hpp"

#include <string>

#include <Windows.h>
// Y tho Windows  T.T
#undef PlaySound

void CustomSound::play()
{
	sound_manager->play_sound(*this);
}

struct SoundManager::Sound {
	std::uint32_t ref_count;
	std::string path;
	DecodedAudioBuffer buffer;
	FMOD::Sound* fmod_sound{ nullptr };
};

SoundManager::SoundManager(DecodeAudioFile* decode_function)
	: m_DecodeFunction{ decode_function }
{
	if (HMODULE fmod_studio = GetModuleHandle("fmodstudio.dll"))
	{
		auto memory = Memory::get();
		auto exe = memory.exe();
		auto start = memory.after_bundle;
		auto location = find_inst(exe, "\x48\x8d", find_inst(exe, "\x85\xc0\x74\x2b\x44\x8b\xc0"s, start) - 0x10);
		auto fmod_studio_system = (FMODStudio::System**)memory.at_exe(decode_pc(exe, location));

		auto get_core_system = reinterpret_cast<FMODStudio::GetCoreSystem*>(GetProcAddress(fmod_studio, "FMOD_Studio_System_GetCoreSystem"));
		auto err = get_core_system(*fmod_studio_system, &m_FmodSystem);
		if (err != FMOD::OK)
		{
			DEBUG("Could not get Fmod System, custom audio won't work...");
		}
	}
	else
	{
		DEBUG("fmodstudio.dll was not loaded, custom audio won't work...");
	}

	if (HMODULE fmod = GetModuleHandle("fmod.dll"))
	{
		m_CreateSound = reinterpret_cast<FMOD::CreateSound*>(GetProcAddress(fmod, "FMOD_System_CreateSound"));
		m_ReleaseSound = reinterpret_cast<FMOD::ReleaseSound*>(GetProcAddress(fmod, "FMOD_Sound_Release"));
		m_PlaySound = reinterpret_cast<FMOD::PlaySound*>(GetProcAddress(fmod, "FMOD_System_PlaySound"));
	}
	else
	{
		DEBUG("fmod.dll was not loaded, custom audio won't work...");
	}
}
SoundManager::~SoundManager()
{
	for (Sound& sound : m_SoundStorage)
	{
		m_ReleaseSound(sound.fmod_sound);
	}
}

CustomSound SoundManager::get_sound(const char* path)
{
	auto it = std::find_if(m_SoundStorage.begin(), m_SoundStorage.end(), [path](const Sound& sound) { return sound.path == path; });
	if (it != m_SoundStorage.end())
	{
		it->ref_count++;
		return CustomSound{ it->fmod_sound, this };
	}

	Sound new_sound;
	new_sound.ref_count = 1;
	new_sound.path = path;
	new_sound.buffer = m_DecodeFunction(path);

	FMOD::FMOD_MODE mode = (FMOD::FMOD_MODE)(FMOD::MODE_CREATESAMPLE | FMOD::MODE_OPENMEMORY_POINT | FMOD::MODE_OPENRAW | FMOD::MODE_LOOP_OFF | FMOD::MODE_IGNORETAGS);

	FMOD::CREATESOUNDEXINFO create_sound_exinfo{};
	create_sound_exinfo.cbsize = sizeof(create_sound_exinfo);
	create_sound_exinfo.length = (std::uint32_t)new_sound.buffer.data_size;
	create_sound_exinfo.numchannels = new_sound.buffer.num_channels;
	create_sound_exinfo.defaultfrequency = new_sound.buffer.frequency;
	create_sound_exinfo.format = [path](SoundFormat format) {
		switch (format) {
		default:
			DEBUG("Sound format is not supported for file {}...", path);
			return FMOD::SOUND_FORMAT::NONE;
		case SoundFormat::PCM_8:
			return FMOD::SOUND_FORMAT::PCM8;
		case SoundFormat::PCM_16:
			return FMOD::SOUND_FORMAT::PCM16;
		case SoundFormat::PCM_24:
			return FMOD::SOUND_FORMAT::PCM24;
		case SoundFormat::PCM_32:
			return FMOD::SOUND_FORMAT::PCM32;
		case SoundFormat::PCM_FLOAT:
			return FMOD::SOUND_FORMAT::PCMFLOAT;
		}
	}(new_sound.buffer.format);

	FMOD::FMOD_RESULT err = m_CreateSound(m_FmodSystem, (const char*)new_sound.buffer.data.get(), mode, &create_sound_exinfo, &new_sound.fmod_sound);
	if (err != FMOD::OK)
	{
		return CustomSound{ nullptr, nullptr };
	}
	
	m_SoundStorage.push_back(std::move(new_sound));
	return CustomSound{ m_SoundStorage.back().fmod_sound, this };
}
void SoundManager::release_sound(CustomSound sound)
{
	auto it = std::find_if(m_SoundStorage.begin(), m_SoundStorage.end(), [fmod_sound = sound.fmod_sound](const Sound& sound) { return sound.fmod_sound == fmod_sound; });
	if (it == m_SoundStorage.end())
	{
		DEBUG("Trying to release sound that does not exist...");
		return;
	}

	// TODO: Really worth releasing or should we just keep this for eternity?
	if (it->ref_count == 1)
	{
		m_ReleaseSound(it->fmod_sound);
		m_SoundStorage.erase(it);
	}
	else
	{
		it->ref_count--;
	}
}
void SoundManager::play_sound(CustomSound sound)
{
	// TODO: Get back channel for more control
	m_PlaySound(m_FmodSystem, sound.fmod_sound, nullptr, false, nullptr);
}
