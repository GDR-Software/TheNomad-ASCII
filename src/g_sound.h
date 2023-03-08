#ifndef _G_SOUND_
#define _G_SOUND_

#pragma once

typedef enum
{
	sfx_adb_shot,
	NUMSFX,
	
	sfx_null
} sfxnum_t;

typedef enum
{
	mus_odst,
	NUMMUSIC,
	
	mus_null
} musicnum_t;

typedef struct nomadmus_s
{
	ALuint source;
	ALuint buffer;
    SF_INFO fdata;
	musicnum_t num;
	nomadbool_t played : 1;
	nomadbool_t queued : 1;
    nomadshort_t *buf;
    nomadsize_t bufsize = 0;
} nomadmus_t;

typedef struct nomadsfx_s
{
	ALuint source;
	ALuint buffer;
    SF_INFO fdata;
	sfxnum_t num;
	nomadbool_t played : 1;
	nomadbool_t queued : 1;
	std::vector<nomadshort_t> buf;
} nomadsfx_t;

typedef struct nomadsnd_s
{
	const char* str;
	nomaduint_t num;
} nomadsnd_t;

#if 0
void P_PlaySFX(const std::string& name);
#endif
void Snd_Kill();
inline void P_PlaySFX(const std::string& name){}
void M_PlaySFX(const std::string& name, Mob* const actor);
void G_RunSound();


#endif