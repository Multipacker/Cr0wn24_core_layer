#ifndef WIN32_PLATFORM_H
#define WIN32_PLATFORM_H

#undef internal
#define WIN32_LEAN_AND_MEAN
#include <windows.h>
#define internal static

#include <timeapi.h>

#include <GL/gl.h>

typedef enum OS_EventType
{
	OS_EventType_None,
	OS_EventType_Quit,
	OS_EventType_Resize,
	OS_EventType_KeyPress,
	OS_EventType_KeyRelease,

	OS_EventType_COUNT,
} OS_EventType;

typedef enum OS_Key
{
	OS_Key_A,
	OS_Key_B,
	OS_Key_C,
	OS_Key_D,
	OS_Key_E,
	OS_Key_F,
	OS_Key_G,
	OS_Key_H,
	OS_Key_I,
	OS_Key_J,
	OS_Key_K,
	OS_Key_L,
	OS_Key_M,
	OS_Key_N,
	OS_Key_O,
	OS_Key_P,
	OS_Key_Q,
	OS_Key_R,
	OS_Key_S,
	OS_Key_T,
	OS_Key_U,
	OS_Key_V,
	OS_Key_W,
	OS_Key_X,
	OS_Key_Y,
	OS_Key_Z,

	OS_Key_F1,
	OS_Key_F2,
	OS_Key_F3,
	OS_Key_F4,
	OS_Key_F5,
	OS_Key_F6,
	OS_Key_F7,
	OS_Key_F8,
	OS_Key_F9,
	OS_Key_F10,
	OS_Key_F11,
	OS_Key_F12,

	OS_Key_MouseLeft,
	OS_Key_MouseRight,

	OS_Key_Return,
	OS_Key_Insert,
	OS_Key_Escape,

	OS_Key_PageUp,
	OS_Key_PageDown,

	OS_Key_COUNT,
} OS_Key;

typedef enum OS_Cursor
{
	OS_Cursor_Arrow,
	OS_Cursor_Hand,
	OS_Cursor_ResizeY,
	OS_Cursor_ResizeX,
	OS_Cursor_ResizeXY,

	OS_Cursor_COUNT,
} OS_Cursor;

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

typedef struct OS_Window
{
	void *handle;
	void *device_context;
	String8 class_name;
} OS_Window;

typedef struct OS_Event
{
	OS_EventType type;

	OS_Key key;
} OS_Event;

typedef struct OS_EventNode
{
	struct OS_EventNode *next;
	struct OS_EventNode *prev;
	OS_Event event;
} OS_EventNode;

typedef struct OS_EventList
{
	OS_EventNode *first;
	OS_EventNode *last;
} OS_EventList;

typedef struct OS_ReadFileResult
{
	U32 contents_size;
	void *content;
} OS_ReadFileResult;

typedef struct OS_Library
{
	void *handle;
} OS_Library;

internal void OS_Init(MemoryArena permanent_arena);
internal void OS_DestroyWindow(OS_Window *window);

#endif
