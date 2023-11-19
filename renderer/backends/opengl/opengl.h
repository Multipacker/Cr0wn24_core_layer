#ifndef OPENGL_H
#define OPENGL_H

typedef struct ShaderProgram
{
	GLuint id;
} ShaderProgram;

typedef struct OpenGL
{
	GLuint vao;
	GLuint vbo;
	GLuint ebo;

	GLuint texture_array;
	U32 next_texture_handle;

	ShaderProgram shader_program;
} OpenGL;

internal U32 R_GL_GenerateTextureHandle(R_Texture *texture);
internal void R_GL_UpdateTextureHandle(R_Texture *texture);
internal void R_GL_End(OS_Window *window);
internal void R_GL_Begin(OS_Window *window, MemoryArena *frame_arena);

#endif