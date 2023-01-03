//----------------------------------------------------------
//
// Copyright (C) SIGAAMDAD 2022-2023
//
// This source is available for distribution and/or modification
// only under the terms of the SACE Source Code License as
// published by SIGAAMDAD. All rights reserved
//
// The source is distributed in the hope that it will be
// useful, but WITHOUT ANY WARRANTY; without even the implied
// warranty of FITNESS FOR A PARTICLAR PURPOSE. See the SACE
// Source Code License for more details. If you, however do not
// want to use the SACE Source Code License, then you must use
// this source as if it were to be licensed under the GNU General
// Public License (GPL) version 2.0 or later as published by the
// Free Software Foundation.
//
// DESCRIPTION:
//  src/g_sound.cpp
//----------------------------------------------------------
#include <alsa/asoundlib.h>
#include "g_game.h"

typedef struct WAV_HEADER
{
    /* RIFF Chunk Descriptor */
    nomadenum_t RIFF[4];    // RIFF Header Magic header
    nomaduint_t ChunkSize; // RIFF Chunk Size
    nomadenum_t WAVE[4];    // WAVE Header
    /* "fmt" sub-chunk */
    nomadenum_t fmt[4];         // FMT header
    nomaduint_t Subchunk1Size; // Size of the fmt chunk
    nomadushort_t AudioFormat;   // Audio format 1=PCM,6=mulaw,7=alaw,     257=IBM Mu-Law, 258=IBM A-Law, 259=ADPCM
    nomadushort_t NumOfChan;     // Number of channels 1=Mono 2=Sterio
    nomaduint_t SamplesPerSec; // Sampling Frequency in Hz
    nomaduint_t bytesPerSec;   // bytes per second
    nomadushort_t blockAlign;    // 2=16-bit mono, 4=16-bit stereo
    nomadushort_t bitsPerSample; // Number of bits per sample
    /* "data" sub-chunk */
    nomadenum_t Subchunk2ID[4]; // "data"  string
    nomaduint_t Subchunk2Size; // Sampled data length
} wav_hdr;

typedef struct WAV_FILE
{
    // HEADER
    wav_hdr header;
    FILE* file;
    nomaduint_t rate;
} wav_file;

typedef struct
{
    std::string name; // name of the file
    char* buffer;
    wav_file file;
    nomadenum_t aggressiveness;
} music_t;

typedef struct
{
    std::string name;
    char* buffer;
    wav_file file;
} sfx_t;


class Sound
{
public: // alsa stuff
    snd_pcm_t* pcm_handle;
    snd_pcm_hw_params_t *params;
    snd_pcm_uframes_t frames;
    nomadint_t channels;
    std::vector<char*> sfx_buffer;
    char* music_buffer;
public:
    music_t* current_song;
    sfx_t* current_psound; // the most recent sound the player has made
public:
    Sound() = default;
    ~Sound()
    {
        snd_pcm_drain(pcm_handle);
	    snd_pcm_close(pcm_handle);
    }
    inline void Snd_ReadWavHdr(wav_file* const file)
    {
        wav_hdr* h = &file->header;
        fread(&h->RIFF, sizeof(h->RIFF[0]), sizeof(h->RIFF), file->file);
        fread(&h->ChunkSize, 1, sizeof(h->ChunkSize), file->file);
        fread(&h->WAVE, sizeof(h->WAVE[0]), sizeof(h->WAVE), file->file);
        fread(&h->fmt, sizeof(h->fmt[0]), sizeof(h->fmt), file->file);
        fread(&h->Subchunk1Size, 1, sizeof(h->Subchunk1Size), file->file);
        fread(&h->AudioFormat, 1, sizeof(h->AudioFormat), file->file);
        fread(&h->NumOfChan, 1, sizeof(h->NumOfChan), file->file);
        fread(&h->SamplesPerSec, 1 , sizeof(h->SamplesPerSec), file->file);
        fread(&h->bytesPerSec, 1, sizeof(h->bytesPerSec), file->file);
        fread(&h->blockAlign, 1, sizeof(h->blockAlign), file->file);
        fread(&h->bitsPerSample, 1, sizeof(h->bitsPerSample), file->file);
        fread(&h->Subchunk2ID, sizeof(h->Subchunk2ID[0]), sizeof(h->Subchunk2ID), file->file);
        fread(&h->Subchunk2Size, 1, sizeof(h->Subchunk2Size), file->file);
    }
    inline void Snd_LoadWav(sfx_t* const sfx)
    {
        wav_file* file = &sfx->file;
        file->file = fopen(sfx->name.c_str(), "r");
        Snd_ReadWavHdr(file);
    }
    inline void Snd_LoadWav(music_t* const mus)
    {
        wav_file* file = &mus->file;
        file->file = fopen(mus->name.c_str(), "r");
        Snd_ReadWavHdr(file);
    }
    inline sfx_t* Snd_InitSfx(const char* name)
    {
        current_psound = (sfx_t*)Z_Malloc(sizeof(sfx_t), TAG_PURGELEVEL, &current_psound);
        current_psound->name = name;
        Snd_LoadWav(current_psound);
        return current_psound;
    }

    inline music_t* Snd_InitMusic(const char* name)
    {
        current_song = (music_t*)Z_Malloc(sizeof(music_t), TAG_STATIC, &current_song);
        current_song->name = "Files/gamedata/MUSIC/";
        current_song->name += name;
        Snd_LoadWav(current_song);
        return current_song;
    }
};

static Game* game;
static Sound* snd;
static nomaduint_t rate, tmp;
typedef int8_t sndbyte_t;
static constexpr uint16_t BUFFER_SIZE = 4096;

void SoundAssigner(Game* const gptr)
{
    game = gptr;
}

void P_PlaySfx(const char* sfxname)
{
    sfx_t* sfx = snd->Snd_InitSfx(sfxname);

    nomadushort_t bytesPerSample = sfx->file.header.bitsPerSample / 8;     // Number of bytes per sample
    nomadulong_t numSamples = sfx->file.header.ChunkSize / bytesPerSample;        // How many samples are in the wav file
    sndbyte_t buffer[BUFFER_SIZE];

    snd_pcm_uframes_t buffer_size = BUFFER_SIZE; // bytes
    snd_pcm_hw_params_set_buffer_size_near(snd->pcm_handle, snd->params, &buffer_size);

    snd_pcm_uframes_t period_size = bytesPerSample; // bytes
    snd_pcm_hw_params_set_period_size_near(snd->pcm_handle, snd->params, &period_size, NULL);
    size_t bytesRead;
    while ((bytesRead = fread(&buffer, sizeof(sndbyte_t), BUFFER_SIZE / (sizeof(sndbyte_t)), sfx->file.file)) > 0) {
        // Number of frames in the current buffer
        snd_pcm_uframes_t frames = bytesRead / (snd->channels * bytesPerSample);
        
        // Writes buffer of frames
        nomadint_t err = snd_pcm_writei(snd->pcm_handle, &buffer, frames);

        if (err == -EPIPE) {
            // an underrun occurred
            fprintf(stderr, "Error: ALSA Sound Lib Underrun Occured (XRUN)\n");
            snd_pcm_prepare(snd->pcm_handle);
        } else if (err < 0) {
            break; // kill the sound
        }
    }
    snd_pcm_drain(snd->pcm_handle);
}

void G_PlayMusic(music_t* const mus)
{
    nomadushort_t bytesPerSample = mus->file.header.bitsPerSample / 8;     // Number of bytes per sample
    nomadulong_t numSamples = mus->file.header.ChunkSize / bytesPerSample;        // How many samples are in the wav file
    sndbyte_t buffer[BUFFER_SIZE];

    snd_pcm_uframes_t buffer_size = BUFFER_SIZE; // bytes
    snd_pcm_hw_params_set_buffer_size_near(snd->pcm_handle, snd->params, &buffer_size);

    snd_pcm_uframes_t period_size = bytesPerSample; // bytes
    snd_pcm_hw_params_set_period_size_near(snd->pcm_handle, snd->params, &period_size, NULL);
    size_t bytesRead;
    while ((bytesRead = fread(&buffer, sizeof(sndbyte_t), BUFFER_SIZE / (sizeof(sndbyte_t)), mus->file.file)) > 0) {
        // Number of frames in the current buffer
        snd_pcm_uframes_t frames = bytesRead / (snd->channels * bytesPerSample);
        
        // Writes buffer of frames
        nomadint_t err = snd_pcm_writei(snd->pcm_handle, &buffer, frames);

        if (err == -EPIPE) {
            // an underrun occurred
 //           fprintf(stderr, "Error: ALSA Sound Lib Underrun Occured (XRUN)\n");
 //           snd_pcm_prepare(snd->pcm_handle);
            break;
        } else if (err < 0) {
            break; // kill the sound
        }
    }
    snd_pcm_drain(snd->pcm_handle);
}


static music_t* next; // the queue'd up song
static music_t* current; // the currently playing song

void* P_AdaptiveSound(void *arg)
{
    while (game->gamestate == GS_LEVEL) {
        if (!current) {
            current = snd->Snd_InitMusic("SuperGoreNest.wav");
            next = nullptr;
            G_PlayMusic(current);
        }
        else {
            continue; // TODO: finish up this function
        }
    };
    return NULL;
}

void Snd_Init(Game* const game)
{
    snd = (Sound *)Z_Malloc(sizeof(Sound), TAG_STATIC, &snd);
    snd->channels = 2;
    int err;
    if ((err = snd_pcm_open(&snd->pcm_handle, "default", SND_PCM_STREAM_PLAYBACK, 0)) < 0) {
        fprintf(stderr, "Error: failed to initialize alsa sound library (PCM device \"default\" couldn't be opened).\n");
        fprintf(stderr, "ALSA StrError: %s\n", snd_strerror(err));
        exit(EXIT_FAILURE);
    }
    snd_pcm_hw_params_malloc(&snd->params);
    snd_pcm_hw_params_any(snd->pcm_handle, snd->params);

    if ((err = snd_pcm_hw_params_set_access(snd->pcm_handle, snd->params, SND_PCM_ACCESS_RW_INTERLEAVED)) < 0)
        fprintf(stderr, "Error: Can't set interleaved mode. %s\n", snd_strerror(err));
    if ((err = snd_pcm_hw_params_set_format(snd->pcm_handle, snd->params, SND_PCM_FORMAT_S16_LE)) < 0)
        fprintf(stderr, "Error: Can't set format. %s\n", snd_strerror(err));

    if ((err = snd_pcm_hw_params_set_channels(snd->pcm_handle, snd->params, snd->channels)) < 0)
        fprintf(stderr, "Error: Can't set channels number. %s\n", snd_strerror(err));

    if ((err = snd_pcm_hw_params_set_rate_near(snd->pcm_handle, snd->params, &rate, 0)) < 0)
        fprintf(stderr, "Error: Can't set rate. %s\n", snd_strerror(err));

    
    if ((err = snd_pcm_hw_params(snd->pcm_handle, snd->params)) < 0)
        fprintf(stderr, "Error: Can't set hardware parameters. %s\n", snd_strerror(err));

    snd_pcm_hw_params_get_channels(snd->params, &tmp);
    snd_pcm_hw_params_get_rate(snd->params, &tmp, 0);
}