#ifndef BASE_MEMORY_H
#define BASE_MEMORY_H

#define CopyStruct(dst, src) 	    CopySize_((U8 *)dst, (U8 *)src, sizeof(*dst))
#define CopyArray(dst, src, count)  CopySize_((U8*)dst, (U8 *)src, sizeof(*dst) * count)

#define ZeroSize(memory, size) 	    ZeroSize_((U8 *)memory, size)
#define ZeroStruct(memory) 		    ZeroSize_((U8 *)memory, sizeof(*memory))
#define ZeroArray(memory, count) 	ZeroSize_((U8 *)memory, sizeof(*memory)*count)

#define PushSize(arena, size) 			        PushSizeZero_(arena, size)
#define PushStruct(arena, type) 		    (type *)PushSizeZero_(arena, sizeof(type))
#define PushArray(arena, count, type)      (type *)PushSizeZero_(arena, count * sizeof(type))

#define PushSizeNoZero(arena, size) 			        PushSize_(arena, size)
#define PushStructNoZero(arena, type) 		    (type *)PushSize_(arena, sizeof(type))
#define PushArrayNoZero(arena, count, type)      (type *)PushSize_(arena, count * sizeof(type))

#define PushStructCopy(arena, src) 								PushSizeCopy_(arena, (U8 *)src, sizeof(*src));

typedef struct MemoryArena
{
	U64 pos;
	U64 size;
	U8 *base;
} MemoryArena;

typedef struct TempMemoryArena
{
	MemoryArena *arena;
	U64 pos;
} TempMemoryArena;

#endif