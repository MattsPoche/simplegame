#define GA_GAME_LAYER
#include <assert.h>
#include <math.h>
#include <stdint.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include "const.h"
#include "game_api.h"

typedef struct {
	Game_Memory *mem;
	size_t entity_count;
} Game_State;

#define ENTITY(index) (((Entity *)(game_state.mem->entity_pool + sizeof(Mem_Pool)))[index])
#define AUDIO_STATE   ((Audio_State *)(game_state.mem->audio_pool + sizeof(Mem_Pool)))
#define CONTROL_STATE ((Control_State *)(game_state.mem->control_pool + sizeof(Mem_Pool)))

static Game_State game_state;

#include "mem.c"
#include "graphics.c"
#include "control.c"

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
	/* define cube2 entity */
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

	/* init control state */
	Control_State *control = mem_pool_push(game_state.mem->control_pool, sizeof(Control_State));
	*control = (Control_State) {0};

	/* init audio state */
	Audio_State *audio = mem_pool_push(game_state.mem->audio_pool, sizeof(Audio_State));
	audio->frequency = 110.0f;
}

void
ga_update_and_render(uint32_t *draw_buffer, int width, int height, float elapsed_time)
{
	if (CONTROL_STATE->upbutton_state) {
		ENTITY(0).pos.z -= 5.0f;
	}
	if (CONTROL_STATE->downbutton_state) {
		ENTITY(0).pos.z += 5.0f;
	}
	if (CONTROL_STATE->leftbutton_state) {
		ENTITY(0).pos.x += 1.0f;
	}
	if (CONTROL_STATE->rightbutton_state) {
		ENTITY(0).pos.x -= 1.0f;
	}
	if (CONTROL_STATE->floatbutton_state) {
		ENTITY(0).pos.y -= 1.0f;
	}
	if (CONTROL_STATE->sinkbutton_state) {
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
		float sv =  amplitude * sinf((2.0f * AUDIO_STATE->frequency * PI) *(ftime));
		*sample++ = sv; /* left channel */
		*sample++ = sv; /* right channel */
	}
}

