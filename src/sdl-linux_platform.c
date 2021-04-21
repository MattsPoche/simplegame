#define _POSIX_C_SOURCE 200809L /* NOTE: Required for nanosleep (glibc) */
#include <assert.h>
#include <dlfcn.h>
#include <stdio.h>
#include <stdint.h>
#include <sys/stat.h>
#include <errno.h>
#include <time.h>
#include <pthread.h>
#include <poll.h>
#include <unistd.h>
#include <sys/inotify.h>
#include <SDL2/SDL.h>

#include "const.h"
#include "game_api.h"

typedef struct {
	SDL_Window *window;
	SDL_Renderer *renderer;
	SDL_Texture *draw_buffer;
	int32_t w, h;
} Graphics;

static Graphics init_graphics(void);
static SDL_AudioDeviceID init_audio(void);
static void plat_quit(void);
static void plat_pause_audio(void);
static uint64_t plat_get_wall_clock(void);
static float plat_get_s_elapsed(uint64_t start, uint64_t end);
static int plat_msleep(int64_t msec);

static uint8_t quit = 0;
static uint8_t pause_audio = 1;
static SDL_AudioDeviceID adev;
static const char game_lib_name[] = "bin/game.so";
static int game_lib_changed = 0;
static ga_update_and_render game_update_and_render = NULL;
static ga_update_sound game_update_sound = NULL;
#define SYMCAST(h) (*(void **)(&(h)))

#include "sdl_event_handler.c"
#include "watchfile.c"

static Graphics
init_graphics(void)
{
	SDL_Window *window = SDL_CreateWindow("SimpleGame", 0, 0,
										  SCREEN_WIDTH, SCREEN_HEIGHT, SDL_WINDOW_SHOWN);
	if (window == NULL) {
		fprintf(ERR_LOG, "ERROR Could not create window %s\n", SDL_GetError());
		exit(1);
	}
	SDL_Renderer *renderer = SDL_CreateRenderer(window, -1, 
												SDL_RENDERER_ACCELERATED|SDL_RENDERER_PRESENTVSYNC);
	if (renderer == NULL) {
		fprintf(ERR_LOG, "ERROR Could not create renderer %s\n", SDL_GetError());
		exit(1);
	}
	SDL_Texture *texture = SDL_CreateTexture(renderer,
											 SDL_PIXELFORMAT_RGBA32,
											 SDL_TEXTUREACCESS_STREAMING,
											 SCREEN_WIDTH, SCREEN_HEIGHT);
	if (texture == NULL) {
		fprintf(ERR_LOG, "ERROR Could not create texture %s\n", SDL_GetError());
		exit(1);
	}
	
	return (Graphics){
		.window = window,
		.renderer = renderer,
		.draw_buffer = texture,
		.w = SCREEN_WIDTH,
		.h = SCREEN_HEIGHT 
	};
}

static void
audio_cb(void *userdata, uint8_t *stream, int32_t bytes)
{
	if (game_update_sound == NULL) {
		return;
	}

	UNUSED(userdata);
	int32_t frames = bytes / (sizeof(int16_t) * AUDIO_CHANNELS);
	game_update_sound(stream, frames);
}

static SDL_AudioDeviceID
init_audio(void)
{
	SDL_InitSubSystem(SDL_INIT_AUDIO);
	SDL_AudioSpec want = {
		.freq = AUDIO_SAMPLE_RATE,
		.format = AUDIO_S16SYS,
		.channels = AUDIO_CHANNELS,
		.samples = 4096,
		.callback = audio_cb,
		.userdata = NULL,
	};
	SDL_AudioDeviceID adev = SDL_OpenAudioDevice(NULL, 0, &want, NULL, 0);
	if (adev == 0) {
		fprintf(ERR_LOG, "ERROR Could not open audio device: %s\n", SDL_GetError());
		exit(1);
	}
	return adev;
}

static void
plat_quit(void)
{
	SDL_Event event;
	event.type = SDL_QUIT;
	SDL_PushEvent(&event);
}

static void
plat_pause_audio(void)
{
	SDL_PauseAudioDevice(adev, pause_audio);
	pause_audio = pause_audio ? 0 : 1;
}

static uint64_t
plat_get_wall_clock(void)
{
	return clock();
}

static float
plat_get_s_elapsed(uint64_t start, uint64_t end)
{
	return (float)((end - start)) / (float)CLOCKS_PER_SEC;
}

static int
plat_msleep(int64_t msec)
{
	struct timespec ts;
	int res;
	if (msec < 0) {
		errno = EINVAL;
		return -1;
	}
	ts.tv_sec = msec / 1000;
	ts.tv_nsec = (msec % 1000) * 1000000;
	do {
		res = nanosleep(&ts, &ts);
	} while (res && errno == EINTR);

	return res;
}

static void *
link_to_game(void)
{
	printf("hello, from link_to_game\n");
	void *handle = dlopen(game_lib_name, RTLD_NOW|RTLD_LOCAL);
	if (handle == NULL) {
		fprintf(ERR_LOG, "[ERROR] problem dynamically loading game: %s\n",
				dlerror());
		return NULL;
	}
	SYMCAST(game_update_and_render) = dlsym(handle, "ga_update_and_render");
	if (game_update_and_render == NULL) {
		goto error;
	}
	SYMCAST(game_update_sound) = dlsym(handle, "ga_update_sound");
	if (game_update_sound == NULL) {
		goto error;
	}
	FOREACH_ARRAY(Key *, key, downkeys, {
		if (key->link_symbol) {
			SYMCAST(key->game_action) = dlsym(handle, key->link_symbol);
			if (key->game_action == NULL) {
				goto error;
			}
		}
	});

	FOREACH_ARRAY(Key *, key, upkeys, {
		if (key->link_symbol) {
			SYMCAST(key->game_action) = dlsym(handle, key->link_symbol);
			if (key->game_action == NULL) {
				goto error;
			}
		}
	});

	return handle;
error:
	fprintf(ERR_LOG, "[ERROR] Unable to link symbol %s\n", dlerror());
	return NULL;
}

static void
unlink_to_game(void *handle)
{
	printf("hello, from unlink_to_game\n");
	game_update_and_render = NULL;
	game_update_sound = NULL;
	FOREACH_ARRAY(Key *, key, downkeys, {
		if (key->link_symbol) {
			key->game_action = NULL;
		}
	});

	FOREACH_ARRAY(Key *, key, upkeys, {
		if (key->link_symbol) {
			key->game_action = NULL;
		}
	});
	dlclose(handle);
}

int
main(void)
{
	Graphics g = init_graphics();
	adev = init_audio();
	void *game_lib_handle = link_to_game();
	if (game_lib_handle == NULL) {
		exit(1);
	}
	start_watching("bin", "game.so");

	float refreash_rate = 60.0f;
	float frame_tt = 1.0f / refreash_rate;
	uint64_t frame_count = 0;
	uint64_t last_counter = plat_get_wall_clock();
	SDL_Event event;
	SDL_PauseAudioDevice(adev, 0);
	while (!quit) {
		if (game_lib_changed) {
			SDL_PauseAudioDevice(adev, 1);
			pause_audio = 0;
			unlink_to_game(game_lib_handle);
			link_to_game();
			plat_pause_audio();
			game_lib_changed = 0;
		}
		while (SDL_PollEvent(&event)) {
			if (event.type < SDL_USEREVENT) {
				if (event.type < SDLEVENTRANGE && handler[event.type]) {
					handler[event.type](&event);
				} /* else { events not handled by handler } */
			} /* else { handle user registered events } */
		}
		void *pixels; 
		int32_t pitch;
		if (game_update_and_render != NULL) {
			SDL_LockTexture(g.draw_buffer, NULL, &pixels, &pitch);
			game_update_and_render(pixels, g.w, g.h);
			SDL_UnlockTexture(g.draw_buffer);
			SDL_RenderCopy(g.renderer, g.draw_buffer, NULL, NULL);
			SDL_RenderPresent(g.renderer);
		}

		uint64_t wall_clock = plat_get_wall_clock();
		float elapsed_time = plat_get_s_elapsed(last_counter, wall_clock);
		if (plat_msleep((int64_t)((frame_tt - elapsed_time) * 1000)) < 0) {
			fprintf(ERR_LOG, "Exceeded frame target time; et: %f, ftt: %f\n", 
					elapsed_time, frame_tt);
		}
		++frame_count;
#ifdef PRINTFPS
		printf("fps: %f\n", (float)frame_count / SDL_GetTicks() * 1000);
#endif
		last_counter = plat_get_wall_clock();
	}
	unlink_to_game(game_lib_handle);
	SDL_PauseAudioDevice(adev, 1);
	SDL_CloseAudioDevice(adev);
	return 0;
}
