#ifndef WIN32_PLATFORM_H
#define WIN32_PLATFORM_H

#undef internal
#define WIN32_LEAN_AND_MEAN
#include <windows.h>
#define internal static

#include <timeapi.h>

#include <GL/gl.h>

typedef struct OS_State
{
	MemoryArena permanent_arena;

	char last_char;

	LARGE_INTEGER start_counter;
	F64 freq;

	S32 scroll;

	HCURSOR cursors[OS_Cursor_COUNT];

	B32 initialized;
} OS_State;

struct OS_Window
{
	void *handle;
	void *device_context;
	String8 class_name;
};

typedef struct OS_Library
{
	void *handle;
} OS_Library;

#endif
