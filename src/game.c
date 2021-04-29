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

#define ENTITY(index)        (((Entity *)(game_state.mem->entity_pool + sizeof(Mem_Pool)))[index])
#define AUDIO_STATE          ((Audio_State *)(game_state.mem->audio_pool + sizeof(Mem_Pool)))
#define CONTROL_STATE        ((Control_State *)(game_state.mem->control_pool + sizeof(Mem_Pool)))
#define MODEL_LOOKUP(index)  (((Model_Data **)(game_state.mem->model_lookup + sizeof(Mem_Pool)))[index])

static Game_State game_state;
static const Entity_Def entity_def[] = {
	[0] = {
		.pos    = { 0.0f, 0.0f, 400.0f },
		.scalar = 50.0f,
		.color  = 0xFF << GSHIFT,
		.m      = 0,
		.tag    = "Cube1",
	},
	[1] = {
		.pos    = { 200.0f, 0.0f, 400.0f },
		.scalar = 50.0f,
		.color  = 0xFF << BSHIFT,
		.m      = 0,
		.tag    = "Cube2",
	},
	[2] = {
		.tag = NULL,
	},
};

#include "mem.c"
#include "graphics.c"
#include "control.c"

UNUSED_FUNC int
rand_range(int low, int high)
{
	return (rand() % (high - low + 1)) + low;
}

void
ga_init(Game_Memory *game_memory)
{
	game_state.mem = game_memory;	
	for (size_t i = 0; entity_def[i].tag != NULL; ++i) {
		Entity *e = mem_pool_push(game_state.mem->entity_pool, sizeof(Entity));
		e->active = 1;
		e->pos = entity_def[i].pos;
		e->scalar = entity_def[i].scalar;
		e->color = entity_def[i].color;
		e->model = MODEL_LOOKUP(entity_def[i].m);
		size_t tag_len = strlen(entity_def[i].tag);
		assert(tag_len + 1 < ENTITY_TAG_LEN);
		memcpy(e->tag, entity_def[i].tag, tag_len + 1);
		e->tag_len = tag_len;
		++game_state.entity_count;
	}

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
		ENTITY(0).pos.z += 5.0f;
	}
	if (CONTROL_STATE->downbutton_state) {
		ENTITY(0).pos.z -= 5.0f;
	}
	if (CONTROL_STATE->leftbutton_state) {
		ENTITY(0).pos.x -= 1.0f;
	}
	if (CONTROL_STATE->rightbutton_state) {
		ENTITY(0).pos.x += 1.0f;
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

