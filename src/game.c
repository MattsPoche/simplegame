#include <stdint.h>
#include <stdio.h>
#include <math.h>

#include "game.h"
#include "const.h"

static int xoffset = 0;
static int yoffset = 0;
static float frequency = 110.0f;

static int upbutton_state = 0;
static int downbutton_state = 0;
static int leftbutton_state = 0;
static int rightbutton_state = 0;

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
ga_inc_freq(void)
{
	frequency *= STEP;
}

void
ga_dec_freq(void)
{
	frequency /= STEP;
}

static void
render_weird_gradient(uint32_t *draw_buffer, int width, int height)
{
	uint32_t *pixel = draw_buffer;
	for (int y = 0; y < height; ++y) {
		for (int x = 0; x < width; ++x) {
			*pixel = ((uint8_t)(y + yoffset) << RSHIFT)
					|((uint8_t)(x + xoffset) << GSHIFT);

			++pixel;
		}
	}
}

void
ga_update_and_render(uint32_t *draw_buffer, int width, int height)
{
	if (upbutton_state) {
		yoffset -= 5;
	}
	if (downbutton_state) {
		yoffset += 5;
	}
	if (leftbutton_state) {
		xoffset -= 5;
	}
	if (rightbutton_state) {
		xoffset += 5;
	}
	render_weird_gradient(draw_buffer, width, height);
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

