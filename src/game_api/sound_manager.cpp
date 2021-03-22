#include "sound_manager.hpp"

#include "logger.h"
#include "memory.hpp"

#include <mutex>
#include <string>

#include <Windows.h>

#define SOL_ALL_SAFETIES_ON 1
#include <sol/function.hpp>


struct ChannelControlCollbackData
{
	ChannelControlCollbackData(const ChannelControlCollbackData&) = delete;
	ChannelControlCollbackData& operator=(const ChannelControlCollbackData&) = delete;
	ChannelControlCollbackData(ChannelControlCollbackData&&) = default;
	ChannelControlCollbackData& operator=(ChannelControlCollbackData&&) = default;

	FMOD::Channel* channel{ nullptr };
	SoundCallbackFunction callback;
};
std::mutex s_CallbacksMutex;
std::vector<ChannelControlCollbackData> s_Callbacks;
FMOD::FMOD_RESULT ChannelControlCallback(FMOD::ChannelControl* channel_control, FMOD::ChannelControlType channel_control_type,
	FMOD::ChannelControlCallbackType channel_control_callback_type, void* calback_data_1, void* calback_data_2)
{
	if (channel_control_type == FMOD::ChannelControlType::Channel && channel_control_callback_type == FMOD::ChannelControlCallbackType::End)
	{
		FMOD::Channel* channel = reinterpret_cast<FMOD::Channel*>(channel_control);

		std::lock_guard lock{ s_CallbacksMutex };
		auto it = std::find_if(s_Callbacks.begin(), s_Callbacks.end(),
			[channel](const ChannelControlCollbackData& callback) {
			return callback.channel == channel;
		});
		if (it != s_Callbacks.end()) {
			it->callback();
			s_Callbacks.erase(it);
		}
	}
	// TODO: Cleanup old channels?
	return FMOD::OK;
}

CustomSound::CustomSound(const CustomSound& rhs)
{
	m_FmodSound = rhs.m_FmodSound;
	m_SoundManager = rhs.m_SoundManager;
	if (m_SoundManager != nullptr)
	{
		m_SoundManager->acquire_sound(rhs.m_FmodSound);
	}
}
CustomSound::CustomSound(CustomSound&& rhs) noexcept
{
	std::swap(m_FmodSound, rhs.m_FmodSound);
	std::swap(m_SoundManager, rhs.m_SoundManager);
}
CustomSound::CustomSound(FMOD::Sound* fmod_sound, SoundManager* sound_manager)
	: m_FmodSound{ fmod_sound }
	, m_SoundManager{ sound_manager }
{}
CustomSound::~CustomSound()
{
	if (m_SoundManager != nullptr)
	{
		m_SoundManager->release_sound(m_FmodSound);
	}
}

PlayingSound CustomSound::play()
{
	return play(false, SoundType::Sfx);
}
PlayingSound CustomSound::play(bool paused)
{
	return play(paused, SoundType::Sfx);
}
PlayingSound CustomSound::play(bool paused, SoundType sound_type)
{
	return m_SoundManager->play_sound(m_FmodSound, paused, sound_type == SoundType::Music);
}


PlayingSound::PlayingSound(FMOD::Channel* fmod_channel, SoundManager* sound_manager)
	: m_FmodChannel{ fmod_channel }
	, m_SoundManager{ sound_manager }
{
}

bool PlayingSound::is_playing()
{
	return m_SoundManager->is_playing(*this);
}
bool PlayingSound::stop()
{
	return m_SoundManager->stop(*this);
}
bool PlayingSound::set_pause(bool pause)
{
	return m_SoundManager->set_pause(*this, pause);
}
bool PlayingSound::set_mute(bool mute)
{
	return m_SoundManager->set_mute(*this, mute);
}
bool PlayingSound::set_pitch(float pitch)
{
	return m_SoundManager->set_pitch(*this, pitch);
}
bool PlayingSound::set_pan(float pan)
{
	return m_SoundManager->set_pan(*this, pan);
}
bool PlayingSound::set_volume(float volume)
{
	return m_SoundManager->set_volume(*this, volume);
}
bool PlayingSound::set_looping(LoopMode loop_mode)
{
	return m_SoundManager->set_looping(*this, loop_mode);
}
bool PlayingSound::set_callback(SoundCallbackFunction&& callback)
{
	return m_SoundManager->set_callback(*this, std::move(callback));
}


struct SoundManager::Sound {
	std::uint32_t ref_count;
	DecodedAudioBuffer buffer;
	std::string path;
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
		auto fmod_studio_system = *(FMODStudio::System**)memory.at_exe(decode_pc(exe, location));

		auto get_core_system = reinterpret_cast<FMODStudio::GetCoreSystem*>(GetProcAddress(fmod_studio, "FMOD_Studio_System_GetCoreSystem"));
		{
			auto err = get_core_system(fmod_studio_system, &m_FmodSystem);
			if (err != FMOD::OK)
			{
				DEBUG("Could not get Fmod System, custom audio won't work...");
			}
		}

		auto flush_commands = reinterpret_cast<FMODStudio::FlushCommands*>(GetProcAddress(fmod_studio, "FMOD_Studio_System_FlushCommands"));
		auto get_bus = reinterpret_cast<FMODStudio::GetBus*>(GetProcAddress(fmod_studio, "FMOD_Studio_System_GetBus"));
		auto lock_channel_group = reinterpret_cast<FMODStudio::LockChannelGroup*>(GetProcAddress(fmod_studio, "FMOD_Studio_Bus_LockChannelGroup"));
		auto get_channel_group = reinterpret_cast<FMODStudio::GetChannelGroup*>(GetProcAddress(fmod_studio, "FMOD_Studio_Bus_GetChannelGroup"));

		auto get_channel_group_from_bus_name = [=](const char* bus_name, FMOD::ChannelGroup** channel_group) {
			FMODStudio::Bus* bus{ nullptr };
			auto err = get_bus(fmod_studio_system, bus_name, &bus);
			if (err != FMOD::OK)
			{
				DEBUG("Could not get bus '{}', custom audio volume won't be synced with game volume properly...", bus_name);
			}
			else {
				err = lock_channel_group(bus);
				if (err != FMOD::OK)
				{
					DEBUG("Could not lock channel group for bus '{}', custom audio volume won't be synced with game volume properly...", bus_name);
				}
				else
				{
					err = flush_commands(fmod_studio_system);
					if (err != FMOD::OK)
					{
						DEBUG("Could not flush commands after locking channel group for bus '{}', custom audio volume won't be synced with game volume properly...", bus_name);
					}
					else {
						err = get_channel_group(bus, channel_group);
						if (err != FMOD::OK)
						{
							DEBUG("Could not obtain channel group for bus '{}', custom audio volume won't be synced with game volume properly...", bus_name);
						}
					}
				}
			}
		};
		get_channel_group_from_bus_name("bus:/Master_SUM/Master_SFX", &m_SfxChannelGroup);
		get_channel_group_from_bus_name("bus:/Master_SUM/Master_BGM", &m_MusicChannelGroup);
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

		m_ChannelIsPlaying = reinterpret_cast<FMOD::ChannelIsPlaying*>(GetProcAddress(fmod, "FMOD_Channel_IsPlaying"));
		m_ChannelStop = reinterpret_cast<FMOD::ChannelStop*>(GetProcAddress(fmod, "FMOD_Channel_Stop"));
		m_ChannelSetPaused = reinterpret_cast<FMOD::ChannelSetPaused*>(GetProcAddress(fmod, "FMOD_Channel_SetPaused"));
		m_ChannelSetMute = reinterpret_cast<FMOD::ChannelSetMute*>(GetProcAddress(fmod, "FMOD_Channel_SetMute"));
		m_ChannelSetPitch = reinterpret_cast<FMOD::ChannelSetPitch*>(GetProcAddress(fmod, "FMOD_Channel_SetPitch"));
		m_ChannelSetPan = reinterpret_cast<FMOD::ChannelSetPan*>(GetProcAddress(fmod, "FMOD_Channel_SetPan"));
		m_ChannelSetVolume = reinterpret_cast<FMOD::ChannelSetVolume*>(GetProcAddress(fmod, "FMOD_Channel_SetVolume"));
		m_ChannelSetMode = reinterpret_cast<FMOD::ChannelSetMode*>(GetProcAddress(fmod, "FMOD_Channel_SetMode"));
		m_ChannelSetCallback = reinterpret_cast<FMOD::ChannelSetCallback*>(GetProcAddress(fmod, "FMOD_Channel_SetCallback"));
		m_ChannelSetUserData = reinterpret_cast<FMOD::ChannelSetUserData*>(GetProcAddress(fmod, "FMOD_Channel_SetUserData"));
		m_ChannelGetUserData = reinterpret_cast<FMOD::ChannelGetUserData*>(GetProcAddress(fmod, "FMOD_Channel_GetUserData"));
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

CustomSound SoundManager::get_sound(std::string path)
{
	auto it = std::find_if(m_SoundStorage.begin(), m_SoundStorage.end(), [&path](const Sound& sound) {
		return sound.path == path;
	});
	if (it != m_SoundStorage.end())
	{
		it->ref_count++;
		return CustomSound{ it->fmod_sound, this };
	}

	DecodedAudioBuffer buffer;
	try {
		buffer = m_DecodeFunction(path.c_str());
	}
	catch (std::exception& except) {
		DEBUG("Failed loading audio file {}\n{}", path, except.what());
		return CustomSound{ nullptr, nullptr };
	}

	Sound new_sound;
	new_sound.ref_count = 1;
	new_sound.buffer = std::move(buffer);
	new_sound.path = std::move(path);

	FMOD::FMOD_MODE mode = (FMOD::FMOD_MODE)(FMOD::MODE_CREATESAMPLE | FMOD::MODE_OPENMEMORY_POINT | FMOD::MODE_OPENRAW | FMOD::MODE_IGNORETAGS | FMOD::MODE_LOOP_OFF);

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
CustomSound SoundManager::get_sound(const char* path)
{
	return get_sound(std::string{ path });
}
void SoundManager::acquire_sound(FMOD::Sound* fmod_sound)
{
	auto it = std::find_if(m_SoundStorage.begin(), m_SoundStorage.end(), [fmod_sound](const Sound& sound) { return sound.fmod_sound == fmod_sound; });
	if (it == m_SoundStorage.end())
	{
		DEBUG("Trying to acquire sound that does not exist...");
		return;
	}

	it->ref_count++;
}
void SoundManager::release_sound(FMOD::Sound* fmod_sound)
{
	auto it = std::find_if(m_SoundStorage.begin(), m_SoundStorage.end(), [fmod_sound](const Sound& sound) { return sound.fmod_sound == fmod_sound; });
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
PlayingSound SoundManager::play_sound(FMOD::Sound* fmod_sound, bool paused, bool as_music)
{
	FMOD::Channel* channel{ nullptr };
	m_PlaySound(m_FmodSystem, fmod_sound, as_music ? m_MusicChannelGroup : m_SfxChannelGroup, paused, &channel);
	return PlayingSound{ channel, this };
}

bool SoundManager::is_playing(PlayingSound playing_sound)
{
	bool is_playing{ false };
	return FMOD_CHECK_CALL(m_ChannelIsPlaying(playing_sound.m_FmodChannel, &is_playing)) && is_playing;
}
bool SoundManager::stop(PlayingSound playing_sound)
{
	return FMOD_CHECK_CALL(m_ChannelStop(playing_sound.m_FmodChannel));
}
bool SoundManager::set_pause(PlayingSound playing_sound, bool pause)
{
	return FMOD_CHECK_CALL(m_ChannelSetPaused(playing_sound.m_FmodChannel, pause));
}
bool SoundManager::set_mute(PlayingSound playing_sound, bool mute)
{
	return FMOD_CHECK_CALL(m_ChannelSetMute(playing_sound.m_FmodChannel, mute));
}
bool SoundManager::set_pitch(PlayingSound playing_sound, float pitch)
{
	return FMOD_CHECK_CALL(m_ChannelSetPitch(playing_sound.m_FmodChannel, pitch));
}
bool SoundManager::set_pan(PlayingSound playing_sound, float pan)
{
	return FMOD_CHECK_CALL(m_ChannelSetPan(playing_sound.m_FmodChannel, pan));
}
bool SoundManager::set_volume(PlayingSound playing_sound, float volume)
{
	return FMOD_CHECK_CALL(m_ChannelSetVolume(playing_sound.m_FmodChannel, std::clamp(volume, -1.0f, 1.0f)));
}
bool SoundManager::set_looping(PlayingSound playing_sound, LoopMode loop_mode)
{
	if (m_ChannelSetMode)
	{
		switch (loop_mode)
		{
		case LoopMode::Off:
			return FMOD_CHECK_CALL(m_ChannelSetMode(playing_sound.m_FmodChannel, FMOD::MODE_LOOP_OFF));
		case LoopMode::Loop:
			return FMOD_CHECK_CALL(m_ChannelSetMode(playing_sound.m_FmodChannel, FMOD::MODE_LOOP_NORMAL));
		case LoopMode::Bidirectional:
			return FMOD_CHECK_CALL(m_ChannelSetMode(playing_sound.m_FmodChannel, FMOD::MODE_LOOP_BIDI));
		}
	}
	return false;
}
bool SoundManager::set_callback(PlayingSound playing_sound, SoundCallbackFunction&& callback)
{
	std::lock_guard lock{ s_CallbacksMutex };
	if (FMOD_CHECK_CALL(m_ChannelSetCallback(playing_sound.m_FmodChannel, &ChannelControlCallback)))
	{
		s_Callbacks.push_back(ChannelControlCollbackData{ playing_sound.m_FmodChannel, std::move(callback) });
		return true;
	}
	return false;
}
