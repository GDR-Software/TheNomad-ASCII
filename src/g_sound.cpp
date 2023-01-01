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
#include <SDL2/SDL_mixer.h>
#include <SDL2/SDL_audio.h>
#include <alsa/asoundlib.h>
#include "g_game.h"

class Sound
{
private:
    SDL_AudioDeviceID device;
    SDL_AudioSpec want;
public:
    Music* current_song;
    std::vector<Audio*> sfx_list;
public:
    Sound() = default;
    ~Sound() = delete;
};

static Game* game;

void SoundAssigner(Game* const gptr)
{
    game = gptr;
}

void G_AdaptiveSound()
{

}

void Snd_Init()
{
}