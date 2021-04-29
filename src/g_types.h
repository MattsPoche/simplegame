#ifndef GRAPHICS_TYPES_H_
#define GRAPHICS_TYPES_H_

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

#define MODEL_NAME_MAX_LEN 24
typedef struct {
	size_t faces;	 			   /* count of faces */
	Tri3 tris[];      			   /* array of tris */
} Model_Data;

#endif /* GRAPHICS_TYPES_H_ */
