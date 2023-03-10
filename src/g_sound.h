#ifndef _G_SOUND_
#define _G_SOUND_

#pragma once


void P_PlaySFX(const std::string& name);
void Snd_Kill();
void M_PlaySFX(const std::string& name, Mob* const actor);
void G_RunSound();
void P_AdaptiveSound();


#endif