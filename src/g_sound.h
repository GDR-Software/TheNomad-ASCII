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
// DESCRIPTION: src/g_sound.h
//----------------------------------------------------------
#ifndef _G_SOUND_
#define _G_SOUND_

#pragma once

void S_PlayMusic(const char* name);
void Snd_RaiseMusic();
void Snd_LowerMusic();
void P_PlaySFX(const char* name);
void Snd_Kill();
void M_PlaySFX(const char* name, Mob* const actor);
void G_RunSound();
void P_AdaptiveSound();


#endif