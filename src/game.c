#include <assert.h>
#include <stdint.h>
#include <stdio.h>
#include <stdlib.h>
#include <math.h>

#include "const.h"
#include "game.h"

#include "graphics.c"

static float frequency = 110.0f;

static int upbutton_state = 0;
static int downbutton_state = 0;
static int leftbutton_state = 0;
static int rightbutton_state = 0;
static int floatbutton_state = 0;
static int sinkbutton_state = 0;

void
ga_upbutton_down(void)
{
	upbutton_state = KEY_PRESSED;
}

void
ga_upbutton_up(void)
{
	upbutton_state = KEY_RELEASED;
}

void
ga_downbutton_down(void)
{
	downbutton_state = KEY_PRESSED;
}

void
ga_downbutton_up(void)
{
	downbutton_state = KEY_RELEASED;
}

void
ga_leftbutton_down(void)
{
	leftbutton_state = KEY_PRESSED;
}

void
ga_leftbutton_up(void)
{
	leftbutton_state = KEY_RELEASED;
}

void
ga_rightbutton_down(void)
{
	rightbutton_state = KEY_PRESSED;
}

void
ga_rightbutton_up(void)
{
	rightbutton_state = KEY_RELEASED;
}

void
ga_floatbutton_down(void)
{
	floatbutton_state = KEY_PRESSED;
}

void
ga_floatbutton_up(void)
{
	floatbutton_state = KEY_RELEASED;
}

void
ga_sinkbutton_down(void)
{
	sinkbutton_state = KEY_PRESSED;
}

void
ga_sinkbutton_up(void)
{
	sinkbutton_state = KEY_RELEASED;
}

void
ga_inc_freq(void)
{
	frequency *= STEP;
}

void
ga_dec_freq(void)
{
	frequency /= STEP;
}

void
ga_register_malloc(malloc_cb f)
{
	ga_malloc = f;
}

UNUSED_FUNC int
rand_range(int low, int high)
{
	return (rand() % (high - low + 1)) + low;
}

void
ga_update_and_render(uint32_t *draw_buffer, int width, int height, float elapsed_time)
{
	if (upbutton_state) {
		cube_offset.z -= 5.0f;
	}
	if (downbutton_state) {
		cube_offset.z += 5.0f;
	}
	if (leftbutton_state) {
		cube_offset.x += 1.0f;
	}
	if (rightbutton_state) {
		cube_offset.x -= 1.0f;
	}
	if (floatbutton_state) {
		cube_offset.y -= 1.0f;
	}
	if (sinkbutton_state) {
		cube_offset.y += 1.0f;
	}
	clear_screen(draw_buffer, width, height);
	render_3d_model(draw_buffer, width, height, elapsed_time, cube, 12);
}

void
ga_update_sound(void *audio_buffer, int32_t frames)
{
	static size_t pfcount = 0;
	float amplitude = 2000.0f;
	int16_t *sample = audio_buffer;
	for (int32_t i = 0; i < frames; ++i, ++pfcount) {
		float ftime = (float)pfcount / (float)AUDIO_SAMPLE_RATE;
		float sv =  amplitude * sinf((2.0f * frequency * PI) *(ftime));
		*sample++ = sv; /* left channel */
		*sample++ = sv; /* right channel */
	}
}

