#ifndef GAME_API_H_
#define GAME_API_H_

/* [NOTE]: order of includes matters here */
#include "g_types.h"
#include "mem.h"
#include "ga_types.h"

#ifdef GA_PLATFORM_LAYER

typedef void (*ga_update_and_render)(uint32_t *draw_buffer,
									 int width, int height,
									 float elapsed_time);
typedef void (*ga_update_sound)(void *audio_buffer, int frames);
typedef void (*ga_controller_input)(void);
typedef void (*ga_init)(Game_Memory *game_memory, Model_Data *model);

/* GLOBAL function pointers required for runtime-linking */
static ga_update_and_render game_update_and_render = NULL;
static ga_update_sound game_update_sound = NULL;
static ga_init game_init = NULL;

#endif /* PLATFORM_LAYER */

#ifdef GA_GAME_LAYER

void ga_init(Game_Memory *game_memory, Model_Data *model);
void ga_update_and_render(uint32_t *draw_buffer, int width, int height, float elapsed_time);
void ga_update_sound(void *audio_buffer, int frames);
void ga_upbutton_down(void);
void ga_upbutton_up(void);
void ga_downbutton_down(void);
void ga_downbutton_up(void);
void ga_leftbutton_down(void);
void ga_leftbutton_up(void);
void ga_rightbutton_down(void);
void ga_rightbutton_up(void);
/* [TODO]: These sound functions should be button state change functions */
void ga_inc_freq(void);
void ga_dec_freq(void);

#define KEY_PRESSED 1
#define KEY_RELEASED 0

#endif /* GAME_LAYER */
#endif /* GAME_API_H_ */
