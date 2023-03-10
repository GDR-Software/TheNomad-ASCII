#include "n_shared.h"
#include "scf.h"
#include "g_zone.h"
#include "g_items.h"
#include "g_obj.h"
#include "g_mob.h"
#include "p_npc.h"
#include "g_map.h"
#include "s_scripted.h"
#include "s_world.h"
#include "g_playr.h"
#include "g_game.h"
#include "g_animation.h"
#include "g_rng.h"
#include "g_sound.h"

enum
{
	SND_BOOL_PLAYED,
	SND_BOOL_SOURCE_ALLOCED,
	SND_BOOL_BUFFER_ALLOCED,
	SND_BOOL_BUFFER_BOUND,
	
	NUMSNDBOOLS
};

#pragma pack(push, 1)
typedef struct nomadsnd_s
{
	nomadbool_t bools[NUMSNDBOOLS];
	ALuint buffer;
	ALuint source;
	std::vector<nomadshort_t> rdbuf;
	std::string fname;
	struct nomadsnd_s* next;
	struct nomadsnd_s* prev;
	
	nomadbool_t has_played = false;
    nomadbool_t has_source = false;
    nomadbool_t has_buffer = false;
    nomadbool_t buffer_bound = false;
} nomadsnd_t;
#pragma pack(pop)

#pragma pack(push, 1)
typedef struct sound_s
{
	ALCdevice* device;
	ALCcontext* context;
	ALenum error;
	
	nomadsnd_t* snd_list;
	nomadsnd_t* music;
} sound_t;
#pragma pack(pop)

#define alCall(x)\
	x; if ((snd->error = alGetError()) != AL_NO_ERROR) N_Error("[OpenAL Error] %s, %u: %s",__func__,__LINE__,alStrError())
static sound_t* snd;
static nomadbool_t* sfx_on = &scf::sfx_on;
static nomadbool_t* music_on = &scf::music_on;
static nomadbool_t snd_on;
static Game* game;
static Playr* playr;

static const char* alStrError()
{
	switch (snd->error) {
	case AL_NO_ERROR: return "No Error";
	case AL_INVALID_NAME: return "Bad ID was passed to function";
	case AL_INVALID_ENUM: return "Invalid enum value was passed to function";
	case AL_INVALID_VALUE: return "Invalid value was passed to function";
	case AL_INVALID_OPERATION: return "Invalid operation requested";
	case AL_OUT_OF_MEMORY: return "Memory failure occured. Out of memory";
	};
	return (const char *)NULL;
}

#define SND_TYPE_MUSIC 0
#define SND_TYPE_SFX   1

inline void S_FreeBuffer(nomadsnd_t* const ptr);
inline void S_FreeSource(nomadsnd_t* const ptr);
inline void S_AllocBuffer(nomadsnd_t* const ptr);
inline void S_AllocSource(nomadsnd_t* const ptr);
inline void S_BindBuffer(nomadsnd_t* const ptr);
inline void S_UnbindBuffer(nomadsnd_t* const ptr);

void S_AllocMusic(const char* name)
{
	if (!(*music_on)) {
		LOG_INFO("scf::music_on == false, ignoring call");
		return;
	}

    char *path = (char *)Z_Malloc(sizeof(char) * 528, TAG_SCOPE, &path);
    memset(path, 0, 528);
    strcpy(path, "Files/gamedata/BFF/");
    strcat(path, game->bffname);
    strcat(path, "/MUSIC/");
    strcat(path, name);
    snd->music = new nomadsnd_t;
    if (!snd->music) {
        N_Error("S_AllocMusic: memory allocation failed");
    }
    nomadsnd_t* music = snd->music;
    music->prev = NULL;
    music->next = snd->snd_list;
    snd->snd_list->prev = music;
	SF_INFO fdata;
	SNDFILE* sf = sf_open(path, SFM_READ, &fdata);
    Z_ChangeTag(path, TAG_PURGELEVEL);
	if (!sf) {
		LOG_WARN("sf_open: failed to create read-only audio file stream for %s, aborting", path);
        Z_Free(snd->music);
		return;
	}
    nomadshort_t *buffer = new nomadshort_t[sizeof(nomadshort_t) * 4096];
    if (!buffer) {
        N_Error("S_AllocMusic: memory allocation failed");
    }
    memset(buffer, 0, sizeof(nomadshort_t) * 4096);
	nomadsize_t read;
	while ((read = sf_read_short(sf, buffer, 4096 * sizeof(nomadshort_t))) != 0) {
		snd->music->rdbuf.insert(snd->music->rdbuf.end(), buffer, buffer + read);
	}
    delete[] buffer;
    alGenSources(1, &music->source);
    alGenBuffers(1, &music->buffer);
	alBufferData(snd->music->buffer, fdata.channels == 1 ? AL_FORMAT_MONO16 : AL_FORMAT_STEREO16,
		&snd->music->rdbuf.front(), snd->music->rdbuf.size() * sizeof(nomadshort_t), fdata.samplerate);
	alSourcei(music->source, AL_BUFFER, music->buffer);
    snd->music->has_played = false;
    music->has_buffer = true;
    music->has_source = true;
    music->buffer_bound = true;
}

void S_AllocSFX(const std::string& name)
{
	nomadsnd_t* end;
	if (snd->snd_list->rdbuf.size() < 1) {
		end = snd->snd_list;
	}
	else {
		for (nomadsnd_t *it = snd->snd_list;; it = it->next) {
			if (it->next == NULL) {
				it->next = new nomadsnd_t;
                if (!it->next) {
                    N_Error("S_AllocSFX: memory allocation failed");
                }
				it->next->has_source = false;
				it->next->has_buffer = false;
				it->next->buffer_bound = false;
				it->next->has_played = false;
                end = it->next;
				break;
			}
		}
	}
	
	// make the path
    char *path = (char *)Z_Malloc(sizeof(char) * 528, TAG_SCOPE, &path);
    memset(path, 0, 528);
    strcpy(path, "Files/gamedata/BFF/");
    strcat(path, game->bffname);
    strcat(path, "/SFX/");
    strcat(path, name.c_str());
	
	SF_INFO fdata;
	SNDFILE* sf = sf_open(path, SFM_READ, &fdata);
    Z_ChangeTag(path, TAG_PURGELEVEL);
	if (!sf) {
		LOG_WARN("sf_open: failed to open audio file stream for %s, aborting", path);
		Z_Free(end->next);
		return;
	}
	
	nomadsize_t read;
    nomadshort_t* buffer = new nomadshort_t[sizeof(nomadshort_t) * 4096];
    if (!buffer) {
        N_Error("S_AllocSFX: malloc failed");
    }
    memset(buffer, 0, sizeof(nomadshort_t) * 4096);
	while ((read = sf_read_short(sf, buffer, 4096 * sizeof(nomadshort_t))) != 0) {
		end->rdbuf.insert(end->rdbuf.end(), buffer, buffer + read);
	}
    delete[] buffer;
	
    alGenSources(1, &end->source);
    alGenBuffers(1, &end->buffer);
	alBufferData(end->buffer, fdata.channels == 1 ? AL_FORMAT_MONO16 : AL_FORMAT_STEREO16,
		&end->rdbuf.front(), end->rdbuf.size() * sizeof(nomadshort_t), fdata.samplerate);
	alSourcei(end->source, AL_BUFFER, end->buffer);
    end->has_played = false;
    end->buffer_bound = true;
    end->has_buffer = true;
    end->has_source = true;

	// link the structs
	end->prev = end;
	end->next = NULL;
}

void P_PlaySFX(const std::string& name)
{
	if (!(*sfx_on))
		return;
	
	S_AllocSFX(name);
}

void S_FreeSound(nomadsnd_t *ptr)
{
    if (!ptr) {
        return;
    }
	if (ptr->prev == NULL && ptr->next == NULL && ptr != snd->snd_list) {
		N_Error("S_FreeSound: ptr has improper linkage");
	}
	if (ptr == snd->snd_list) {
		ptr->has_played = false;
        alSourcei(ptr->source, AL_BUFFER, 0);
        alDeleteBuffers(1, &ptr->buffer);
        alDeleteSources(1, &ptr->source);
        ptr->has_buffer = false;
        ptr->has_source = false;
        ptr->buffer_bound = false;
	}
	else if (ptr != snd->snd_list && ptr != snd->music) {
		if (!ptr->prev) {
			LOG_WARN("ptr->prev == NULL, ptr != snd->snd_list && ptr != snd->music, improper linkage");
			N_Error("S_FreeSound: ptr->prev has improper linkage");
		}
        alSourcei(ptr->source, AL_BUFFER, 0);
        alDeleteBuffers(1, &ptr->buffer);
        alDeleteSources(1, &ptr->source);
		
		// unlink, link, then free
		ptr->prev->next = ptr->next;
        if (ptr->next) {
            ptr->next->prev = ptr->prev;
        }
        delete ptr;
	}
	else if (ptr == snd->music) {
		if (!ptr->next) {
			LOG_WARN("ptr->next == NULL, ptr == snd->music, improper linkage");
			N_Error("S_FreeSound: ptr->next has improper linkage");
		}
		ptr->has_played = false;
		alSourcei(ptr->source, AL_BUFFER, 0);
        alDeleteBuffers(1, &ptr->buffer);
        alDeleteSources(1, &ptr->source);
        delete ptr;
	}
}

void Snd_Kill()
{
	ALint state;
	for (nomadsnd_t* it = snd->snd_list->prev; it != NULL; it = it->next) {
		if (it->has_source) {
			alGetSourcei(it->source, AL_SOURCE_STATE, &state);
			if (state == AL_PLAYING && it->has_played) {
				alSourceStop(it->source);
			}
            if (it->has_buffer) {
                if (it->buffer_bound) {
                    alSourcei(it->source, AL_BUFFER, 0);
                }
                alDeleteBuffers(1, &it->buffer);
            }
            alDeleteSources(1, &it->source);
		}
        delete it;
	}
	alcMakeContextCurrent(NULL);
	alcDestroyContext(snd->context);
	alcCloseDevice(snd->device);
    delete snd;
}

void S_PlayMusic(const char* name)
{
    if (!(*music_on))
        return;
    
    S_AllocMusic(name);
}

void G_RunSound()
{
	// deal with the music data first
	if (*music_on && snd->music->has_buffer && snd->music->has_source) {
		nomadsnd_t* music = snd->music;
		if (!music) {
			N_Error("G_RunSound: snd->music is NULL");
		}
        ALint state;
        alGetSourcei(music->source, AL_SOURCE_STATE, &state);
        if (state != AL_PLAYING && !music->has_played) {
            music->has_played = true;
            alSourcePlay(music->source);
        }
        else if (state != AL_PLAYING && music->has_played) {
            S_FreeSound(music);
        }
	}
	if (!(*sfx_on))
		return;
	
	for (nomadsnd_t* it = snd->snd_list; it != NULL; it = it->next) {
		ALint state;
        alGetSourcei(it->source, AL_SOURCE_STATE, &state);
        if (state != AL_PLAYING && !it->has_played) {
            it->has_played = true;
            alSourcePlay(it->source);
        }
        else if (state != AL_PLAYING && it->has_played) {
            S_FreeSound(it);
        }
        else {
            continue;
        }
	}
}

static void S_PreAllocate()
{
	snd->snd_list = new nomadsnd_t;
    if (!snd->snd_list) {
        N_Error("S_PreAllocate: memory allocation failed");
    }
	
	nomadsnd_t* sfx = snd->snd_list;
	sfx->next = NULL;
	snd->music = sfx->prev;
	sfx->has_source = false;
	sfx->has_buffer = false;
	sfx->buffer_bound = false;
	sfx->has_played = false;
}

void Snd_Init(Game* const gptr)
{
    game = gptr;
    playr = game->playr;
	*sfx_on = false;
	*music_on = false;
	snd = new sound_t;
    if (!snd) {
        N_Error("Snd_Init: memory allocation failed");
    }
	snd->device = alcOpenDevice(NULL);
	if (!snd->device) {
		fprintf(stderr, "alcOpenDevice: failed to initialize device, OpenALC Error: %s\n",
			alcGetString(snd->device, alcGetError(snd->device)));
		delete snd;
		return;
	}
	snd->context = alcCreateContext(snd->device, NULL);
	if (!snd->context) {
		fprintf(stderr, "alcCreateContext: failed to initialize context, OpenALC Error: %s\n",
			alcGetString(snd->device, alcGetError(snd->device)));
		alcCloseDevice(snd->device);
		delete snd;
		return;
	}
	alcMakeContextCurrent(snd->context);
	ALCenum error;
	if ((error = alcGetError(snd->device)) != ALC_NO_ERROR) {
		fprintf(stderr, "alcMakeContextCurrent: failed to initialize snd->context, OpenALC Error: %s\n",
			alcGetString(snd->device, alcGetError(snd->device)));
		alcDestroyContext(snd->context);
		alcCloseDevice(snd->device);
		delete snd;
		return;
	}
	*sfx_on = true;
	*music_on = true;
	
	fprintf(stdout, "Successfully intialized OpenAL\n");
	
	// pre-allocate all the buffers and sources
	S_PreAllocate();
	atexit(Snd_Kill);
    S_PlayMusic("odst.wav");
}

void P_AdaptiveSound()
{
}