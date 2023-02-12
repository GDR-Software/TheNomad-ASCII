#ifndef _G_ANIMATION_
#define _G_ANIMATION_

void A_Init(Game* const gptr);

// the function the updates and checks the animations vector and runs them
void* A_Loop(void *arg);

typedef void (*animator_t)(); // generic animations
typedef void (*animatorm_t)(Mob*); // mob-specific animations
typedef void (*animatorp_t)(Playr*); // player-specific animations
typedef void (*animatorb_t)(NPC*); // bot-specific animations

void A_PushAnimation(nomadlong_t numtics, animator_t anim);
void A_PushAnimation(nomadlong_t numtics, animatorm_t anim, Mob* mob);
void A_PushAnimation(nomadlong_t numtics, animatorp_t anim, Playr* playr);
void A_PushAnimation(nomadlong_t numtics, animatorb_t anim, NPC* npc);

#endif