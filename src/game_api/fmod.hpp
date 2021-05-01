#pragma once

#define FMOD_CHECK_CALL(x) [](auto err) { \
	if (err != FMOD::OK) \
	{ \
		DEBUG("{}: {}", #x, FMOD::ErrStr(err)); \
		return false; \
	} \
	return true; \
}((x))

namespace FMOD {
	enum FMOD_RESULT {
		OK,
		ERR_BADCOMMAND,
		ERR_CHANNEL_ALLOC,
		ERR_CHANNEL_STOLEN,
		ERR_DMA,
		ERR_DSP_CONNECTION,
		ERR_DSP_DONTPROCESS,
		ERR_DSP_FORMAT,
		ERR_DSP_INUSE,
		ERR_DSP_NOTFOUND,
		ERR_DSP_RESERVED,
		ERR_DSP_SILENCE,
		ERR_DSP_TYPE,
		ERR_FILE_BAD,
		ERR_FILE_COULDNOTSEEK,
		ERR_FILE_DISKEJECTED,
		ERR_FILE_EOF,
		ERR_FILE_ENDOFDATA,
		ERR_FILE_NOTFOUND,
		ERR_FORMAT,
		ERR_HEADER_MISMATCH,
		ERR_HTTP,
		ERR_HTTP_ACCESS,
		ERR_HTTP_PROXY_AUTH,
		ERR_HTTP_SERVER_ERROR,
		ERR_HTTP_TIMEOUT,
		ERR_INITIALIZATION,
		ERR_INITIALIZED,
		ERR_INTERNAL,
		ERR_INVALID_FLOAT,
		ERR_INVALID_HANDLE,
		ERR_INVALID_PARAM,
		ERR_INVALID_POSITION,
		ERR_INVALID_SPEAKER,
		ERR_INVALID_SYNCPOINT,
		ERR_INVALID_THREAD,
		ERR_INVALID_VECTOR,
		ERR_MAXAUDIBLE,
		ERR_MEMORY,
		ERR_MEMORY_CANTPOINT,
		ERR_NEEDS3D,
		ERR_NEEDSHARDWARE,
		ERR_NET_CONNECT,
		ERR_NET_SOCKET_ERROR,
		ERR_NET_URL,
		ERR_NET_WOULD_BLOCK,
		ERR_NOTREADY,
		ERR_OUTPUT_ALLOCATED,
		ERR_OUTPUT_CREATEBUFFER,
		ERR_OUTPUT_DRIVERCALL,
		ERR_OUTPUT_FORMAT,
		ERR_OUTPUT_INIT,
		ERR_OUTPUT_NODRIVERS,
		ERR_PLUGIN,
		ERR_PLUGIN_MISSING,
		ERR_PLUGIN_RESOURCE,
		ERR_PLUGIN_VERSION,
		ERR_RECORD,
		ERR_REVERB_CHANNELGROUP,
		ERR_REVERB_INSTANCE,
		ERR_SUBSOUNDS,
		ERR_SUBSOUND_ALLOCATED,
		ERR_SUBSOUND_CANTMOVE,
		ERR_TAGNOTFOUND,
		ERR_TOOMANYCHANNELS,
		ERR_TRUNCATED,
		ERR_UNIMPLEMENTED,
		ERR_UNINITIALIZED,
		ERR_UNSUPPORTED,
		ERR_VERSION,
		ERR_EVENT_ALREADY_LOADED,
		ERR_EVENT_LIVEUPDATE_BUSY,
		ERR_EVENT_LIVEUPDATE_MISMATCH,
		ERR_EVENT_LIVEUPDATE_TIMEOUT,
		ERR_EVENT_NOTFOUND,
		ERR_STUDIO_UNINITIALIZED,
		ERR_STUDIO_NOT_LOADED,
		ERR_INVALID_STRING,
		ERR_ALREADY_LOCKED,
		ERR_NOT_LOCKED,
		ERR_RECORD_DISCONNECTED,
		ERR_TOOMANYSAMPLES
	};

	inline const char* ErrStr(FMOD_RESULT err)
	{
#define ERR_CASE(err_enum) case err_enum: return #err_enum
		switch (err)
		{
			ERR_CASE(OK);
			ERR_CASE(ERR_BADCOMMAND);
			ERR_CASE(ERR_CHANNEL_ALLOC);
			ERR_CASE(ERR_CHANNEL_STOLEN);
			ERR_CASE(ERR_DMA);
			ERR_CASE(ERR_DSP_CONNECTION);
			ERR_CASE(ERR_DSP_DONTPROCESS);
			ERR_CASE(ERR_DSP_FORMAT);
			ERR_CASE(ERR_DSP_INUSE);
			ERR_CASE(ERR_DSP_NOTFOUND);
			ERR_CASE(ERR_DSP_RESERVED);
			ERR_CASE(ERR_DSP_SILENCE);
			ERR_CASE(ERR_DSP_TYPE);
			ERR_CASE(ERR_FILE_BAD);
			ERR_CASE(ERR_FILE_COULDNOTSEEK);
			ERR_CASE(ERR_FILE_DISKEJECTED);
			ERR_CASE(ERR_FILE_EOF);
			ERR_CASE(ERR_FILE_ENDOFDATA);
			ERR_CASE(ERR_FILE_NOTFOUND);
			ERR_CASE(ERR_FORMAT);
			ERR_CASE(ERR_HEADER_MISMATCH);
			ERR_CASE(ERR_HTTP);
			ERR_CASE(ERR_HTTP_ACCESS);
			ERR_CASE(ERR_HTTP_PROXY_AUTH);
			ERR_CASE(ERR_HTTP_SERVER_ERROR);
			ERR_CASE(ERR_HTTP_TIMEOUT);
			ERR_CASE(ERR_INITIALIZATION);
			ERR_CASE(ERR_INITIALIZED);
			ERR_CASE(ERR_INTERNAL);
			ERR_CASE(ERR_INVALID_FLOAT);
			ERR_CASE(ERR_INVALID_HANDLE);
			ERR_CASE(ERR_INVALID_PARAM);
			ERR_CASE(ERR_INVALID_POSITION);
			ERR_CASE(ERR_INVALID_SPEAKER);
			ERR_CASE(ERR_INVALID_SYNCPOINT);
			ERR_CASE(ERR_INVALID_THREAD);
			ERR_CASE(ERR_INVALID_VECTOR);
			ERR_CASE(ERR_MAXAUDIBLE);
			ERR_CASE(ERR_MEMORY);
			ERR_CASE(ERR_MEMORY_CANTPOINT);
			ERR_CASE(ERR_NEEDS3D);
			ERR_CASE(ERR_NEEDSHARDWARE);
			ERR_CASE(ERR_NET_CONNECT);
			ERR_CASE(ERR_NET_SOCKET_ERROR);
			ERR_CASE(ERR_NET_URL);
			ERR_CASE(ERR_NET_WOULD_BLOCK);
			ERR_CASE(ERR_NOTREADY);
			ERR_CASE(ERR_OUTPUT_ALLOCATED);
			ERR_CASE(ERR_OUTPUT_CREATEBUFFER);
			ERR_CASE(ERR_OUTPUT_DRIVERCALL);
			ERR_CASE(ERR_OUTPUT_FORMAT);
			ERR_CASE(ERR_OUTPUT_INIT);
			ERR_CASE(ERR_OUTPUT_NODRIVERS);
			ERR_CASE(ERR_PLUGIN);
			ERR_CASE(ERR_PLUGIN_MISSING);
			ERR_CASE(ERR_PLUGIN_RESOURCE);
			ERR_CASE(ERR_PLUGIN_VERSION);
			ERR_CASE(ERR_RECORD);
			ERR_CASE(ERR_REVERB_CHANNELGROUP);
			ERR_CASE(ERR_REVERB_INSTANCE);
			ERR_CASE(ERR_SUBSOUNDS);
			ERR_CASE(ERR_SUBSOUND_ALLOCATED);
			ERR_CASE(ERR_SUBSOUND_CANTMOVE);
			ERR_CASE(ERR_TAGNOTFOUND);
			ERR_CASE(ERR_TOOMANYCHANNELS);
			ERR_CASE(ERR_TRUNCATED);
			ERR_CASE(ERR_UNIMPLEMENTED);
			ERR_CASE(ERR_UNINITIALIZED);
			ERR_CASE(ERR_UNSUPPORTED);
			ERR_CASE(ERR_VERSION);
			ERR_CASE(ERR_EVENT_ALREADY_LOADED);
			ERR_CASE(ERR_EVENT_LIVEUPDATE_BUSY);
			ERR_CASE(ERR_EVENT_LIVEUPDATE_MISMATCH);
			ERR_CASE(ERR_EVENT_LIVEUPDATE_TIMEOUT);
			ERR_CASE(ERR_EVENT_NOTFOUND);
			ERR_CASE(ERR_STUDIO_UNINITIALIZED);
			ERR_CASE(ERR_STUDIO_NOT_LOADED);
			ERR_CASE(ERR_INVALID_STRING);
			ERR_CASE(ERR_ALREADY_LOCKED);
			ERR_CASE(ERR_NOT_LOCKED);
			ERR_CASE(ERR_RECORD_DISCONNECTED);
			ERR_CASE(ERR_TOOMANYSAMPLES);
		}
#undef ERR_CASE
		return "UNKNONW";
	}

	enum FMOD_MODE : std::uint32_t
	{
		MODE_DEFAULT = 0x00000000,
		MODE_LOOP_OFF = 0x00000001,
		MODE_LOOP_NORMAL = 0x00000002,
		MODE_LOOP_BIDI = 0x00000004,
		MODE_2D = 0x00000008,
		MODE_3D = 0x00000010,
		MODE_CREATESTREAM = 0x00000080,
		MODE_CREATESAMPLE = 0x00000100,
		MODE_CREATECOMPRESSEDSAMPLE = 0x00000200,
		MODE_OPENUSER = 0x00000400,
		MODE_OPENMEMORY = 0x00000800,
		MODE_OPENMEMORY_POINT = 0x10000000,
		MODE_OPENRAW = 0x00001000,
		MODE_OPENONLY = 0x00002000,
		MODE_ACCURATETIME = 0x00004000,
		MODE_MPEGSEARCH = 0x00008000,
		MODE_NONBLOCKING = 0x00010000,
		MODE_UNIQUE = 0x00020000,
		MODE_3D_HEADRELATIVE = 0x00040000,
		MODE_3D_WORLDRELATIVE = 0x00080000,
		MODE_3D_INVERSEROLLOFF = 0x00100000,
		MODE_3D_LINEARROLLOFF = 0x00200000,
		MODE_3D_LINEARSQUAREROLLOFF = 0x00400000,
		MODE_3D_INVERSETAPEREDROLLOFF = 0x00800000,
		MODE_3D_CUSTOMROLLOFF = 0x04000000,
		MODE_3D_IGNOREGEOMETRY = 0x40000000,
		MODE_IGNORETAGS = 0x02000000,
		MODE_LOWMEM = 0x08000000,
		MODE_VIRTUAL_PLAYFROMSTART = 0x80000000
	};
	enum class SOUND_FORMAT
	{
		NONE,
		PCM8,
		PCM16,
		PCM24,
		PCM32,
		PCMFLOAT,
		BITSTREAM,
		MAX
	};
	enum class CHANNELORDER : int
	{
		DEFAULT,
		WAVEFORMAT,
		PROTOOLS,
		ALLMONO,
		ALLSTEREO,
		ALSA,
		MAX,
	};
	enum class SOUND_TYPE
	{
		UNKNOWN,
		AIFF,
		ASF,
		DLS,
		FLAC,
		FSB,
		IT,
		MIDI,
		MOD,
		MPEG,
		OGGVORBIS,
		PLAYLIST,
		RAW,
		S3M,
		USER,
		WAV,
		XM,
		XMA,
		AUDIOQUEUE,
		AT9,
		VORBIS,
		MEDIA_FOUNDATION,
		MEDIACODEC,
		FADPCM,
		OPUS,
		MAX,
	};
	enum class TIMEUNIT : std::uint32_t
	{
		MS = 0x00000001,
		PCM = 0x00000002,
		PCMBYTES = 0x00000004,
		RAWBYTES = 0x00000008,
		PCMFRACTION = 0x00000010,
		MODORDER = 0x00000100,
		MODROW = 0x00000200,
		MODPATTERN = 0x00000400,
	};

	struct CREATESOUNDEXINFO
	{
		int                      cbsize;
		std::uint32_t            length;
		std::uint32_t            fileoffset;
		int                      numchannels;
		int                      defaultfrequency;
		SOUND_FORMAT             format;
		std::uint32_t            decodebuffersize;
		int                      initialsubsound;
		int                      numsubsounds;
		std::intptr_t            inclusionlist;
		int                      inclusionlistnum;
		void*                    pcmreadcallback;
		void*                    pcmsetposcallback;
		void*                    nonblockcallback;
		std::intptr_t            dlsname;
		std::intptr_t            encryptionkey;
		int                      maxpolyphony;
		std::intptr_t            userdata;
		SOUND_TYPE               suggestedsoundtype;
		void*                    fileuseropen;
		void*                    fileuserclose;
		void*                    fileuserread;
		void*                    fileuserseek;
		void*                    fileuserasyncread;
		void*                    fileuserasynccancel;
		std::intptr_t            fileuserdata;
		int                      filebuffersize;
		CHANNELORDER             channelorder;
		std::intptr_t            initialsoundgroup;
		std::uint32_t            initialseekposition;
		TIMEUNIT                 initialseekpostype;
		int                      ignoresetfilesystem;
		std::uint32_t            audioqueuepolicy;
		std::uint32_t            minmidigranularity;
		int                      nonblockthreadid;
		std::intptr_t            fsbguid;
	};

	enum class ChannelControlType {
		Channel,
		ChannelGroup
	};
	enum class ChannelControlCallbackType {
		End,
		VirtualVoice,
		SyncPoint,
		Occlusion
	};

	template<class tag>
	struct tagged_void {};

	using System = tagged_void<struct system_tag>;
	using Bank = tagged_void<struct bank_tag>;
	using Sound = tagged_void<struct sound_tag>;
	using Channel = tagged_void<struct channel_tag>;
	using ChannelGroup = tagged_void<struct channel_group_tag>;
	using ChannelControl = tagged_void<struct channel_control_tag>; // Either a Channel or a ChannelGroup

	using CreateSound = FMOD_RESULT(System*, const char*, FMOD_MODE, CREATESOUNDEXINFO*, Sound**);
	using ReleaseSound = FMOD_RESULT(Sound*);
	using PlaySound = FMOD_RESULT(System*, Sound*, ChannelGroup*, bool, Channel**);

	using ChannelControlCallback = FMOD_RESULT(ChannelControl*, ChannelControlType, ChannelControlCallbackType, void*, void*);

	using ChannelIsPlaying = FMOD_RESULT(Channel*, bool*);
	using ChannelStop = FMOD_RESULT(Channel*);
	using ChannelSetPaused = FMOD_RESULT(Channel*, bool);
	using ChannelSetMute = FMOD_RESULT(Channel*, bool);
	using ChannelSetPitch = FMOD_RESULT(Channel*, float);
	using ChannelSetPan = FMOD_RESULT(Channel*, float);
	using ChannelSetVolume = FMOD_RESULT(Channel*, float);
	using ChannelSetFrequency = FMOD_RESULT(Channel*, float);
	using ChannelSetMode = FMOD_RESULT(Channel*, FMOD_MODE);
	using ChannelSetCallback = FMOD_RESULT(Channel*, ChannelControlCallback*);
	using ChannelSetUserData = FMOD_RESULT(Channel*, void*);
	using ChannelGetUserData = FMOD_RESULT(Channel*, void**);
	using ChannelGetChannelGroup = FMOD_RESULT(Channel*, ChannelGroup**);
}

namespace FMODStudio {
	using namespace FMOD;

	using System = tagged_void<struct system_tag>;
	using Bus = tagged_void<struct bus_tag>;

	using EventDescription = tagged_void<struct event_description_tag>;
	using EventInstance = tagged_void<struct event_instance_tag>;

	using GetCoreSystem = FMOD_RESULT(System*, FMOD::System**);
	using FlushCommands = FMOD_RESULT(System*);
	using GetBus = FMOD_RESULT(System*, const char*, Bus**);
	using LockChannelGroup = FMOD_RESULT(Bus*);
	using GetChannelGroup = FMOD_RESULT(Bus*, ChannelGroup**);

	using EventDescriptionCreateInstance = FMOD_RESULT(EventDescription* event, EventInstance** isntance);
}
