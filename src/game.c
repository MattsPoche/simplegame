#define GA_GAME_LAYER
#include <assert.h>
#include <stdint.h>
#include <stdio.h>
#include <stdlib.h>
#include <math.h>

#include "const.h"
#include "game_api.h"

#include "mem.c"
#include "graphics.c"

typedef struct {
	int upbutton_state;
	int downbutton_state;
	int leftbutton_state;
	int rightbutton_state;
	int floatbutton_state;
	int sinkbutton_state;
} Control_State;

typedef struct {
	Game_Memory *game_memory;
	Model_Data *cube_model;
	Control_State controls;
	float frequency;
} Game_State;

static Game_State game_state;


void
ga_upbutton_down(void)
{
	game_state.controls.upbutton_state = KEY_PRESSED;
}

void
ga_upbutton_up(void)
{
	game_state.controls.upbutton_state = KEY_RELEASED;
}

void
ga_downbutton_down(void)
{
	game_state.controls.downbutton_state = KEY_PRESSED;
}

void
ga_downbutton_up(void)
{
	game_state.controls.downbutton_state = KEY_RELEASED;
}

void
ga_leftbutton_down(void)
{
	game_state.controls.leftbutton_state = KEY_PRESSED;
}

void
ga_leftbutton_up(void)
{
	game_state.controls.leftbutton_state = KEY_RELEASED;
}

void
ga_rightbutton_down(void)
{
	game_state.controls.rightbutton_state = KEY_PRESSED;
}

void
ga_rightbutton_up(void)
{
	game_state.controls.rightbutton_state = KEY_RELEASED;
}

void
ga_floatbutton_down(void)
{
	game_state.controls.floatbutton_state = KEY_PRESSED;
}

void
ga_floatbutton_up(void)
{
	game_state.controls.floatbutton_state = KEY_RELEASED;
}

void
ga_sinkbutton_down(void)
{
	game_state.controls.sinkbutton_state = KEY_PRESSED;
}

void
ga_sinkbutton_up(void)
{
	game_state.controls.sinkbutton_state = KEY_RELEASED;
}

void
ga_inc_freq(void)
{
	game_state.frequency *= STEP;
}

void
ga_dec_freq(void)
{
	game_state.frequency /= STEP;
}

UNUSED_FUNC int
rand_range(int low, int high)
{
	return (rand() % (high - low + 1)) + low;
}

void
ga_init(Game_Memory *game_memory, Model_Data *model)
{
	game_state.game_memory = game_memory;	
	game_state.cube_model = model;
	game_state.frequency = 110.0f;
}

void
ga_update_and_render(uint32_t *draw_buffer, int width, int height, float elapsed_time)
{
	/* [TODO]: move cube_offset into game_state */
	if (game_state.controls.upbutton_state) {
		cube_offset.z -= 5.0f;
	}
	if (game_state.controls.downbutton_state) {
		cube_offset.z += 5.0f;
	}
	if (game_state.controls.leftbutton_state) {
		cube_offset.x += 1.0f;
	}
	if (game_state.controls.rightbutton_state) {
		cube_offset.x -= 1.0f;
	}
	if (game_state.controls.floatbutton_state) {
		cube_offset.y -= 1.0f;
	}
	if (game_state.controls.sinkbutton_state) {
		cube_offset.y += 1.0f;
	}
	clear_screen(draw_buffer, width, height);
	render_3d_model(draw_buffer, width, height, elapsed_time,
			game_state.cube_model->tris, game_state.cube_model->faces);
}

void
ga_update_sound(void *audio_buffer, int32_t frames)
{
	static size_t pfcount = 0;
	float amplitude = 2000.0f;
	int16_t *sample = audio_buffer;
	for (int32_t i = 0; i < frames; ++i, ++pfcount) {
		float ftime = (float)pfcount / (float)AUDIO_SAMPLE_RATE;
		float sv =  amplitude * sinf((2.0f * game_state.frequency * PI) *(ftime));
		*sample++ = sv; /* left channel */
		*sample++ = sv; /* right channel */
	}
}

