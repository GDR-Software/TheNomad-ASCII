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

#define SNDFILE_WAV  0
#define SNDFILE_MIDI 1
#define SNDFILE_MP3  2
#define SFX_CHANNELS(sfx) ((sfx.fdata.channels)==1?AL_FORMAT_MONO16:AL_FORMAT_STEREO16)

static Game* game;
static Playr* playr;
static nomadmus_t *music;
static ALCdevice* device;
static ALCcontext* context;

void Snd_Kill()
{
	ALint state;
	alGetSourcei(music->source, AL_SOURCE_STATE, &state);
	if (state == AL_PLAYING && music->played) {
		alSourceStop(music->source);
        alSourcei(music->source, AL_BUFFER, 0);
		alDeleteBuffers(1, &music->buffer);
	}
	alDeleteSources(1, &music->source);
#if 0
	for (std::vector<nomadsfx_t>::iterator it = sound->sfx.begin(); it != sound->sfx.end(); ++it) {
		ALint state;
        std::vector<nomadsfx_t>::iterator i = it;
		alGetSourcei(i->source, AL_SOURCE_STATE, &state);
		if (state == AL_PLAYING && i->played) {
			alSourceStop(i->source);
            alSourcei(i->source, AL_BUFFER, 0);
			alDeleteBuffers(1, &i->buffer);
		}
		alDeleteSources(1, &i->source);
	}
#endif
	alcMakeContextCurrent(NULL);
	alcDestroyContext(context);
	alcCloseDevice(device);
}

#if 0
void P_PlaySFX(const std::string& name)
{
//	if (!scf::sfx_on)
//		return;
    nomadshort_t buffer[4096];
    sound->sfx.emplace_back();
    nomadsfx_t *snd = &sound->sfx.back();
    char *path = (char *)alloca(1024+name.size());
    memset(path, '\0', 1024+name.size());
    sprintf(path, "Files/gamedata/BFF/%s/%s", game->bffname, name.c_str());
    SNDFILE* sf = sf_open(path, SFM_READ, &snd->fdata);
    if (!sf) {
        N_Error("failed to open sfx file %s!", path);
    }
    nomadsize_t read;
    while ((read = sf_read_short(sf, sound->buffer, sizeof(sound->buffer))) != 0) {
        snd->buf.insert(snd->buf.end(), sound->buffer, sound->buffer + read);
    }
    sf_close(sf);
    alGenSources(1, &snd->source);
	alGenBuffers(1, &snd->buffer);
	alBufferData(snd->buffer, SFX_CHANNELS((*snd)),
		&snd->buf.front(), snd->buf.size() * sizeof(nomadshort_t), snd->fdata.samplerate);
    alSourcei(snd->source, AL_BUFFER, snd->buffer);
}
#endif

void S_PlayMusic(const char* name)
{
    if (!scf::music_on)
        return;
    
    ALint state;
    char *path = (char *)alloca(1024+strlen(name));
    memset(path, '\0', 1024+strlen(name));
    sprintf(path, "Files/gamedata/BFF/%s/MUSIC/%s", game->bffname, name);
    SNDFILE* sf = sf_open(path, SFM_READ, &music->fdata);
    if (!sf) {
        N_Error("failed to open music file %s!", path);
    }
    nomadsize_t read;
    nomadshort_t buffer[4096];
    nomaduint_t count = 0;
    std::array<nomadshort_t, 4096> buf;
    while ((read = sf_read_short(sf, buf.back(), 4096 * sizeof(nomadshort_t))) != 0) {
        buf.emplace_back();
    }
    sf_close(sf);
    alGenSources(1, &music->source);
    alGenBuffers(1, &music->buffer);
    alBufferData(music->buffer, SFX_CHANNELS((*music)),
        &music->buf, music->bufsize* sizeof(nomadshort_t), music->fdata.samplerate);
    alSourcei(music->source, AL_BUFFER, music->buffer);
}

void Snd_Init(Game* const gptr)
{
    device = alcOpenDevice(NULL);
	context = alcCreateContext(device, NULL);
	alcMakeContextCurrent(context);
	game = gptr;
	playr = game->playr;
    music = (nomadmus_t *)Z_Malloc(sizeof(nomadmus_t), TAG_STATIC, &music);
    scf::music_on = true;
    scf::sfx_on = true;
	//phealth = &playr->health;
	//parmor = &playr->armor;
	//m_Active = &game->m_Active;
	//gametics = &game->ticcount;
    S_PlayMusic("odst.wav");
}

void G_RunSound()
{
	ALint state;
#if 0
	for (std::vector<nomadsfx_t>::iterator it = sound->sfx.begin(); it != sound->sfx.end(); it++) {
		alGetSourcei(it->source, AL_SOURCE_STATE, &state);
		if (state != AL_PLAYING && !it->played) {
			it->played = true;
			alSourcePlay(it->source);
		}
		else if (state != AL_PLAYING && it->played) {
			it->played = false;
			alSourceStop(it->source);
			alSourcei(it->source, AL_BUFFER, 0);
            alDeleteBuffers(1, &it->buffer);
            alDeleteSources(1, &it->source);
            sound->sfx.erase(it);
		}
	}
#endif
	alGetSourcei(music->source, AL_SOURCE_STATE, &state);
	if (state != AL_PLAYING && !music->played) {
		music->played = true;
		alSourcePlay(music->source);
	}
	else if (state == AL_STOPPED && music->played) {
		alSourceStop(music->source);
		alSourcei(music->source, AL_BUFFER, 0);
        alDeleteBuffers(1, &music->buffer);
        alDeleteSources(1, &music->source);
	}
}