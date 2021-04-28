#define GA_GAME_LAYER
#include <assert.h>
#include <math.h>
#include <stdint.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include "const.h"
#include "game_api.h"

#include "mem.c"
#include "graphics.c"

#define ENTITY_TAG_LEN 24
typedef struct {
	int upbutton_state;
	int downbutton_state;
	int leftbutton_state;
	int rightbutton_state;
	int floatbutton_state;
	int sinkbutton_state;
} Control_State;

typedef struct {
	int active;
	Vec3 pos;
	float scalar;
	uint32_t color;
	Model_Data *model;
	char tag[24];
	int tag_len;
} Entity;

typedef struct {
	Game_Memory *mem;
	size_t entity_count;
	Control_State controls; /* [TODO]: add control state and audio data to game memory */
	float frequency;
} Game_State;

#define ENTITY(index) (((Entity *)(game_state.mem->entity_pool + sizeof(Mem_Pool)))[index])

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
	game_state.mem = game_memory;	
	Entity *cube = mem_pool_push(game_state.mem->entity_pool, sizeof(Entity));
	/* define cube entity */
	cube->active = 1;
	cube->pos.x = 0.0f,
	cube->pos.y = 0.0f,
	cube->pos.z = 400.0f,
	cube->scalar = 50.0f,
	cube->color = 0xFF << GSHIFT;
	cube->model = model;
	memcpy(cube->tag, "Cube", 4);
	cube->tag_len = 4;
	Entity *cube2 = mem_pool_push(game_state.mem->entity_pool, sizeof(Entity));
	/* define cube entity */
	cube2->active = 1;
	cube2->pos.x = 100.0f,
	cube2->pos.y = 0.0f,
	cube2->pos.z = 400.0f,
	cube2->scalar = 50.0f,
	cube2->color = 0xFF << RSHIFT;
	cube2->model = model;
	memcpy(cube2->tag, "Cube2", 5);
	cube2->tag_len = 5;
	game_state.entity_count += 2;
	game_state.frequency = 110.0f;
}

void
ga_update_and_render(uint32_t *draw_buffer, int width, int height, float elapsed_time)
{
	/* [TODO]: move cube_offset into game_state */
	if (game_state.controls.upbutton_state) {
		ENTITY(0).pos.z -= 5.0f;
	}
	if (game_state.controls.downbutton_state) {
		ENTITY(0).pos.z += 5.0f;
	}
	if (game_state.controls.leftbutton_state) {
		ENTITY(0).pos.x += 1.0f;
	}
	if (game_state.controls.rightbutton_state) {
		ENTITY(0).pos.x -= 1.0f;
	}
	if (game_state.controls.floatbutton_state) {
		ENTITY(0).pos.y -= 1.0f;
	}
	if (game_state.controls.sinkbutton_state) {
		ENTITY(0).pos.y += 1.0f;
	}
	clear_screen(draw_buffer, width, height);

	/* render active entities */
	for (size_t e = 0; e < game_state.entity_count; ++e) {
		if (ENTITY(e).active) {
			render_3d_model(draw_buffer, width, height, elapsed_time,
					ENTITY(e).model,
					ENTITY(e).pos,
					ENTITY(e).scalar,
					ENTITY(e).color);
		}
	}
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

