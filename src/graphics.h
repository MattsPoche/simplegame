#ifndef GRAPHICS_H_
#define GRAPHICS_H_

#include "g_types.h"

static void clear_screen(uint32_t *draw_buffer, int width, int height);
static inline Vec3 scalev3(float a, Vec3 v);
static inline Vec3 addv3(Vec3 a, Vec3 b);
static inline Vec3 subv3(Vec3 a, Vec3 b);
static inline Vec4 mulm4_v4(Matrix4 m, Vec4 v);
static inline Vec3 mulm4_v3(Matrix4 m, Vec3 v);
static inline void swapv2(Vec2 *a, Vec2 *b);
static inline void swapf(float *a, float *b);
static inline Vec2 linear_interp_y(float y, Vec2 p0, Vec2 p1);
static inline Vec2 linear_interp_x(float x, Vec2 p0, Vec2 p1);
static void wireframe_tri2(uint32_t *draw_buffer, const int width, const int height, Tri2 tri);
static void raster_tri2(uint32_t *draw_buffer, const int width, const int height, Tri2 tri);
static Vec2 projection_map(int width, int height, Vec3 pa);
static void render_3d_model(uint32_t *draw_buffer,
							int width, int height,
							float elapsed_time,
							Tri3 *model,
							size_t tri_count);

#endif /* GRAPHICS_H_ */
