# Core layer

This is my core_layer used in almost all of my projects. It is all written from scratch with no libraries except for:

- stb_image.h (for reading .png, .jpg, ...)
- freetype (for creating font bitmaps)

It includes:

- Base layer
  - Memory arenas
  - Strings
  - Math
  - Implementation of doubly & singly linked lists, queues and stacks
- OS Layer (Windows only)
  -  Absraction layer over Win32
  -  Create a window & poll events
  -  Load in OpenGL functions
  -  Other miscellaneous that you expect from the OS
-  Renderer layer
   - 2D abstraction layer built on Direct3D11 and OpenGL (OpenGL not working at the current state)
   - Push rectangles, text or bitmaps onto batches
- UI Layer
  - Immediate-mode style GUI
  - Easy to use API
  - Sliders, buttons, radio buttons, text input & more

## Building from source
This demonstrates how to build and run the provided example in examples folder.

Only builds on windows at the moment.

1. Run `build_core_layer_demo.bat` with visual studio environment variables enabled
2. A folder `build` has been created in the root folder, run the .exe created.
