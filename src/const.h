#ifndef CONST_H_
#define CONST_H_

#define ERR_LOG stderr
#define UNUSED(x) ((void)x)
#define UNUSED_FUNC
#define UNUSED_GLOBAL

#define SCREEN_WIDTH (1280 * 1.4)
#define SCREEN_HEIGHT (720 * 1.4)

#if 0
#if SDL_BYTEORDER == SDL_BIG_ENDIAN
#define RMASK 0xff000000
#define GMASK 0x00ff0000
#define BMASK 0x0000ff00
#define AMASK 0x000000ff
#define RSHIFT 24
#define GSHIFT 16
#define BSHIFT 8
#define ASHIFT 0
#endif 
#endif /* #if 0 */

#define RMASK 0x000000ff
#define GMASK 0x0000ff00
#define BMASK 0x00ff0000
#define AMASK 0xff000000
#define RSHIFT 0
#define GSHIFT 8
#define BSHIFT 16
#define ASHIFT 24

#define AUDIO_SAMPLE_RATE 48000
#define AUDIO_CHANNELS 2
#define STEP (pow(2.0, 1.0 / 12.0)) /* divide the octave */
#define PI 3.14159265358979323846f

#define ARRCOUNT(arr) (sizeof(arr) / sizeof(arr[0]))
#define FOREACH_ARRAY(type, val, arr, body) \
	do { \
		for (size_t _i = 0; _i < ARRCOUNT(arr); ++_i) { \
			type val = &arr[_i]; 						\
			body;										\
		} \
	} while (0);
#endif /* CONST_H_ */
