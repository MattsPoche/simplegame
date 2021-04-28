#include "graphics.h"

static void
clear_screen(uint32_t *draw_buffer, int width, int height)
{
	uint32_t clear_color = 0x0;
	for (int i = 0; i < width * height; ++i) {
		draw_buffer[i] = clear_color;
	}
}

static inline Vec3
scalev3(float a, Vec3 v)
{
	return (Vec3) {
		.x = v.x * a,
		.y = v.y * a,
		.z = v.z * a,
	};
}

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

static inline Vec3
mulm4_v3(Matrix4 m, Vec3 v)
{
	Vec4 r = {
		.x = v.x,
		.y = v.y,
		.z = v.z,
		.w = 1,
	};
	r = mulm4_v4(m, r);
	if (r.w != 0) {
		r.x /= r.w;
		r.y /= r.w;
		r.z /= r.w;
	}
	return (Vec3) {
		.x = r.x,
		.y = r.y,
		.z = r.z,
	};
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
linear_interp_y(float y, Vec2 p0, Vec2 p1) /* for a given y position, return the point on the line */
{
	float slope = 0.0f;
	if (p0.y != p1.y) {
		slope = (p1.x - p0.x) / (p1.y - p0.y);
	}
	float x = p0.x + ((y - p0.y) * (slope));
	return (Vec2){ .x = x, .y = y };
}

static inline Vec2
linear_interp_x(float x, Vec2 p0, Vec2 p1) /* for a given x position, return the point on the line */
{
	float slope = 1.0f;
	if (p0.x != p1.x) {
		slope = (p1.y - p0.y) / (p1.x - p0.x);
	}
	float y = p0.y + ((x - p0.x) * (slope));
	return (Vec2){ .x = x, .y = y };
}

static void 
wireframe_tri2(uint32_t *draw_buffer, const int width, const int height, Tri2 tri)
{
	/* [TODO]: Bug (wireframe_tri2, raster_tri2) -> invisible lines with zero slope
	 * [NOTE]: when the slope of a line aproaches zero, the x distance between the lines
	 * becomes greater until no line is drawn when slope = 0
	 */

	/* find highest point */
	Vec2 p0 = tri.v[0];
	Vec2 p1 = tri.v[1];
	Vec2 p2 = tri.v[2];
	if (p1.y < p0.y) { swapv2(&p0, &p1); }
	if (p2.y < p0.y) { swapv2(&p2, &p0); }
	if (p2.y < p1.y) { swapv2(&p2, &p1); }

	/* Draw Edges of face */
	float y;
	for (y = p0.y; y < p1.y; ++y) {
		Vec2 pa = linear_interp_y(y, p0, p1);
		Vec2 pb = linear_interp_y(y, p0, p2);
		if ((int)pa.y > -1 && (int)pa.y < height && (int)pa.x > -1 && (int)pa.x < width) {
			draw_buffer[((int)pa.y * width) + (int)pa.x] = 0xFFFFFFFF; /* white */
		}
		if ((int)pb.y > -1 && (int)pb.y < height && (int)pb.x > -1 && (int)pb.x < width) {
			draw_buffer[((int)pb.y * width) + (int)pb.x] = 0xFFFFFFFF; /* white */
		}
	}
	for (; y < p2.y; ++y) {
		Vec2 pa = linear_interp_y(y, p1, p2);
		Vec2 pb = linear_interp_y(y, p0, p2);
		if ((int)pa.y > -1 && (int)pa.y < height && (int)pa.x > -1 && (int)pa.x < width) {
			draw_buffer[((int)pa.y * width) + (int)pa.x] = 0xFFFFFFFF; /* white */
		}
		if ((int)pb.y > -1 && (int)pb.y < height && (int)pb.x > -1 && (int)pb.x < width) {
			draw_buffer[((int)pb.y * width) + (int)pb.x] = 0xFFFFFFFF; /* white */
		}
	}
}

static void 
raster_tri2(uint32_t *draw_buffer, const int width, const int height, Tri2 tri, uint32_t color)
{
	/* find highest point */
	Vec2 p0 = tri.v[0];
	Vec2 p1 = tri.v[1];
	Vec2 p2 = tri.v[2];
	if (p1.y < p0.y) { swapv2(&p0, &p1); }
	if (p2.y < p0.y) { swapv2(&p2, &p0); }
	if (p2.y < p1.y) { swapv2(&p2, &p1); }

	/* Draw Face */
	float y;
	for (y = p0.y; y < p1.y; ++y) {
		Vec2 pa = linear_interp_y(y, p0, p1);
		Vec2 pb = linear_interp_y(y, p0, p2);
		if (pa.x > pb.x) { swapv2(&pa, &pb); }
		for (float x = pa.x; x < pb.x; ++x) {
			Vec2 pc = linear_interp_x(x, pa, pb);
			if ((int)pc.y > -1 && (int)pc.y < height && (int)pc.x > -1 && (int)pc.x < width) {
				draw_buffer[((int)pc.y * width) + (int)pc.x] = color; 
			}
		}
	}
	for (; y < p2.y; ++y) {
		Vec2 pa = linear_interp_y(y, p1, p2);
		Vec2 pb = linear_interp_y(y, p0, p2);
		if (pa.x > pb.x) { swapv2(&pa, &pb); }
		for (float x = pa.x; x < pb.x; ++x) {
			Vec2 pc = linear_interp_x(x, pa, pb);
			if ((int)pc.y > -1 && (int)pc.y < height && (int)pc.x > -1 && (int)pc.x < width) {
				draw_buffer[((int)pc.y * width) + (int)pc.x] = color; 
			}
		}
	}
}

static Vec2
projection_map(int width, int height, Vec3 pa)
{
	float fov = 90.0f;
	float fov_rad = 1.0f / tanf((fov * PI / 180.0f) * 0.5f);
	float zn = 0.1; /* "z near" - distance of camera pinhole from draw surface */
	float zf = 1000.0f; /* "z far" */
	float ar = (float)height / (float)width; /* aspect ratio */
	Matrix4 proj_m = {
		.v = {
			[0] = { ar * fov_rad, 0,       0,                      0 },
			[1] = { 0,            fov_rad, 0,                      0 },
			[2] = { 0,            0,       zf / (zf - zn),         1 },
			[3] = { 0,            0,      -(zf * zn) / (zf - zn),  0 },
		}
	};
	Vec3 pa_projected = mulm4_v3(proj_m, pa);
	return (Vec2) { 
		.x = pa_projected.x, 
		.y = pa_projected.y,
	};
}

static void
render_3d_model(uint32_t *draw_buffer,
				int width, int height,
				float elapsed_time,
				Model_Data *model_data,
				Vec3 offset,
				float scalar,
				uint32_t color)
{
	static float theta = 0.0f;
	static const Vec3 camera = {0};
	theta += elapsed_time;
	Matrix4 rotz = {
		.v = {
			[0] = {  cosf(theta), sinf(theta), 0, 0},
			[1] = { -sinf(theta), cosf(theta), 0, 0},
			[2] = {  0,           0,           1, 0},
			[3] = {  0,           0,           0, 1},
		}
	};
	Matrix4 rotx = {
		.v = {
			[0] = { 1,  0,                  0,                  0 },
			[1] = { 0,  cosf(theta * 0.5f), sinf(theta * 0.5f), 0 },
			[2] = { 0, -sinf(theta * 0.5f), cosf(theta * 0.5f), 0 },
			[3] = { 0,  0,                  0,                  1 },
		}
	};

	for (size_t t = 0; t < model_data->faces; ++t) {
		Tri3 tri_scaled = {
			.v = {
				[0] = scalev3(scalar, model_data->tris[t].v[0]),
				[1] = scalev3(scalar, model_data->tris[t].v[1]),
				[2] = scalev3(scalar, model_data->tris[t].v[2]),
			}
		};
		Tri3 tri_rotatedz = {
			.v = {
				[0] = mulm4_v3(rotz, tri_scaled.v[0]),
				[1] = mulm4_v3(rotz, tri_scaled.v[1]),
				[2] = mulm4_v3(rotz, tri_scaled.v[2]),
			}
		};
		Tri3 tri_rotatedx = {
			.v = {
				[0] = mulm4_v3(rotx, tri_rotatedz.v[0]),
				[1] = mulm4_v3(rotx, tri_rotatedz.v[1]),
				[2] = mulm4_v3(rotx, tri_rotatedz.v[2]),
			}
		};
		Tri3 tri_offset = {
			.v = {
				[0] = addv3(tri_rotatedx.v[0], offset),
				[1] = addv3(tri_rotatedx.v[1], offset),
				[2] = addv3(tri_rotatedx.v[2], offset),
			}
		};

		Vec3 normal, edge1, edge2;
		edge1 = subv3(tri_offset.v[1], tri_offset.v[0]);
		edge2 = subv3(tri_offset.v[2], tri_offset.v[0]);
		/* cross product */
		normal.x = edge1.y * edge2.z - edge1.z * edge2.y;
		normal.y = edge1.z * edge2.x - edge1.x * edge2.z;
		normal.z = edge1.x * edge2.y - edge1.y * edge2.x;
		float l = sqrtf(normal.x * normal.x + normal.y * normal.y + normal.z * normal.z);
		normal.x /= l;
		normal.y /= l;
		normal.z /= l;

		if (normal.x * (tri_offset.v[0].x - camera.x) +
			normal.y * (tri_offset.v[0].y - camera.y) +
			normal.z * (tri_offset.v[0].z - camera.z) < 0.0f) {

			Tri2 tri_projected = {
				.v = {
					[0] = projection_map(width, height, tri_offset.v[0]),
					[1] = projection_map(width, height, tri_offset.v[1]),
					[2] = projection_map(width, height, tri_offset.v[2]),
				}
			};
			tri_projected.v[0].x += 1.0f; tri_projected.v[0].y += 1.0f;
			tri_projected.v[1].x += 1.0f; tri_projected.v[1].y += 1.0f;
			tri_projected.v[2].x += 1.0f; tri_projected.v[2].y += 1.0f;
			tri_projected.v[0].x *= 0.5f * (float)width; tri_projected.v[0].y *= 0.5f * (float)height;
			tri_projected.v[1].x *= 0.5f * (float)width; tri_projected.v[1].y *= 0.5f * (float)height;
			tri_projected.v[2].x *= 0.5f * (float)width; tri_projected.v[2].y *= 0.5f * (float)height;

			wireframe_tri2(draw_buffer, width, height, tri_projected);
			raster_tri2(draw_buffer, width, height, tri_projected, color);
		}
	}
}
