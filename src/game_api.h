#ifndef GAME_API_H_
#define GAME_API_H_

typedef void (*ga_update_and_render)(uint32_t *draw_buffer,
									 int width, int height,
									 float elapsed_time);
typedef void (*ga_update_sound)(void *audio_buffer, int frames);
typedef void (*ga_controller_input)(void);
typedef void (*ga_register_malloc)(void *(*malloc_cb)(size_t size));

#endif /* GAME_API_H_ */
