#pragma comment(lib, "gdi32.lib")
#pragma comment(lib, "user32.lib")
#pragma comment(lib, "kernel32.lib")

global OS_State os_state;

function void *OS_AllocMem(size_t size);

MemoryArena core_memory_arena = {0};
function void CoreInit()
{
	srand((U32)time(0));

	ArenaInit(scratch_arena_ + 0, OS_AllocMem(GIGABYTES(1)), GIGABYTES(1));
	ArenaInit(scratch_arena_ + 1, OS_AllocMem(GIGABYTES(1)), GIGABYTES(1));

	size_t core_memory_storage_size = MEGABYTES(512);
	void *core_memory = OS_AllocMem(core_memory_storage_size);

	ArenaInit(&core_memory_arena, core_memory, core_memory_storage_size);

	MemoryArena os_permanent_arena;
	ArenaInit(&os_permanent_arena, PushSizeNoZero(&core_memory_arena, 1024 * 1024), 1024 * 1024);

	OS_Init(os_permanent_arena);
}

function void CoreShutdown(OS_Window *window)
{
	OS_DestroyWindow(window);
}

function void OS_Init(MemoryArena permanent_arena)
{
	os_state.permanent_arena = permanent_arena;

	QueryPerformanceCounter(&os_state.start_counter);

	LARGE_INTEGER freq_counter;
	QueryPerformanceFrequency(&freq_counter);
	os_state.freq = (F64)freq_counter.QuadPart;

	os_state.cursors[OS_Cursor_Arrow] = LoadCursorA(0, IDC_ARROW);
	Assert(os_state.cursors[OS_Cursor_Arrow]);

	os_state.cursors[OS_Cursor_Hand] = LoadCursorA(0, IDC_HAND);
	Assert(os_state.cursors[OS_Cursor_Hand]);

	os_state.cursors[OS_Cursor_ResizeY] = LoadCursorA(0, IDC_SIZENS);
	Assert(os_state.cursors[OS_Cursor_ResizeY]);

	os_state.cursors[OS_Cursor_ResizeX] = LoadCursorA(0, IDC_SIZEWE);
	Assert(os_state.cursors[OS_Cursor_ResizeX]);

	os_state.cursors[OS_Cursor_ResizeXY] = LoadCursorA(0, IDC_SIZENWSE);
	Assert(os_state.cursors[OS_Cursor_ResizeXY]);

	os_state.initialized = true;
}

function LRESULT CALLBACK OS_WindowProc(HWND window, UINT message, WPARAM wparam, LPARAM lparam)
{
	LRESULT result = 0;
	switch(message)
	{
		case WM_CLOSE:
		{
			PostQuitMessage(0);
		} break;

		case WM_DESTROY:
		{
			PostQuitMessage(0);
		} break;

		case WM_SETCURSOR:
		{
		} break;

		case WM_CHAR:
		{
			char key = (char)wparam;
			os_state.last_char = key;
		} break;

		case WM_SIZE:
		{
			S32 width, height;
			RECT rect;
			GetClientRect(window, &rect);
			width = rect.right - rect.left;
			height = rect.bottom - rect.top;
			// glViewport(0, 0, width, height);
		} break;

		default:
		{
			result = DefWindowProcA(window, message, wparam, lparam);
		} break;
	}

	return result;
}

function OS_Window *
OS_CreateWindow(String8 title, S32 x, S32 y, S32 width, S32 height, B32 show_window)
{
	timeBeginPeriod(0);

	OS_Window *window = VirtualAlloc(0, sizeof(*window), MEM_COMMIT | MEM_RESERVE, PAGE_READWRITE);

	Assert(window && "Failed to allocate the window");

	HINSTANCE instance = GetModuleHandleA(0);

	String8 class_name = Str8Lit("OSWindowClassName");
	window->class_name = class_name;

	WNDCLASSEXA window_class = {0};

	window_class.cbSize = sizeof(window_class);
	window_class.style = CS_HREDRAW | CS_VREDRAW | CS_OWNDC;
	window_class.lpfnWndProc = OS_WindowProc;
	window_class.hInstance = instance;
	window_class.lpszClassName = (LPCSTR)class_name.str;
	window_class.hCursor = LoadCursorA(0, IDC_ARROW);
	if(!RegisterClassExA(&window_class))
	{
		Assert(false);
	}

	SetCursor(LoadCursorA(0, IDC_ARROW));

	DWORD create_window_flags = WS_OVERLAPPEDWINDOW;
	if(show_window)
	{
		create_window_flags |= WS_VISIBLE;
	}

	// TODO(hampus): Make so that this actually 
	// uses the width and height parameters
	window->handle = CreateWindowExA(0, window_class.lpszClassName, (LPCSTR)title.str,
																	 create_window_flags,
																	 CW_USEDEFAULT, CW_USEDEFAULT,
																	 CW_USEDEFAULT, CW_USEDEFAULT,
																	 0, 0, instance, 0);

	window->device_context = GetDC(window->handle);

	return window;
}

function void
OS_ShowWindow(OS_Window *window, B32 show)
{
	ShowWindow(window->handle, show);
}

function void
OS_DestroyWindow(OS_Window *window)
{
	ReleaseDC(window->handle, window->device_context);
	DestroyWindow(window->handle);
	UnregisterClassA((LPCSTR)window->class_name.str, GetModuleHandle(0));
}

function void
OS_ToggleFullscreen(OS_Window *window)
{
	local_persist WINDOWPLACEMENT g_wpPrev = {sizeof(g_wpPrev)};
	DWORD WindowStyle = GetWindowLong(window->handle, GWL_STYLE);
	if(WindowStyle & WS_OVERLAPPEDWINDOW)
	{
		MONITORINFO MonitorInfo = {sizeof(MonitorInfo)};
		if(GetWindowPlacement(window->handle, &g_wpPrev) &&
			 GetMonitorInfo(MonitorFromWindow(window->handle, MONITOR_DEFAULTTOPRIMARY), &MonitorInfo))
		{
			SetWindowLong(window->handle, GWL_STYLE, WindowStyle & ~WS_OVERLAPPEDWINDOW);

			SetWindowPos(window->handle, HWND_TOP,
									 MonitorInfo.rcMonitor.left, MonitorInfo.rcMonitor.top,
									 MonitorInfo.rcMonitor.right - MonitorInfo.rcMonitor.left,
									 MonitorInfo.rcMonitor.bottom - MonitorInfo.rcMonitor.top,
									 SWP_NOOWNERZORDER | SWP_FRAMECHANGED);
		}
	}
	else
	{
		SetWindowLong(window->handle, GWL_STYLE, WindowStyle | WS_OVERLAPPEDWINDOW);
		SetWindowPlacement(window->handle, &g_wpPrev);
		SetWindowPos(window->handle, NULL, 0, 0, 0, 0,
								 SWP_NOMOVE | SWP_NOSIZE | SWP_NOZORDER |
								 SWP_NOOWNERZORDER | SWP_FRAMECHANGED);
	}
}

function void OS_EatEvent(OS_EventList *event_list, OS_EventNode *node)
{
	if(node == event_list->first)
	{
		event_list->first = event_list->first->next;
	}
	else
	{
		if(node->prev)
		{
			node->prev->next = node->next;
		}

		if(node->next)
		{
			node->next->prev = node->prev;
		}
	}
}

function OS_EventList *
OS_GatherEventsFromWindow(MemoryArena *arena)
{
	os_state.last_char = 0;

	OS_EventList *event_list = PushStruct(arena, OS_EventList);

	os_state.scroll = 0;

	MSG message;
	while(PeekMessageA(&message, 0, 0, 0, PM_REMOVE))
	{
		OS_Event event = {0};

		B32 interesting_message = true;

		switch(message.message)
		{
			case WM_QUIT:
			{
				event.type = OS_EventType_Quit;
			} break;

			case WM_SIZE:
			{
				event.type = OS_EventType_Resize;
			} break;

			case WM_LBUTTONUP:
			{
				event.type = OS_EventType_KeyRelease;
				event.key = OS_Key_MouseLeft;
			} break;

			case WM_LBUTTONDOWN:
			{
				event.type = OS_EventType_KeyPress;
				event.key = OS_Key_MouseLeft;
			} break;

			case WM_RBUTTONUP:
			{
				event.type = OS_EventType_KeyRelease;
				event.key = OS_Key_MouseRight;
			} break;

			case WM_RBUTTONDOWN:
			{
				event.type = OS_EventType_KeyPress;
				event.key = OS_Key_MouseRight;
			} break;

			case WM_MOUSEWHEEL:
			{
				os_state.scroll = GET_WHEEL_DELTA_WPARAM(message.wParam) / WHEEL_DELTA;
			} break;

			case WM_KEYUP:
			case WM_SYSKEYUP:
			case WM_SYSKEYDOWN:
			case WM_KEYDOWN:
			{
				U32 vk_code = (U32)message.wParam;
				B32 was_down = ((message.lParam & (1 << 30)) != 0);
				B32 is_down = ((message.lParam & (1 << 31)) == 0);
				B32 alt_key_was_down = ((message.lParam & (1 << 29)));

				if(was_down != is_down)
				{
					if(is_down)
					{
						event.type = OS_EventType_KeyPress;
					}
					else
					{
						event.type = OS_EventType_KeyRelease;
					}

#define ProcessKeyMessage(vk, platform_key) case vk: { event.key = platform_key; } break;
					switch(vk_code)
					{
						ProcessKeyMessage('A', OS_Key_A);
						ProcessKeyMessage('B', OS_Key_B);
						ProcessKeyMessage('C', OS_Key_C);
						ProcessKeyMessage('D', OS_Key_D);
						ProcessKeyMessage('E', OS_Key_E);
						ProcessKeyMessage('F', OS_Key_F);
						ProcessKeyMessage('G', OS_Key_G);
						ProcessKeyMessage('H', OS_Key_H);
						ProcessKeyMessage('I', OS_Key_I);
						ProcessKeyMessage('J', OS_Key_J);
						ProcessKeyMessage('K', OS_Key_K);
						ProcessKeyMessage('L', OS_Key_L);
						ProcessKeyMessage('M', OS_Key_M);
						ProcessKeyMessage('N', OS_Key_N);
						ProcessKeyMessage('O', OS_Key_O);
						ProcessKeyMessage('P', OS_Key_P);
						ProcessKeyMessage('Q', OS_Key_Q);
						ProcessKeyMessage('R', OS_Key_R);
						ProcessKeyMessage('S', OS_Key_S);
						ProcessKeyMessage('T', OS_Key_T);
						ProcessKeyMessage('U', OS_Key_U);
						ProcessKeyMessage('V', OS_Key_V);
						ProcessKeyMessage('W', OS_Key_W);
						ProcessKeyMessage('X', OS_Key_X);
						ProcessKeyMessage('Y', OS_Key_Y);
						ProcessKeyMessage('Z', OS_Key_Z);

						ProcessKeyMessage(VK_F1, OS_Key_F1);
						ProcessKeyMessage(VK_F2, OS_Key_F2);
						ProcessKeyMessage(VK_F3, OS_Key_F3);
						ProcessKeyMessage(VK_F4, OS_Key_F4);
						ProcessKeyMessage(VK_F5, OS_Key_F5);
						ProcessKeyMessage(VK_F6, OS_Key_F6);
						ProcessKeyMessage(VK_F7, OS_Key_F7);
						ProcessKeyMessage(VK_F8, OS_Key_F8);
						ProcessKeyMessage(VK_F9, OS_Key_F9);
						ProcessKeyMessage(VK_F10, OS_Key_F10);
						ProcessKeyMessage(VK_F11, OS_Key_F11);
						ProcessKeyMessage(VK_F12, OS_Key_F12);

						ProcessKeyMessage(VK_INSERT, OS_Key_Insert);
						ProcessKeyMessage(VK_RETURN, OS_Key_Return);
						ProcessKeyMessage(VK_ESCAPE, OS_Key_Escape);

						ProcessKeyMessage(VK_PRIOR, OS_Key_PageUp);
						ProcessKeyMessage(VK_NEXT, OS_Key_PageDown);
					}
				}

				if(vk_code == VK_F4 && alt_key_was_down)
				{
					event.type = OS_EventType_Quit;
				}
				TranslateMessage(&message);

			} break;

			default:
			{
				interesting_message = false;
				TranslateMessage(&message);
				DispatchMessageA(&message);
			} break;
		}

		if(interesting_message)
		{
			OS_EventNode *node = PushStruct(arena, OS_EventNode);
			node->event = event;
			DLL_PushFront(event_list->first, event_list->last, node);
		}
	}

	return event_list;
}

function void OS_QueryWindowSize(OS_Window *window, U32 *width, U32 *height)
{
	RECT rect;
	GetClientRect(window->handle, &rect);
	*width = rect.right - rect.left;
	*height = rect.bottom - rect.top;
}

function void OS_SwapBuffers(OS_Window *window)
{
	SwapBuffers(window->device_context);
}

function char OS_GetLastChar()
{
	return os_state.last_char;
}

function S32 OS_GetScroll()
{
	return os_state.scroll;
}

function OS_ReadFileResult OS_ReadEntireFile(String8 path)
{
	OS_ReadFileResult result = {0};

	HANDLE file = CreateFileA((char *)path.str, GENERIC_READ, FILE_SHARE_READ, 0, OPEN_EXISTING, FILE_ATTRIBUTE_NORMAL, 0);

	if(file == INVALID_HANDLE_VALUE)
	{
		// TODO(hampus):  Logging
		return result;
	}

	LARGE_INTEGER file_size;
	if(!GetFileSizeEx(file, &file_size))
	{
		// TODO(hampus):  Logging
		return result;
	}

	Assert(file_size.QuadPart <= 0xffffffff);

	U32 file_size32 = (U32)file_size.QuadPart;

	result.content = VirtualAlloc(0, file_size32, MEM_COMMIT | MEM_RESERVE, PAGE_READWRITE);
	result.contents_size = file_size32;
	Assert(result.content);

	DWORD bytes_read;
	if(!ReadFile(file, result.content, file_size32, &bytes_read, 0))
	{
		// TODO(hampus):  Logging
		VirtualFree(result.content, 0, MEM_RELEASE);
		return result;
	}

	Assert(bytes_read == file_size32);

	CloseHandle(file);

	return result;
}

function F64 OS_SecondsSinceAppStart()
{
	LARGE_INTEGER counter = {0};
	QueryPerformanceCounter(&counter);
	return((F64)counter.QuadPart - (F64)os_state.start_counter.QuadPart) / os_state.freq;
}

function void *OS_AllocMem(size_t size)
{
	void *result = 0;

	result = VirtualAlloc(0, size, MEM_COMMIT | MEM_RESERVE, PAGE_READWRITE);

	return result;
}

function void OS_FreeMemory(void *memory)
{
	if(memory)
	{
		VirtualFree(memory, 0, MEM_DECOMMIT);
	}
}

function OS_Library OS_LoadLibrary(String8 library_name)
{
	OS_Library result = {0};

	result.handle = LoadLibraryA((LPCSTR)library_name.str);

	return(result);
}

function void OS_FreeLibrary(OS_Library *library)
{
	FreeLibrary(library->handle);
	library->handle = 0;
}

function void *OS_LoadFunction(OS_Library library, String8 function_name)
{
	void *result = 0;
	result = GetProcAddress((HMODULE)library.handle, (LPCSTR)function_name.str);
	return result;
}

// TODO(hampus): Move this onto the base layer!
function B32 SameTime(Time *a, Time *b)
{
	return(a->year == b->year &&
				 a->month == b->month &&
				 a->day == b->day &&
				 a->hour == b->hour &&
				 a->minute == b->minute &&
				 a->second == b->second);
}

function void OS_CopyFile(String8 dst, String8 src)
{
	CopyFileA((LPCSTR)src.str, (LPCSTR)dst.str, FALSE);
}

function Time OS_SystemTimeToTime(SYSTEMTIME *time)
{
	Time result;

	result.year = time->wYear;
	result.month = time->wMonth;
	result.day = time->wDay;

	result.hour = time->wHour;
	result.minute = time->wMinute;
	result.second = time->wSecond;

	result.millisecond = time->wMilliseconds;
	return(result);
}

function Time OS_GetLastWriteTime(String8 file_name)
{
	Time result = {0};

	WIN32_FILE_ATTRIBUTE_DATA data = {0};
	if(GetFileAttributesExA((LPCSTR)file_name.str, GetFileExInfoStandard, &data))
	{
		FILETIME last_write_time = data.ftLastWriteTime;
		SYSTEMTIME time;
		if(FileTimeToSystemTime(&last_write_time, &time))
		{
			result = OS_SystemTimeToTime(&time);
		}
		else
		{
			Assert(false);
		}
	}

	return(result);
}

function void OS_Sleep(U32 milliseconds)
{
	Sleep(milliseconds);
}

function Time OS_GetLocalTime()
{
	Time result;
	SYSTEMTIME time;
	GetLocalTime(&time);
	result = OS_SystemTimeToTime(&time);
	return(result);
}

function Vec2F32 OS_GetMousePos(OS_Window *window)
{
	POINT point;
	GetCursorPos(&point);
	ScreenToClient(window->handle, &point);
	Vec2F32 result;
	result.x = (F32)point.x;
	result.y = (F32)point.y;
	return(result);
}

function void OS_SetHoverCursor(OS_Cursor type)
{
	SetCursor(os_state.cursors[type]);
}