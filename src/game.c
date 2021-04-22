#include <assert.h>
#include <stdint.h>
#include <stdio.h>
#include <stdlib.h>
#include <math.h>

#include "game.h"
#include "const.h"

#define UNUSED_FUNC

typedef struct {
	float x;
	float y;
} Vec2;

typedef struct {
	Vec2 v[3];
} Tri2;

typedef struct {
	float x;
	float y;
	float z;
} Vec3;

typedef struct {
	Vec3 v[3];
} Matrix3;
typedef Matrix3 Tri3;

typedef struct {
	float x;
	float y;
	float z;
	float w;
} Vec4;

typedef struct {
	Vec4 v[4];
} Matrix4;

// static int xoffset = 0;
// static int yoffset = 0;
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

UNUSED_FUNC int
rand_range(int low, int high)
{
	return (rand() % (high - low + 1)) + low;
}

static void
clear_screen(uint32_t *draw_buffer, int width, int height)
{
	uint32_t clear_color = 0x0;
	for (int i = 0; i < width * height; ++i) {
		draw_buffer[i] = clear_color;
	}
}

#if 0
static Tri2 left_tri = {
	.v = {
		[0] = { 500.0f, 200.0f },
		[1] = { 300.0f, 500.0f },
		[2] = { 600.0f, 800.0f },
	},
};
static Tri2 right_tri = {
	.v = {
		[0] = { 800.0f, 200.0f },
		[1] = { 700.0f, 800.0f },
		[2] = { 1000.0f, 500.0f },
	},
};
static Tri2 flat_bottom_tri = {
	.v = {
		[0] = { 1500.0f, 200.0f },
		[1] = { 1500.0f, 500.0f },
		[2] = { 1700.0f, 500.0f },
	},
};
static Tri2 flat_top_tri = {
	.v = {
		[0] = { 1300.0f, 200.0f },
		[1] = { 1300.0f, 500.0f },
		[2] = { 1400.0f, 200.0f },
	},
};
#endif

static Vec3 cube_offset = {
	.x = 0.0f,
	.y = 0.0f,
	.z = 500.0f,
};
static Tri3 cube[12] = {
	/* face 1; front */
	[0]  = { .v = { { 0.0f, 0.0f, 0.0f }, { 50.0f, 0.0f, 0.0f }, { 0.0f, 50.0f, 0.0f } } },
	[1]  = { .v = { { 50.0f, 0.0f, 0.0f }, { 50.0f, 50.0f, 0.0f }, { 0.0f, 50.0f, 0.0f } } },
	/* face 2; back */
	[2]  = { .v = { { 0.0f, 0.0f, 50.0f }, { 50.0f, 0.0f, 50.0f }, { 0.0f, 50.0f, 50.0f } } },
	[3]  = { .v = { { 50.0f, 0.0f, 50.0f }, { 50.0f, 50.0f, 50.0f }, { 0.0f, 50.0f, 50.0f } } },
	/* face 3; right */
	[4]  = { .v = { { 50.0f, 0.0f, 0.0f }, { 50.0f, 0.0f, 50.0f }, { 50.0f, 50.0f, 0.0f } } },
	[5]  = { .v = { { 50.0f, 0.0f, 50.0f }, { 50.0f, 50.0f, 50.0f }, { 50.0f, 50.0f, 0.0f } } },
	/* face 4; left */
	[6]  = { .v = { { 0.0f, 0.0f, 50.0f }, { 0.0f, 0.0f, 0.0f }, { 0.0f, 50.0f, 50.0f } } },
	[7]  = { .v = { { 0.0f, 0.0f, 0.0f }, { 0.0f, 50.0f, 0.0f }, { 0.0f, 50.0f, 50.0f } } },
	/* face 5; up */
	[8]  = { .v = { { 0.0f, 0.0f, 50.0f }, { 50.0f, 0.0f, 50.0f }, { 0.0f, 0.0f, 0.0f } } },
	[9]  = { .v = { { 50.0f, 0.0f, 50.0f }, { 50.0f, 0.0f, 0.0f }, { 0.0f, 0.0f, 0.0f } } },
	/* face 6; down */
	[10] = { .v = { { 0.0f, 50.0f, 50.0f }, { 50.0f, 50.0f, 50.0f }, { 0.0f, 50.0f, 0.0f } } },
	[11] = { .v = { { 50.0f, 50.0f, 50.0f }, { 50.0f, 50.0f, 0.0f }, { 0.0f, 50.0f, 0.0f } } },
};

static inline Vec3
addv3(Vec3 a, Vec3 b)
{
	Vec3 r = (Vec3) {
		.x = a.x + b.x,
		.y = a.y + b.y,
		.z = a.z + b.z,
	};
	return r;
}

static inline Vec3
subv3(Vec3 a, Vec3 b)
{
	Vec3 r = (Vec3) {
		.x = a.x - b.x,
		.y = a.y - b.y,
		.z = a.z - b.z,
	};
	return r;
}

static inline Vec3
mulm3_v3(Matrix3 m, Vec3 v)
{
	Vec3 r = {
		.x = (m.v[0].x * v.x) + (m.v[1].x * v.x) + (m.v[2].x * v.x),
		.y = (m.v[0].y * v.y) + (m.v[1].y * v.y) + (m.v[2].y * v.y),
		.z = (m.v[0].z * v.z) + (m.v[1].z * v.z) + (m.v[2].z * v.z),
	};
	return r;
}

static inline Vec4
mulm4_v4(Matrix4 m, Vec4 v)
{
	Vec4 r = {
		.x = (m.v[0].x * v.x) + (m.v[1].x * v.y) + (m.v[2].x * v.z) + (m.v[3].x * v.w),
		.y = (m.v[0].y * v.x) + (m.v[1].y * v.y) + (m.v[2].y * v.z) + (m.v[3].x * v.w),
		.z = (m.v[0].z * v.x) + (m.v[1].z * v.y) + (m.v[2].z * v.z) + (m.v[3].x * v.w),
		.w = (m.v[0].w * v.x) + (m.v[1].w * v.y) + (m.v[2].w * v.z) + (m.v[3].x * v.w),
	};
	return r;
}

static inline void
swapv2(Vec2 *a, Vec2 *b)
{
	Vec2 tmp = *a;
	*a = *b;
	*b = tmp;
}

static inline void
swapf(float *a, float *b)
{
	float tmp = *a;
	*a = *b;
	*b = tmp;
}

/* linear interpolation
 * y(x) = y0 + (x - x0) * ((y1 - y0) / (x1 - x0))
 * */
static inline Vec2
linear_interp_y(int y, Vec2 p0, Vec2 p1)
{
	float slope = (p1.x - p0.x) / (p1.y - p0.y);
	float x = p0.x + (((float)y - p0.y) * (slope));
	return (Vec2){ .x = x, .y = (float)y };
}

static inline Vec2
linear_interp_x(int x, Vec2 p0, Vec2 p1)
{
	float slope = (p1.y - p0.y) / (p1.x - p0.x);
	float y = p0.y + (((float)x - p0.x) * (slope));
	return (Vec2){ .x = (float)x, .y = y };
}

static uint32_t color = 0;
static uint32_t colors[6] = {
	[0] = 0xFF << GSHIFT,
	[1] = 0xFF << RSHIFT,
	[2] = 0xFF << BSHIFT,
	[3] = 0xFF << GSHIFT | 0xFF << BSHIFT,
	[4] = 0xFF << RSHIFT | 0xFF << BSHIFT,
	[5] = 0xFF << RSHIFT | 0xFF << GSHIFT,
};

static void 
raster_tri2(uint32_t *draw_buffer, const int width, const int height, Tri2 tri)
{
	/* find highest point */
	Vec2 p0 = tri.v[0];
	Vec2 p1 = tri.v[1];
	Vec2 p2 = tri.v[2];
	if (p1.y < p0.y) { swapv2(&p0, &p1); }
	if (p2.y < p0.y) { swapv2(&p2, &p0); }
	if (p2.y < p1.y) { swapv2(&p2, &p1); }

	/* Draw Face */
	int y;
	for (y = (int)p0.y; y < (int)p1.y; ++y) {
		Vec2 pa = linear_interp_y(y, p0, p1);
		Vec2 pb = linear_interp_y(y, p0, p2);
		if (pa.x > pb.x) { swapv2(&pa, &pb); }
		for (int x = pa.x; x < pb.x; ++x) {
			Vec2 pc = linear_interp_x(x, pa, pb);
			if (pc.y > -1 && pc.y < height && pc.x > -1 && pc.x < width) {
				draw_buffer[((int)pc.y * width) + (int)pc.x] = colors[color]; 
			}
		}
	}
	for (; y < (int)p2.y; ++y) {
		Vec2 pa = linear_interp_y(y, p1, p2);
		Vec2 pb = linear_interp_y(y, p0, p2);
		if (pa.x > pb.x) { swapv2(&pa, &pb); }
		for (int x = pa.x; x < pb.x; ++x) {
			Vec2 pc = linear_interp_x(x, pa, pb);
			if (pc.y > -1 && pc.y < height && pc.x > -1 && pc.x < width) {
				draw_buffer[((int)pc.y * width) + (int)pc.x] = colors[color]; 
			}
		}
	}
}

static Vec2
projection_map(int width, int height, Vec3 pa)
{
	float fov = 90.0f;
	float fov_rad = 1.0f / tanf(fov * PI / 180.0f * 0.5f);
	float zn = 0.1; /* "z near" - distance of camera pinhole from draw surface */
	float zf = 1000.0f; /* "z far" */
	float ar = (float)width / (float)height; /* aspect ratio */
#if 0
	Matrix4 proj_m = {
		.v = {
			[0] = { ar * fov_rad, 0,       0,               0                     },
			[1] = { 0,            fov_rad, 0,               0                     },
			[2] = { 0,            0,       zf / (zf - zn), -(zf * zn) / (zf - zn) },
			[3] = { 0,            0,       1,               0                     },
		}
	};
#endif
	Matrix4 proj_m = {
		.v = {
			[0] = { ar * fov_rad, 0,       0,                      0                     },
			[1] = { 0,            fov_rad, 0,                      0                     },
			[2] = { 0,            0,       zf / (zf - zn),         1                     },
			[3] = { 0,            0,      -(zf * zn) / (zf - zn),  0                     },
		}
	};
	Vec4 pa_projected = {
		.x = pa.x,
		.y = pa.y,
		.z = pa.z,
		.w = 1.0f,
	};
	pa_projected = mulm4_v4(proj_m, pa_projected);
	if (pa_projected.w != 0) {
		pa.x = pa_projected.x / pa_projected.w;
		pa.y = pa_projected.y / pa_projected.w;
		pa.z = pa_projected.z / pa_projected.w;
	}
	return (Vec2) { 
		.x = pa.x, 
		.y = pa.y,
	};
}

static void
render_3d_model(uint32_t *draw_buffer, int width, int height, Tri3 *model, size_t tri_count)
{
	for (size_t t = 0; t < tri_count; ++t) {
		Tri3 triOffset = {
			.v = {
				[0] = addv3(model[t].v[0], cube_offset),
				[1] = addv3(model[t].v[1], cube_offset),
				[2] = addv3(model[t].v[2], cube_offset),
			}
		};
		Tri2 tri_projected = {
			.v = {
				[0] = projection_map(width, height, triOffset.v[0]),
				[1] = projection_map(width, height, triOffset.v[1]),
				[2] = projection_map(width, height, triOffset.v[2]),
			}
		};
		if (t % 2 == 0) {
			if ((++color) == 6)
				color = 0;
		}
		tri_projected.v[0].x += 1.0f; tri_projected.v[0].y += 1.0f;
		tri_projected.v[1].x += 1.0f; tri_projected.v[1].y += 1.0f;
		tri_projected.v[2].x += 1.0f; tri_projected.v[2].y += 1.0f;
		tri_projected.v[0].x *= 0.5f * (float)width; tri_projected.v[0].y *= 0.5f * (float)height;
		tri_projected.v[1].x *= 0.5f * (float)width; tri_projected.v[1].y *= 0.5f * (float)height;
		tri_projected.v[2].x *= 0.5f * (float)width; tri_projected.v[2].y *= 0.5f * (float)height;
		raster_tri2(draw_buffer, width, height, tri_projected);
	}
}

void
ga_update_and_render(uint32_t *draw_buffer, int width, int height)
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
	render_3d_model(draw_buffer, width, height, cube, 12);
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

