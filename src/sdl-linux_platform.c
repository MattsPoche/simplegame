#define GA_PLATFORM_LAYER
#define _POSIX_C_SOURCE 200809L /* [NOTE]: Required for nanosleep (glibc) 
 								 * Maybe just use sdl_wait instead?
 								 * */
#define _DEFAULT_SOURCE

#include <assert.h>
#include <dirent.h>
#include <dlfcn.h>
#include <ctype.h>
#include <errno.h>
#include <poll.h>
#include <pthread.h>
#include <stdint.h>
#include <stdio.h>
#include <sys/inotify.h>
#include <sys/stat.h>
#include <time.h>
#include <unistd.h>
#include <SDL2/SDL.h>

#include "const.h"
#include "game_api.h"
#define STR_VIEW_IMPLEMENTATION
#include "strview.h"

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
static uint8_t pause_audio = 0;
static SDL_AudioDeviceID adev;
static const char game_lib_name[] = "bin/game.so";

#include "mem.c"
#include "sdl_event_handler.c"
#include "watchfile.c"

#define SYMCAST(h) (*(void **)(&(h)))

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
	/* [TODO]: use flag to keep lib data loaded */
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
	SYMCAST(game_init) = dlsym(handle, "ga_init");
	if (game_init == NULL) {
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
	game_update_and_render = NULL;
	game_update_sound = NULL;
	game_init = NULL;
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

Model_Data *
deserialize_wavefront_obj_str(Mem_Pool *model_pool, Str_View file)
{
	Model_Data *model_data = NULL;
	Vec3 *temp_verts = NULL;
	Str_View fcur = file;
	size_t face_count = 0;
	size_t vert_count = 0;
	/* count the number of surfaces */
	while (fcur.len > 0) {
		if (*fcur.str == 'f') {
			face_count++;
		}
		if (*fcur.str == 'v') {
			vert_count++;
		}
		fcur = sv_skip_to_next_ln(fcur);
	}
	model_data = mem_pool_push(model_pool,
			sizeof(Model_Data) + (face_count * sizeof(Tri3)));
	assert(model_data != NULL);
	model_data->faces = face_count;
	temp_verts = malloc(vert_count * sizeof(Vec3));
	fcur = file;
	size_t vadd_index = 0;
	size_t face_index = 0;
	while (fcur.len > 0 && face_index < face_count) {
		if (*fcur.str == 'o') {
			fcur.str++; fcur.len--;
			fcur = sv_skip_whitespace(fcur);
			Str_Str res = sv_word(fcur);
			Str_View word = res.sv;
			fcur = res.rest;
			assert (word.len < MODEL_NAME_MAX_LEN);
			memcpy(model_data->name, word.str, word.len);
			model_data->name[word.len] = '\0';
			fcur = sv_skip_to_next_ln(fcur);
			continue;
		}
		if (*fcur.str == 'v') {
			fcur.str++; fcur.len--;
			char *endptr;
			float x, y, z;
			x = strtof(fcur.str, &endptr); 
			if (fcur.str == endptr) {
				/* parse error */
				fprintf(ERR_LOG, "ERROR unexpected format reading Model file\n");
				exit(1);
			}
			fcur.str = endptr;
			fcur.len -= endptr - fcur.str;
			y = strtof(fcur.str, &endptr); 
			if (fcur.str == endptr) {
				/* parse error */
				fprintf(ERR_LOG, "ERROR unexpected format reading Model file\n");
				exit(1);
			}
			fcur.str = endptr;
			fcur.len -= endptr - fcur.str;
			z = strtof(fcur.str, &endptr); 
			if (fcur.str == endptr) {
				/* parse error */
				fprintf(ERR_LOG, "ERROR unexpected format reading Model file\n");
				exit(1);
			}
			fcur.str = endptr;
			fcur.len -= endptr - fcur.str;
			temp_verts[vadd_index].x = x;
			temp_verts[vadd_index].y = y;
			temp_verts[vadd_index].z = z;
			vadd_index++;
			fcur = sv_skip_to_next_ln(fcur);
			continue;
		} 
		if (*fcur.str == 'f') {
			fcur.str++; fcur.len--;
			char *endptr;
			int p0, p1, p2;
			p0 = (int)strtof(fcur.str, &endptr) - 1;
			if (fcur.str == endptr) {
				/* parse error */
				fprintf(ERR_LOG, "ERROR unexpected format reading Model file\n");
				exit(1);
			}
			fcur.str = endptr;
			fcur.len -= endptr - fcur.str;
			p1 = (int)strtof(fcur.str, &endptr) - 1; 
			if (fcur.str == endptr) {
				/* parse error */
				fprintf(ERR_LOG, "ERROR unexpected format reading Model file\n");
				exit(1);
			}
			fcur.str = endptr;
			fcur.len -= endptr - fcur.str;
			p2 = (int)strtof(fcur.str, &endptr) - 1;
			if (fcur.str == endptr) {
				/* parse error */
				fprintf(ERR_LOG, "ERROR unexpected format reading Model file\n");
				exit(1);
			}
			fcur.str = endptr;
			fcur.len -= endptr - fcur.str;
			model_data->tris[face_index].v[0] = temp_verts[p0];
			model_data->tris[face_index].v[1] = temp_verts[p1];
			model_data->tris[face_index].v[2] = temp_verts[p2];
			face_index++;
			fcur = sv_skip_to_next_ln(fcur);
			continue;
		} 
		fcur = sv_skip_to_next_ln(fcur);
	}
	free(temp_verts);
	return model_data;
}

Str_View
slurp_file(FILE *f)
{
	Str_View sv = {0};
	assert(f != stdin);
	fseek(f, 0, SEEK_END);
	sv.len = ftell(f);
	sv.str = malloc(sv.len);
	assert(sv.str != NULL);
	fseek(f, 0, SEEK_SET);
	fread(sv.str, 1, sv.len, f);

	return sv;
}

Model_Data *
plat_load_model_data(Mem_Pool *model_pool, char *file_name)
{
	/* [TODO]: Better error handling */
	FILE *f = fopen(file_name, "r");
	assert(f != NULL && "Unable to open model file (plat_load_model_data)");
	Str_View sv = slurp_file(f);

	Model_Data *model_data = deserialize_wavefront_obj_str(model_pool, sv);

	/* free resources */
	free(sv.str);
	fclose(f);

	return model_data;
}

int
main(void)
{
	/* init graphics */
	Graphics g = init_graphics();
	/* init audio */
	adev = init_audio();
	/* init game lib */
	void *game_lib_handle = link_to_game();
	if (game_lib_handle == NULL) {
		exit(1);
	}
	int game_lib_changed = 0;
	pthread_mutex_t *m = start_watching("bin", "game.so", &game_lib_changed);
	/* create game memory */
	Game_Memory game_memory = {
		.model_pool = mem_pool_new(1024 * 20),
		.entity_pool = mem_pool_new(1024 * 4),
		.temp = mem_pool_new(1024 * 4),
	};
	Model_Data *model_data = plat_load_model_data(game_memory.model_pool, "./test/cube.obj");
	/* Set initial game state */
	game_init(&game_memory, model_data);

	float refreash_rate = 60.0f;
	float frame_tt = 1.0f / refreash_rate;
	uint64_t frame_count = 0;
	uint64_t last_counter = plat_get_wall_clock();
	SDL_Event event;
	while (!quit) {
		pthread_mutex_lock(m);
		if (game_lib_changed) {
			SDL_PauseAudioDevice(adev, 1);
			pause_audio = 0;
			unlink_to_game(game_lib_handle);
			game_lib_handle = link_to_game();
			mem_pool_pop_all(game_memory.entity_pool);
			game_init(&game_memory, model_data);
			game_lib_changed = 0;
		}
		pthread_mutex_unlock(m);
		while (SDL_PollEvent(&event)) {
			if (event.type < SDL_USEREVENT) {
				if (event.type < SDLEVENTRANGE && handler[event.type]) {
					handler[event.type](&event);
				} /* else { events not handled by handler } */
			} /* else { handle user registered events } */
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

		void *pixels; 
		int32_t pitch;
		SDL_LockTexture(g.draw_buffer, NULL, &pixels, &pitch);
		game_update_and_render(pixels, g.w, g.h, elapsed_time);
		SDL_UnlockTexture(g.draw_buffer);
		SDL_RenderCopy(g.renderer, g.draw_buffer, NULL, NULL);
		SDL_RenderPresent(g.renderer);

	}
	unlink_to_game(game_lib_handle);
	SDL_PauseAudioDevice(adev, 1);
	SDL_CloseAudioDevice(adev);
	return 0;
}
