
//----------------------------------------------------------
//
// Copyright (C) GDR Games 2022-2023
//
// This source code is available for distribution and/or
// modification under the terms of either the Apache License
// v2.0 as published by the Apache Software Foundation, or
// the GNU General Public License v2.0 as published by the
// Free Software Foundation.
//
// This source is distributed in the hope that it will be useful,
// but WITHOUT ANY WARRANTY. If you are using this code for personal,
// non-commercial/monetary gain, you may use either of the
// licenses permitted, otherwise, you must use the GNU GPL v2.0.
//
// DESCRIPTION: src/g_sound.cpp
//  plays sounds
//----------------------------------------------------------
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

//#pragma pack(push, 1)
typedef struct nomadsnd_s
{
	//nomadbool_t bools[NUMSNDBOOLS];
	ALuint buffer;
	ALuint source;
//	std::string fname;
	struct nomadsnd_s* next;
	struct nomadsnd_s* prev;
	
	nomadbool_t has_played = false;
    nomadbool_t has_source = false;
    nomadbool_t has_buffer = false;
    nomadbool_t buffer_bound = false;
} nomadsnd_t;
//#pragma pack(pop)

class Sound
{
public:
	ALCdevice* device;
	ALCcontext* context;
	ALenum error;

	nomadbool_t done = false;
	std::unique_ptr<boost::thread> snd_thread;
	
	nomadsnd_t* snd_list;
	nomadsnd_t* music;
	nomadshort_t buffer[4096];
	boost::mutex lock;
	char fname[180]; // queued music track
	char musicfile[180]; // the currently playing music file
public:
	void S_FreeSound(nomadsnd_t* ptr);
	void S_AllocMusic(const char* name);
	void S_AllocSFX(const char* name);
};


#define alCall(x)\
	x; if ((snd->error = alGetError()) != AL_NO_ERROR) N_Error("[OpenAL Error] %s, %u: %s",__func__,__LINE__,alStrError())
static Sound* snd;
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

void Sound::S_AllocMusic(const char* name)
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
    music = (nomadsnd_t *)Z_Malloc(sizeof(nomadsnd_t), TAG_STATIC, &music);
    if (!music) {
        N_Error("S_AllocMusic: memory allocation failed");
    }
    music->prev = NULL;
    music->next = snd_list;
    snd_list->prev = music;
	SF_INFO fdata;
	SNDFILE* sf = sf_open(path, SFM_READ, &fdata);
    Z_ChangeTag(path, TAG_PURGELEVEL);
	if (!sf) {
		LOG_WARN("Sound::S_AllocMusic failed to create read-only audio file stream for %s with sf_open, aborting", path);
        Z_ChangeTag(music, TAG_PURGELEVEL);
		return;
	}
	nomadsize_t read;
	std::vector<nomadshort_t> rdbuf;
	while ((read = sf_read_short(sf, buffer, 4096 * sizeof(nomadshort_t))) != 0) {
		rdbuf.insert(rdbuf.end(), buffer, buffer + read);
	}
    alGenSources(1, &music->source);
    alGenBuffers(1, &music->buffer);
	alBufferData(music->buffer, fdata.channels == 1 ? AL_FORMAT_MONO16 : AL_FORMAT_STEREO16,
		&rdbuf.front(), rdbuf.size() * sizeof(nomadshort_t), fdata.samplerate);
	alSourcei(music->source, AL_BUFFER, music->buffer);
	alSourcei(music->source, AL_LOOPING, AL_TRUE);
	alSourcef(music->source, AL_GAIN, scf::music_vol);
    music->has_played = false;
    music->has_buffer = true;
    music->has_source = true;
    music->buffer_bound = true;
}

void Sound::S_AllocSFX(const char* name)
{
	nomadsnd_t* end;
	if (!snd_list->has_source && !snd_list->has_buffer && !snd_list->buffer_bound) {
		end = snd_list;
	}
	else {
		for (nomadsnd_t *it = snd_list;; it = it->next) {
			if (it->next == NULL) {
				it->next = (nomadsnd_t *)Z_Malloc(sizeof(nomadsnd_t), TAG_STATIC, &it->next);
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
    strcat(path, name);
	
	SF_INFO fdata;
	SNDFILE* sf = sf_open(path, SFM_READ, &fdata);
    Z_ChangeTag(path, TAG_PURGELEVEL);
	if (!sf) {
		LOG_WARN("Sound::S_AllocSFX failed to open audio file stream for %s with sf_open, aborting", path);
		if (end != snd_list) {
			Z_Free(end);
		}
		return;
	}
	
	nomadsize_t read;
	std::vector<nomadshort_t> rdbuf;
	while ((read = sf_read_short(sf, buffer, 4096 * sizeof(nomadshort_t))) != 0) {
		rdbuf.insert(rdbuf.end(), buffer, buffer + read);
	}
    alGenSources(1, &end->source);
    alGenBuffers(1, &end->buffer);
	alBufferData(end->buffer, fdata.channels == 1 ? AL_FORMAT_MONO16 : AL_FORMAT_STEREO16,
		&rdbuf.front(), rdbuf.size() * sizeof(nomadshort_t), fdata.samplerate);
	alSourcei(end->source, AL_BUFFER, end->buffer);
	alSourcef(end->source, AL_GAIN, scf::sfx_vol);
    end->has_played = false;
    end->buffer_bound = true;
    end->has_buffer = true;
    end->has_source = true;

	// link the structs
	if (end != snd_list) {
		end->prev = end;
		end->next = NULL;
	}
}

void Snd_AdjustVolumeAll(nomadfloat_t volume)
{
	for (nomadsnd_t* it = snd->snd_list; it != NULL; it = it->next) {
		alSourcef(it->source, AL_GAIN, volume);
	}
	alSourcef(snd->music->source, AL_GAIN, volume);
}

void Snd_AdjustVolumeSFX(nomadfloat_t volume)
{
	for (nomadsnd_t* it = snd->snd_list; it != NULL; it = it->next) {
		alSourcef(it->source, AL_GAIN, volume);
	}
}

void Snd_AdjustVolumeMusic(nomadfloat_t volume)
{
	alSourcef(snd->music->source, AL_GAIN, volume);
}

void Snd_LowerMusic()
{
	nomadfloat_t volume;
	alGetSourcef(snd->music->source, AL_GAIN, &volume);
	if (volume > scf::music_vol / 2) {
		volume = scf::music_vol / 2;
		alSourcef(snd->music->source, AL_GAIN, volume);
	}
}
void Snd_RaiseMusic()
{
	nomadfloat_t volume;
	alGetSourcef(snd->music->source, AL_GAIN, &volume);
	if (volume < scf::music_vol) {
		volume = scf::music_vol;
		alSourcef(snd->music->source, AL_GAIN, volume);
	}
}

void P_PlaySFX(const char* name)
{
	if (!(*sfx_on))
		return;
	
	snd->S_AllocSFX(name);
}

void Sound::S_FreeSound(nomadsnd_t* ptr)
{
    if (!ptr) {
        return;
    }
	if (ptr->prev == NULL && ptr->next == NULL && ptr != snd_list) {
		N_Error("S_FreeSound: ptr has improper linkage");
	}
	if (ptr == snd_list) {
		ptr->has_played = false;
        alSourcei(ptr->source, AL_BUFFER, 0);
        alDeleteBuffers(1, &ptr->buffer);
        alDeleteSources(1, &ptr->source);
        ptr->has_buffer = false;
        ptr->has_source = false;
        ptr->buffer_bound = false;
		// dont free if its the beginning of the list
	}
	else if (ptr != snd_list && ptr != music) {
		if (!ptr->prev) {
			LOG_WARN("Sound::S_FreeSound: ptr->prev == NULL, ptr != snd->snd_list && ptr != snd->music, improper linkage");
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
		Z_Free(ptr);
	}
	else if (ptr == music) {
		if (!ptr->next) {
			LOG_WARN("Sound::S_FreeSound: ptr->next == NULL, ptr == snd->music, improper linkage");
			N_Error("S_FreeSound: ptr->next has improper linkage");
		}
		ptr->has_played = false;
		ptr->has_source = ptr->has_buffer = ptr->buffer_bound = false;
		alSourcei(ptr->source, AL_BUFFER, 0);
        alDeleteBuffers(1, &ptr->buffer);
        alDeleteSources(1, &ptr->source);
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
	    Z_Free(it);
	}
	alcMakeContextCurrent(NULL);
	alcDestroyContext(snd->context);
	alcCloseDevice(snd->device);
}

void S_PlayMusic(const char* name)
{
    if (!(*music_on))
        return;
    
    snd->S_AllocMusic(name);
}


inline void S_RunMusic()
{
	if (!(*music_on))
		return;

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
	        snd->S_FreeSound(music);
			S_PlayMusic(std::string("MUS0"+std::to_string(P_Random() & 5)).c_str());
	    }
	}
}

void G_RunSound()
{
	boost::unique_lock<boost::mutex> lock{snd->lock};
	S_RunMusic();
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
	        snd->S_FreeSound(it);
	    }
	    else {
	        continue;
	    }
	}
}

static void S_PreAllocate()
{
	snd->snd_list = (nomadsnd_t *)Z_Malloc(sizeof(nomadsnd_t), TAG_STATIC, &snd->snd_list);
    if (!snd->snd_list) {
        N_Error("S_PreAllocate: memory allocation failed");
    }
	memset(snd->buffer, 0, sizeof(snd->buffer));
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
	snd = (Sound *)Z_Malloc(sizeof(Sound), TAG_STATIC, &snd);
    if (!snd) {
		fprintf(stderr, "Snd_Init: failed memory allocation for sound, aborting.\n");
		Z_Free(snd);
		return;
    }
	snd->device = alcOpenDevice(NULL);
	if (!snd->device) {
		fprintf(stderr, "alcOpenDevice: failed to initialize device, OpenALC Error: %s\n",
			alcGetString(snd->device, alcGetError(snd->device)));
		Z_Free(snd);
		return;
	}
	snd->context = alcCreateContext(snd->device, NULL);
	if (!snd->context) {
		fprintf(stderr, "alcCreateContext: failed to initialize context, OpenALC Error: %s\n",
			alcGetString(snd->device, alcGetError(snd->device)));
		alcCloseDevice(snd->device);
		Z_Free(snd);
		return;
	}
	alcMakeContextCurrent(snd->context);
	ALCenum error;
	if ((error = alcGetError(snd->device)) != ALC_NO_ERROR) {
		fprintf(stderr, "alcMakeContextCurrent: failed to initialize snd->context, OpenALC Error: %s\n",
			alcGetString(snd->device, alcGetError(snd->device)));
		alcDestroyContext(snd->context);
		alcCloseDevice(snd->device);
		Z_Free(snd);
		return;
	}
	*sfx_on = true;
	*music_on = true;
	
	fprintf(stdout, "Successfully intialized OpenAL\n");
	
	// pre-allocate all the buffers and sources
	S_PreAllocate();
	atexit(Snd_Kill);
	N_memset(snd->musicfile, 0, sizeof(snd->musicfile));
	S_PlayMusic("MUS11.ogg");
}

static nomadlong_t ticker =  0;

static inline const char* Snd_GetNum(nomadenum_t intensity)
{
	static char buffer[2]={0};
	switch (intensity) {
	case 0: snprintf(buffer, sizeof(buffer), "%d", P_Random() & 1); break;
	case 1: snprintf(buffer, sizeof(buffer), "%d", P_Random() & 2); break;
	case 2: snprintf(buffer, sizeof(buffer), "%d", P_Random() & 3); break;
	default:
		N_Error("Snd_GetNum: variable intensity wasn't valid: %hu", intensity);
		break;
	};
	return buffer;
}

void P_AdaptiveSound()
{
	if (ticker) {
		--ticker;
		return;
	}
	else
		ticker = ticrate_base*2; // re-run every 2 seconds

	N_memset(snd->fname, 0, sizeof(snd->fname));
	N_strcpy(snd->fname, "MUS");
	nomadint_t nummobs_far = 0;
	nomadint_t nummobs_med = 0;
	nomadint_t nummobs_short = 0;
	for (linked_list<Mob*>::iterator it = game->m_Active.begin(); it != NULL; it = it->next) {
		if (disBetweenOBJ(it->val->mpos, playr->pos) < 16
		&& disBetweenOBJ(it->val->mpos, playr->pos) > 10) {
			++nummobs_far;
		}
		else if (disBetweenOBJ(it->val->mpos, playr->pos) < 11
		&& disBetweenOBJ(it->val->mpos, playr->pos) > 5) {
			++nummobs_med;
		}
		else if (disBetweenOBJ(it->val->mpos, playr->pos) < 6) {
			++nummobs_short;
		}
	}
	nomadenum_t intensity = 0;
	if (nummobs_far >= nummobs_med || nummobs_med >= nummobs_short) {
		strcat(snd->fname, "1");
		intensity = 1;
	}
	else if (nummobs_short >= nummobs_far && nummobs_short >= nummobs_med) {
		strcat(snd->fname, "2");
		intensity = 2;
	}
	else {
		strcat(snd->fname, "0");
		intensity = 0;
	}
	strcat(snd->fname, Snd_GetNum(intensity));
	
	// dont re-allocate if its the same file, only redo allocations if necessary
	if (!N_strcmp(snd->fname, snd->musicfile)) {
		N_strcpy(snd->musicfile, snd->fname);
		S_PlayMusic(snd->fname);
	}
}