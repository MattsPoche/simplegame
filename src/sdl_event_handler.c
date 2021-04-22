typedef struct {
	uint16_t mod;
	int sym;
	char *link_symbol;
	void (*game_action)(void);
} Key;

static void keydown(SDL_Event *event);
static void keyup(SDL_Event *event);
static void window_event(SDL_Event *event);
static void quit_event(SDL_Event *event);

#define SDLEVENTRANGE 0x2002 /* highest value of non-user SDL event types */

static Key downkeys[] = {
	{ 0, SDLK_q, NULL, plat_quit },
	{ 0, SDLK_b, NULL, plat_pause_audio },
	{ 0, SDLK_w, "ga_upbutton_down", NULL },
	{ 0, SDLK_s, "ga_downbutton_down", NULL },
	{ 0, SDLK_a, "ga_leftbutton_down", NULL },
	{ 0, SDLK_d, "ga_rightbutton_down", NULL },
	{ 0, SDLK_f, "ga_floatbutton_down", NULL },
	{ 0, SDLK_g, "ga_sinkbutton_down", NULL },
	{ 0, SDLK_v, "ga_inc_freq", NULL },
	{ 0, SDLK_c, "ga_dec_freq", NULL },
};

static Key upkeys[] = {
	{ 0, SDLK_w, "ga_upbutton_up", NULL },
	{ 0, SDLK_s, "ga_downbutton_up", NULL },
	{ 0, SDLK_a, "ga_leftbutton_up", NULL },
	{ 0, SDLK_d, "ga_rightbutton_up", NULL },
	{ 0, SDLK_f, "ga_floatbutton_up", NULL },
	{ 0, SDLK_g, "ga_sinkbutton_up", NULL },
};

static void (*handler[SDLEVENTRANGE]) (SDL_Event *) = {
	[SDL_KEYDOWN] = keydown,
	[SDL_KEYUP] = keyup,
	[SDL_WINDOWEVENT] = window_event,
	[SDL_QUIT] = quit_event,
/* SDL_MOUSEMOTION */
/* SDL_MOUSEBUTTONDOWN */
/* SDL_MOUSEBUTTONUP */
};

static void 
keydown(SDL_Event *event)
{
	for (size_t i = 0; i < ARRCOUNT(downkeys); ++i) {
		if (downkeys[i].game_action
				&& downkeys[i].mod == event->key.keysym.mod
				&& downkeys[i].sym == event->key.keysym.sym) {
			downkeys[i].game_action();
		}
	}
}

static void 
keyup(SDL_Event *event)
{
	for (size_t i = 0; i < ARRCOUNT(upkeys); ++i) {
		if (upkeys[i].game_action
				&& upkeys[i].mod == event->key.keysym.mod
				&& upkeys[i].sym == event->key.keysym.sym) {
			upkeys[i].game_action();
		}
	}
}

static void
window_event(SDL_Event *event)
{
	switch(event->window.event) {
		case SDL_WINDOWEVENT_CLOSE: {
			plat_quit();
		} break;
#if 0	
		case SDL_WINDOWEVENT_FOCUS_LOST: {
			SDL_PauseAudioDevice(adev, 1);
			pause_audio = 0;
		} break;
		case SDL_WINDOWEVENT_FOCUS_GAINED: {
			SDL_PauseAudioDevice(adev, 0);
			pause_audio = 1;
		} break;
#endif
	}
}

static void 
quit_event(SDL_Event *event)
{
	UNUSED(event);
	quit = 1;
}
