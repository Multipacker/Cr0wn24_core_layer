#ifndef OS_INC_H
#define OS_INC_H

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
	OS_Key_MouseMiddle,
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

typedef struct OS_Window OS_Window;

internal void CoreInit();
internal void CoreShutdown(OS_Window *window);
internal void OS_Init(MemoryArena permanent_arena);

internal OS_Window *OS_CreateWindow(String8 title, S32 x, S32 y, S32 width, S32 height, B32 show_window);
internal void OS_DestroyWindow(OS_Window *window);
internal void OS_ShowWindow(OS_Window *window, B32 show);
internal void OS_ToggleFullscreen(OS_Window *window);
internal void OS_QueryWindowSize(OS_Window *window, S32 *width, S32 *height);
internal void OS_SwapBuffers(OS_Window *window);
internal Vec2F32 OS_GetMousePos(OS_Window *window);
internal void OS_SetHoverCursor(OS_Cursor type);

internal void OS_EatEvent(OS_EventList *event_list, OS_EventNode *node);
internal OS_EventList *OS_GatherEventsFromWindow(MemoryArena *arena);
internal char OS_GetLastChar();
internal S32 OS_GetScroll();
internal F64 OS_SecondsSinceAppStart();

internal OS_ReadFileResult OS_ReadEntireFile(String8 path);
internal void OS_CopyFile(String8 dst, String8 src);
internal Time OS_GetLastWriteTime(String8 file_name);

internal void OS_Sleep(U32 milliseconds);
internal Time OS_GetLocalTime();

internal void *OS_AllocMem(size_t size);
internal void OS_FreeMemory(void *memory);

internal OS_Library OS_LoadLibrary(String8 library_name);
internal void OS_FreeLibrary(OS_Library *library);
internal void *OS_LoadFunction(OS_Library library, String8 function_name);

#include "win32/os_win32_inc.h"

#endif
