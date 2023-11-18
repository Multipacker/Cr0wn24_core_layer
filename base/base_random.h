#ifndef BASE_RANDOM_H
#define BASE_RANDOM_H

function U32
GetRandomU32()
{
	U32 result = rand();

	return result;
}

function U32
GetRandomU32Between(U32 min, U32 max)
{
	U32 result = min + (GetRandomU32() % (max - min));

	return result;
}

function F32
GetRandomF32Between(F32 min, F32 max)
{
	F32 result = min + ((F32)GetRandomU32() / (F32)RAND_MAX) * (max - min);
	return result;
}

#endif