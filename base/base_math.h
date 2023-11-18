#ifndef MATH_H
#define MATH_H

#include <math.h>

#define Min(x, y) ((x) < (y) ? (x) : (y))
#define Max(x, y) ((x) > (y) ? (x) : (y))
#define Clamp(a, val, b) (Max(a, Min(val, b)))
#define Abs(x) ((x) < (0) ? (-(x)) : (x))

#define Square(x) ((x)*(x))

#define Pi64 3.14159265358979323846
#define Pi32 3.141592653589793f

typedef union Vec2F32
{
	struct
	{
		F32 x, y;
	};
	F32 m[2];
} Vec2F32;

typedef union Vec2U32
{
	struct
	{
		U32 x, y;
	};
} Vec2U32;

typedef union Vec2S32
{
	struct
	{
		S32 x, y;
	};

	struct
	{
		S32 width, height;
	};
} Vec2S32;

typedef union Vec3F32
{
	struct
	{
		F32 x, y, z;
	};

	struct
	{
		F32 r, g, b;
	};
} Vec3F32;

typedef union Vec4F32
{
	struct
	{
		F32 x, y, z, w;
	};

	struct
	{
		F32 r, g, b, a;
	};
	F32 m[4];
} Vec4F32;

typedef struct Mat4x4F32
{
	F32 m[4][4];
} Mat4x4F32;

#endif