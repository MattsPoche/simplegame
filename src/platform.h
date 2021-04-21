#ifndef PLATFORM_H_
#define PLATFORM_H_

extern void plat_quit(void);
extern void plat_pause_audio(void);
extern uint64_t plat_get_wall_clock(void);
extern float plat_get_s_elapsed(uint64_t start, uint64_t end);
extern int plat_msleep(int64_t msec);

#endif /* PLATFORM_H_ */
