internal Vec2F32 V2(F32 x, F32 y)
{
	Vec2F32 result;
	result.x = x;
	result.y = y;
	return result;
}

internal Vec2F32 V2MulF32(Vec2F32 vec, F32 f)
{
	Vec2F32 result;

	result.x = vec.x * f;
	result.y = vec.y * f;

	return result;
}

internal Vec2F32 V2DivF32(Vec2F32 vec, F32 f)
{
	Vec2F32 result;

	result.x = vec.x / f;
	result.y = vec.y / f;

	return result;
}

internal Vec2F32 V2AddV2(Vec2F32 vec0, Vec2F32 vec1)
{
	Vec2F32 result;

	result.x = vec0.x + vec1.x;
	result.y = vec0.y + vec1.y;

	return result;
}

internal Vec2F32 V2SubV2(Vec2F32 vec0, Vec2F32 vec1)
{
	Vec2F32 result;

	result.x = vec0.x - vec1.x;
	result.y = vec0.y - vec1.y;

	return result;
}
internal Vec2S32 V2S(S32 x, S32 y)
{
	Vec2S32 result;
	result.x = x;
	result.y = y;
	return result;
}

internal Vec3F32 V3(F32 x, F32 y, F32 z)
{
	Vec3F32 result;
	result.x = x;
	result.y = y;
	result.z = z;
	return result;
}

internal Vec4F32 V4(F32 x, F32 y, F32 z, F32 w)
{
	Vec4F32 result;
	result.x = x;
	result.y = y;
	result.z = z;
	result.w = w;
	return result;
}

internal Vec4F32 V4MulF32(Vec4F32 vec, F32 f)
{
	Vec4F32 result;
	result.r = vec.r * f;
	result.g = vec.g * f;
	result.b = vec.b * f;
	result.a = vec.a * f;
	return result;
}

internal Vec4F32 V4Lerp(Vec4F32 a, Vec4F32 b, F32 t)
{
	Vec4F32 result;
	result.r = a.r * (1.0f - t) + b.r * t;
	result.g = a.g * (1.0f - t) + b.g * t;
	result.b = a.b * (1.0f - t) + b.b * t;
	result.a = a.a * (1.0f - t) + b.a * t;
	return result;
}

internal Mat4x4F32 Ortho(F32 left, F32 right, F32 bottom, F32 top, F32 _near, F32 _far)
{
	Mat4x4F32 result;

	result.m[0][0] = 2 / (right - left);
	result.m[0][1] = 0;
	result.m[0][2] = 0;
	result.m[0][3] = 0;

	result.m[1][0] = 0;
	result.m[1][1] = 2 / (top - bottom);
	result.m[1][2] = 0;
	result.m[1][3] = 0;

	result.m[2][0] = 0;
	result.m[2][1] = 0;
	result.m[2][2] = 1 / (_near - _far);
	result.m[2][3] = 0;

	result.m[3][0] = (left + right) / (left - right);
	result.m[3][1] = (top + bottom) / (bottom - top);
	result.m[3][2] = _near / (_near - _far);
	result.m[3][3] = 1;

	return result;
}

internal F32 Radians(F32 degrees)
{
	F32 result = degrees;

	result *= Pi32 / 180;

	return result;
}

internal Mat4x4F32 M4(F32 d)
{
	Mat4x4F32 result = {0};

	result.m[0][0] = d;
	result.m[1][1] = d;
	result.m[2][2] = d;
	result.m[3][3] = d;

	return result;
}

internal Mat4x4F32 M4MulM4(Mat4x4F32 a, Mat4x4F32 b)
{
	Mat4x4F32 result = {
		a.m[0][0] * b.m[0][0] + a.m[0][1] * b.m[1][0] + a.m[0][2] * b.m[2][0] + a.m[0][3] * b.m[3][0],
		a.m[0][0] * b.m[0][1] + a.m[0][1] * b.m[1][1] + a.m[0][2] * b.m[2][1] + a.m[0][3] * b.m[3][1],
		a.m[0][0] * b.m[0][2] + a.m[0][1] * b.m[1][2] + a.m[0][2] * b.m[2][2] + a.m[0][3] * b.m[3][2],
		a.m[0][0] * b.m[0][3] + a.m[0][1] * b.m[1][3] + a.m[0][2] * b.m[2][3] + a.m[0][3] * b.m[3][3],

		a.m[1][0] * b.m[0][0] + a.m[1][1] * b.m[1][0] + a.m[1][2] * b.m[2][0] + a.m[1][3] * b.m[3][0],
		a.m[1][0] * b.m[0][1] + a.m[1][1] * b.m[1][1] + a.m[1][2] * b.m[2][1] + a.m[1][3] * b.m[3][1],
		a.m[1][0] * b.m[0][2] + a.m[1][1] * b.m[1][2] + a.m[1][2] * b.m[2][2] + a.m[1][3] * b.m[3][2],
		a.m[1][0] * b.m[0][3] + a.m[1][1] * b.m[1][3] + a.m[1][2] * b.m[2][3] + a.m[1][3] * b.m[3][3],

		a.m[2][0] * b.m[0][0] + a.m[2][1] * b.m[1][0] + a.m[2][2] * b.m[2][0] + a.m[2][3] * b.m[3][0],
		a.m[2][0] * b.m[0][1] + a.m[2][1] * b.m[1][1] + a.m[2][2] * b.m[2][1] + a.m[2][3] * b.m[3][1],
		a.m[2][0] * b.m[0][2] + a.m[2][1] * b.m[1][2] + a.m[2][2] * b.m[2][2] + a.m[2][3] * b.m[3][2],
		a.m[2][0] * b.m[0][3] + a.m[2][1] * b.m[1][3] + a.m[2][2] * b.m[2][3] + a.m[2][3] * b.m[3][3],

		a.m[3][0] * b.m[0][0] + a.m[3][1] * b.m[1][0] + a.m[3][2] * b.m[2][0] + a.m[3][3] * b.m[3][0],
		a.m[3][0] * b.m[0][1] + a.m[3][1] * b.m[1][1] + a.m[3][2] * b.m[2][1] + a.m[3][3] * b.m[3][1],
		a.m[3][0] * b.m[0][2] + a.m[3][1] * b.m[1][2] + a.m[3][2] * b.m[2][2] + a.m[3][3] * b.m[3][2],
		a.m[3][0] * b.m[0][3] + a.m[3][1] * b.m[1][3] + a.m[3][2] * b.m[2][3] + a.m[3][3] * b.m[3][3]
	};
	return result;
}

internal Mat4x4F32 Translate(Mat4x4F32 mat, Vec3F32 trans)
{
	Mat4x4F32 result = M4(1.0f);

	result.m[0][3] = trans.x;
	result.m[1][3] = trans.y;
	result.m[2][3] = trans.z;

	result = M4MulM4(result, mat);

	return result;
}

internal Mat4x4F32 Scale(Mat4x4F32 mat, Vec3F32 scale)
{
	Mat4x4F32 result = M4(1.0f);

	result.m[0][0] = scale.x;
	result.m[1][1] = scale.y;
	result.m[2][2] = scale.z;

	result = M4MulM4(result, mat);

	return result;
}

internal F32 Smoothstep_F32(F32 edge0, F32 edge1, F32 x)
{
	// scale, and clamp x to 0..1 range
	x = Clamp(0, (x - edge0) / (edge1 - edge0), 1.0f);

	return x * x * (3.0f - 2.0f * x);
}