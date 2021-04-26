#ifndef GAME_H_
#define GAME_H_

typedef void *(*malloc_cb)(size_t size);

void ga_update_and_render(uint32_t *draw_buffer, int width, int height, float elapsed_time);
void ga_update_sound(void *audio_buffer, int frames);
void ga_register_malloc(malloc_cb f);
void ga_upbutton_down(void);
void ga_upbutton_up(void);
void ga_downbutton_down(void);
void ga_downbutton_up(void);
void ga_leftbutton_down(void);
void ga_leftbutton_up(void);
void ga_rightbutton_down(void);
void ga_rightbutton_up(void);
void ga_inc_freq(void);
void ga_dec_freq(void);

#define KEY_PRESSED 1
#define KEY_RELEASED 0

static malloc_cb ga_malloc;

#endif /* GAME_H_ */
