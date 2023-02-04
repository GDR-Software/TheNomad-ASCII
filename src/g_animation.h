#ifndef _G_ANIMATION_
#define _G_ANIMATION_

typedef void(*animator)();
void A_Init(Game* const gptr);

// function for complex animations requiring their own specialized functions to run
inline void A_PushAnimation(nomadulong_t ticcount, animator anim);

// the function the updates and checks the animations vector and runs them
void* A_Loop(void *arg);

/* functions for standard not-very-unique animations in the game */

// function for single-sprite animation at a specific position on the map, lasts 1 second
inline void A_PushSprite(sprite_t ch, coord_t pos);
// function for a 2 second delay version of the above function
inline void A_PushSprite2(sprite_t ch, coord_t pos);

#endif
