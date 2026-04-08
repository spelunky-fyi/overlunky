#pragma once

#define FMOD_CHECK_CALL(x)                          \
    [](auto err) {                                  \
        if (err != FMOD::FMOD_RESULT::OK)           \
        {                                           \
            DEBUG("{}: {}", #x, FMOD::ErrStr(err)); \
            return false;                           \
        }                                           \
        return true;                                \
    }((x))

namespace FMOD
{
enum class FMOD_RESULT
{
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
#define ERR_CASE(err_enum)      \
    case FMOD_RESULT::err_enum: \
        return #err_enum

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

enum class FMOD_MODE : std::uint32_t
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
    int cbsize;
    std::uint32_t length;
    std::uint32_t fileoffset;
    int numchannels;
    int defaultfrequency;
    SOUND_FORMAT format;
    std::uint32_t decodebuffersize;
    int initialsubsound;
    int numsubsounds;
    std::intptr_t inclusionlist;
    int inclusionlistnum;
    void* pcmreadcallback;
    void* pcmsetposcallback;
    void* nonblockcallback;
    std::intptr_t dlsname;
    std::intptr_t encryptionkey;
    int maxpolyphony;
    std::intptr_t userdata;
    SOUND_TYPE suggestedsoundtype;
    void* fileuseropen;
    void* fileuserclose;
    void* fileuserread;
    void* fileuserseek;
    void* fileuserasyncread;
    void* fileuserasynccancel;
    std::intptr_t fileuserdata;
    int filebuffersize;
    CHANNELORDER channelorder;
    std::intptr_t initialsoundgroup;
    std::uint32_t initialseekposition;
    TIMEUNIT initialseekpostype;
    int ignoresetfilesystem;
    std::uint32_t audioqueuepolicy;
    std::uint32_t minmidigranularity;
    int nonblockthreadid;
    std::intptr_t fsbguid;
};

struct FMOD_GUID
{
    unsigned int Data1;
    unsigned short Data2;
    unsigned short Data3;
    unsigned char Data4[8];
};

enum class ChannelControlType
{
    Channel,
    ChannelGroup
};
enum class ChannelControlCallbackType
{
    End,
    VirtualVoice,
    SyncPoint,
    Occlusion
};

using BOOL = int;

template <class tag>
struct tagged_void
{
};

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

using ChannelIsPlaying = FMOD_RESULT(Channel*, BOOL*);
using ChannelStop = FMOD_RESULT(Channel*);
using ChannelSetPaused = FMOD_RESULT(Channel*, BOOL);
using ChannelSetMute = FMOD_RESULT(Channel*, BOOL);
using ChannelSetPitch = FMOD_RESULT(Channel*, float);
using ChannelSetPan = FMOD_RESULT(Channel*, float);
using ChannelSetVolume = FMOD_RESULT(Channel*, float);
using ChannelSetFrequency = FMOD_RESULT(Channel*, float);
using ChannelSetMode = FMOD_RESULT(Channel*, FMOD_MODE);
using ChannelSetCallback = FMOD_RESULT(Channel*, ChannelControlCallback*);
using ChannelSetUserData = FMOD_RESULT(Channel*, void*);
using ChannelGetUserData = FMOD_RESULT(Channel*, void**);
using ChannelGetChannelGroup = FMOD_RESULT(Channel*, ChannelGroup**);
} // namespace FMOD

namespace FMODStudio
{
using namespace FMOD;

enum StopMode
{
    AllowFadeOut,
    Immediate
};

enum EventCallbackType
{
    Created = 0x00000001,
    Destroyed = 0x00000002,
    Starting = 0x00000004,
    Started = 0x00000008,
    Restarted = 0x00000010,
    Stopped = 0x00000020,
    StartFailed = 0x00000040,
    CreateProgrammerSound = 0x00000080,
    DestroyProgrammerSound = 0x00000100,
    PluginCreated = 0x00000200,
    PluginDestroyed = 0x00000400,
    TimelineMarker = 0x00000800,
    TimelineBeat = 0x00001000,
    SoundPlayed = 0x00002000,
    SoundStopped = 0x00004000,
    RealToVirtal = 0x00008000,
    VirtualToReal = 0x00010000,
    All = 0xFFFFFFFF,
    Num = 17
};

enum class LoadBankFlags
{
    Normal = 0x00000000,
    Nonblocking = 0x00000001,
    DecompressSamples = 0x00000002,
    Unencrypted = 0x00000004,
};

enum class LoadingState
{
    Unloading,
    Unloaded,
    Loading,
    Loaded,
    Error
};

enum class PlaybackState
{
    Playing,
    Sustaining,
    Stopped,
    Starting,
    Stopping
};

enum class ParameterType
{
    GameControlled,
    AutomaticDistance,
    AutomaticEventConeAngle,
    AutomaticEventOrientation,
    AutomaticDirection,
    AutomaticElevation,
    AutomaticListenerOrientation,
    AutomaticSpeed,
};

enum class ParameterFlags
{
    ReadOnly = 0x00000001,
    Automatic = 0x00000002,
    Global = 0x00000004,
};

struct ParameterId
{
    uint32_t data1;
    uint32_t data2;
};
struct ParameterDescription
{
    const char* name;
    ParameterId id;
    float minimum;
    float maximum;
    float defaultvalue;
    ParameterType type;
    ParameterFlags flags;
    FMOD_GUID guid;
};

using System = tagged_void<struct system_tag>;
using Bus = tagged_void<struct bus_tag>;

using EventDescription = tagged_void<struct event_description_tag>;
using EventInstance = tagged_void<struct event_instance_tag>;

using GetCoreSystem = FMOD_RESULT(System*, FMOD::System**);
using FlushCommands = FMOD_RESULT(System*);
using GetBus = FMOD_RESULT(System*, const char*, Bus**);
using LockChannelGroup = FMOD_RESULT(Bus*);
using GetChannelGroup = FMOD_RESULT(Bus*, ChannelGroup**);

using SystemLoadBankFile = FMOD_RESULT(System*, const char*, LoadBankFlags, Bank**);
using SystemGetEventByID = FMOD_RESULT(System*, FMOD_GUID*, EventDescription**);

using ParseID = FMOD_RESULT(const char*, FMOD_GUID*);

using BankGetLoadingState = FMOD_RESULT(Bank*, LoadingState*);
using BankLoadSampleData = FMOD_RESULT(Bank*);
using BankUnloadSampleData = FMOD_RESULT(Bank*);
using BankGetSampleLoadingState = FMOD_RESULT(Bank*, LoadingState*);
using BankUnload = FMOD_RESULT(Bank*);
using BankIsValid = bool(Bank*);

using EventInstanceCallback = FMOD_RESULT(EventCallbackType, EventInstance*, void*);

using EventDescriptionCreateInstance = FMOD_RESULT(EventDescription*, EventInstance**);
using EventDescriptionReleaseAllInstances = FMOD_RESULT(EventDescription*);
using EventDescriptionLoadSampleData = FMOD_RESULT(EventDescription*);
using EventDescriptionUnloadSampleData = FMOD_RESULT(EventDescription*);
using EventDescriptionGetSampleLoadingState = FMOD_RESULT(EventDescription*, LoadingState*);
using EventDescriptionGetParameterDescriptionCount = FMOD_RESULT(EventDescription*, int*);
using EventDescriptionGetParameterDescriptionByName = FMOD_RESULT(EventDescription*, const char*, ParameterDescription*);
using EventDescriptionGetParameterDescriptionByIndex = FMOD_RESULT(EventDescription*, int, ParameterDescription*);
using EventDescriptionGetParameterDescriptionByID = FMOD_RESULT(EventDescription*, ParameterId, ParameterDescription*);
using EventDescriptionSetCallback = FMOD_RESULT(EventDescription*, EventInstanceCallback*, EventCallbackType);
using EventDescriptionIsValid = bool(EventDescription*);

using EventInstanceStart = FMOD_RESULT(EventInstance*);
using EventInstanceStop = FMOD_RESULT(EventInstance*, StopMode);
using EventInstanceGetPlaybackState = FMOD_RESULT(EventInstance*, PlaybackState*);
// using EventInstanceIsPlaying = FMOD_RESULT(EventInstance*, BOOL*);
using EventInstanceSetPaused = FMOD_RESULT(EventInstance*, BOOL);
using EventInstanceGetPaused = FMOD_RESULT(EventInstance*, BOOL*);
using EventInstanceKeyOff = FMOD_RESULT(EventInstance*);
// using EventInstanceSetMute = FMOD_RESULT(EventInstance*, BOOL);
using EventInstanceSetPitch = FMOD_RESULT(EventInstance*, float);
using EventInstanceGetPitch = FMOD_RESULT(EventInstance*, float*, float*);
// using EventInstanceSetPan = FMOD_RESULT(EventInstance*, float);
using EventInstanceSetTimelinePosition = FMOD_RESULT(EventInstance*, int);
using EventInstanceGetTimelinePosition = FMOD_RESULT(EventInstance*, int*);
using EventInstanceSetVolume = FMOD_RESULT(EventInstance*, float);
using EventInstanceGetVolume = FMOD_RESULT(EventInstance*, float*, float*);
// using EventInstanceSetFrequency = FMOD_RESULT(EventInstance*, float);
// using EventInstanceSetMode = FMOD_RESULT(EventInstance*, FMOD_MODE);
using EventInstanceSetCallback = FMOD_RESULT(EventInstance*, EventInstanceCallback*, EventCallbackType);
using EventInstanceSetUserData = FMOD_RESULT(EventInstance*, void*);
using EventInstanceGetUserData = FMOD_RESULT(EventInstance*, void**);
using EventInstanceGetDescription = FMOD_RESULT(EventInstance*, EventDescription**);
using EventInstanceSetParameterByName = FMOD_RESULT(EventInstance*, const char*, float, BOOL);
using EventInstanceGetParameterByName = FMOD_RESULT(EventInstance*, const char*, float*, float*);
using EventInstanceSetParameterByNameWithLabel = FMOD_RESULT(EventInstance*, const char*, const char*, BOOL);
using EventInstanceGetParameterByID = FMOD_RESULT(EventInstance*, ParameterId, float*, float*);
using EventInstanceSetParameterByID = FMOD_RESULT(EventInstance*, ParameterId, float, bool);
using EventInstanceSetParameterByIDWithLabel = FMOD_RESULT(EventInstance*, ParameterId, const char*, BOOL);
using EventInstanceRelease = FMOD_RESULT(EventInstance*);
using EventInstanceIsValid = bool(EventInstance*);
} // namespace FMODStudio
