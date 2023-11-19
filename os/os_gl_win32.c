#pragma comment(lib, "opengl32.lib")

#define X(type, name) static type name;
GL_internalS(X)
#undef X

global PFNWGLCHOOSEPIXELFORMATARBPROC wglChoosePixelFormatARB;
global PFNWGLCREATECONTEXTATTRIBSARBPROC wglCreateContextAttribsARB;
global PFNWGLSwapINTERVALEXTPROC wglSwapIntervalEXT;

internal void FatalError(char *message)
{
	MessageBoxA(NULL, message, "Error", MB_ICONEXCLAMATION);
	ExitProcess(0);
}

internal void APIENTRY OS_GL_DebugCallback(GLenum source, GLenum type, GLuint id, GLenum severity,
																					 GLsizei length, const GLchar* message, const void* user)
{
	OutputDebugStringA(message);
	OutputDebugStringA("\n");
	if(severity == GL_DEBUG_SEVERITY_HIGH || severity == GL_DEBUG_SEVERITY_MEDIUM)
	{
		if(IsDebuggerPresent())
		{
			Assert(!"OpenGL error - check the callstack in debugger");
		}
		FatalError("OpenGL API usage error! Use debugger to examine call stack!");
	}
}

internal B32 StringsAreEqual(char *src, char *dst, size_t dstlen)
{
	while(*src && dstlen-- && *dst)
	{
		if(*src++ != *dst++)
		{
			return 0;
		}
	}

	return (dstlen && *src == *dst) || (!dstlen && *src == 0);
}

internal void OS_GL_GetWGLinternals()
{
	HWND dummy = CreateWindowEx(0, "STATIC", "DummyWindow", WS_OVERLAPPED, CW_USEDEFAULT, CW_USEDEFAULT, CW_USEDEFAULT, CW_USEDEFAULT, NULL, NULL, NULL, NULL);
	Assert(dummy && "Failed to create dummy window");

	HDC dc = GetDC(dummy);
	Assert(dc && "Failed to get device context for dummy window");

	PIXELFORMATDESCRIPTOR desc = {
					.nSize = sizeof(desc),
					.nVersion = 1,
					.dwFlags = PFD_DRAW_TO_WINDOW | PFD_SUPPORT_OPENGL | PFD_DOUBLEBUFFER,
					.iPixelType = PFD_TYPE_RGBA,
					.cColorBits = 24,
	};

	S32 format = ChoosePixelFormat(dc, &desc);
	if(!format)
	{
		FatalError("Cannot choose OpenGL pixel format for dummy window");
	}

	S32 ok = DescribePixelFormat(dc, format, sizeof(desc), &desc);
	Assert(ok && "Failed to describe OpenGL pixel format");

	if(!SetPixelFormat(dc, format, &desc))
	{
		FatalError("Cannot set OpenGL pixel format for dummy window");
	}

	HGLRC rc = wglCreateContext(dc);
	Assert(rc && "Failed to create OpenGL context for dummy window");

	ok = wglMakeCurrent(dc, rc);
	Assert(ok && "Failed to make current OpenGL context for dummy window");

	typedef char * WINAPI wglGetExtensionsStringARBproc(HDC hdc);
	wglGetExtensionsStringARBproc *wglGetExtensionsStringARB = (void *)wglGetProcAddress("wglGetExtensionsStringARB");
	if(!wglGetExtensionsStringARB)
	{
		FatalError("OpenGL does not support WGL_ARB_extensions_string extension");
	}

	char *ext = wglGetExtensionsStringARB(dc);
	Assert(ext && "Failed to get OpenGL WGL extension string");

	char * start = ext;
	for(;;)
	{
		while(*ext != 0 && *ext != ' ')
		{
			ext++;
		}
		size_t length = ext - start;
		if(StringsAreEqual("WGL_ARB_pixel_format", start, length))
		{
			// https://www.khronos.org/registry/OpenGL/extensions/ARB/WGL_ARB_pixel_format.txt
			wglChoosePixelFormatARB = (void*)wglGetProcAddress("wglChoosePixelFormatARB");
		}
		else if(StringsAreEqual("WGL_ARB_create_context", start, length))
		{
			// https://www.khronos.org/registry/OpenGL/extensions/ARB/WGL_ARB_create_context.txt
			wglCreateContextAttribsARB = (void*)wglGetProcAddress("wglCreateContextAttribsARB");
		}
		else if(StringsAreEqual("WGL_EXT_swap_control", start, length))
		{
			// https://www.khronos.org/registry/OpenGL/extensions/EXT/WGL_EXT_swap_control.txt
			wglSwapIntervalEXT = (void*)wglGetProcAddress("wglSwapIntervalEXT");
		}
		if(*ext == 0)
		{
			break;
		}

		ext++;
		start = ext;
	}

	if(!(wglChoosePixelFormatARB && wglCreateContextAttribsARB && wglSwapIntervalEXT))
	{
		FatalError("OpenGL does not support required WGL extensions for modern context!");
	}

	wglMakeCurrent(NULL, NULL);
	wglDeleteContext(rc);
	ReleaseDC(dummy, dc);
	DestroyWindow(dummy);
}

internal void OS_GL_CreateContext(OS_Window *window)
{
	OS_GL_GetWGLinternals();

	HDC dc = window->device_context;

	// set pixel format for OpenGL context
	{
		S32 attrib[] = {
						WGL_DRAW_TO_WINDOW_ARB, GL_TRUE,
						WGL_SUPPORT_OPENGL_ARB, GL_TRUE,
						WGL_DOUBLE_BUFFER_ARB,  GL_TRUE,
						WGL_PIXEL_TYPE_ARB,     WGL_TYPE_RGBA_ARB,
						WGL_COLOR_BITS_ARB,     24,
						WGL_DEPTH_BITS_ARB,     24,
						WGL_STENCIL_BITS_ARB,   8,

						// uncomment for sRGB framebuffer, from WGL_ARB_framebuffer_sRGB extension
						// https://www.khronos.org/registry/OpenGL/extensions/ARB/ARB_framebuffer_sRGB.txt
						//WGL_FRAMEBUFFER_SRGB_CAPABLE_ARB, GL_TRUE,

						// uncomment for multisampeld framebuffer, from WGL_ARB_multisample extension
						// https://www.khronos.org/registry/OpenGL/extensions/ARB/ARB_multisample.txt
						//WGL_SAMPLE_BUFFERS_ARB, 1,
						//WGL_SAMPLES_ARB,        4, // 4x MSAA

						0,
		};

		S32 format;
		UINT formats;
		if(!wglChoosePixelFormatARB(dc, attrib, NULL, 1, &format, &formats) || formats == 0)
		{
			FatalError("OpenGL does not support required pixel format!");
		}

		PIXELFORMATDESCRIPTOR desc = {.nSize = sizeof(desc)};
		S32 ok = DescribePixelFormat(dc, format, sizeof(desc), &desc);
		Assert(ok && "Failed to describe OpenGL pixel format");

		if(!SetPixelFormat(dc, format, &desc))
		{
			FatalError("Cannot set OpenGL selected pixel format!");
		}
	}

	// create modern OpenGL context
	{
		S32 attrib[] =
		{
						WGL_CONTEXT_MAJOR_VERSION_ARB, 3,
						WGL_CONTEXT_MINOR_VERSION_ARB, 3,
						WGL_CONTEXT_PROFILE_MASK_ARB,  WGL_CONTEXT_CORE_PROFILE_BIT_ARB,
						// ask for debug context for non "Release" builds
						// this is so we can enable debug callback
						WGL_CONTEXT_FLAGS_ARB, WGL_CONTEXT_DEBUG_BIT_ARB,
						0,
		};

		HGLRC rc = wglCreateContextAttribsARB(dc, NULL, attrib);
		if(!rc)
		{
			FatalError("Cannot create modern OpenGL context! OpenGL version 4.5 not supported?");
		}

		BOOL ok = wglMakeCurrent(dc, rc);
		Assert(ok && "Failed to make current OpenGL context");

		// load OpenGL internals
#define X(type, name) name = (type)wglGetProcAddress(#name); Assert(name);
		GL_internalS(X)
#undef X
			// enable debug callback
			glDebugMessageCallback(&OS_GL_DebugCallback, NULL);
		glEnable(GL_DEBUG_OUTPUT_SYNCHRONOUS);
	}

	{
		u32 width, height;
		OS_QueryWindowSize(window, &width, &height);
		glViewport(0, 0, width, height);
	}
}