#ifndef WIN32_OPENGL_H
#define WIN32_OPENGL_H

// TODO(hampus): Remove these
#include "ext/glcorearb.h"
#include "ext/wglext.h"

#define GL_internalS(X) \
X(PFNGLDEBUGMESSAGECALLBACKARBPROC,     glDebugMessageCallback      ) \
X(PFNGLGENBUFFERSPROC,                  glGenBuffers                ) \
X(PFNGLGENVERTEXARRAYSPROC,             glGenVertexArrays           ) \
X(PFNGLBINDBUFFERPROC,                  glBindBuffer                ) \
X(PFNGLBUFFERDATAPROC,                  glBufferData                ) \
X(PFNGLCREATESHADERPROC,                glCreateShader              ) \
X(PFNGLSHADERSOURCEPROC,                glShaderSource              ) \
X(PFNGLCOMPILESHADERPROC,               glCompileShader             ) \
X(PFNGLGETSHADERIVPROC,                 glGetShaderiv               ) \
X(PFNGLGETSHADERINFOLOGPROC,            glGetShaderInfoLog          ) \
X(PFNGLGETSHADERINFOLOGPROC,            glGetShaderInfoLog          ) \
X(PFNGLCREATEPROGRAMPROC,               glCreateProgram             ) \
X(PFNGLATTACHSHADERPROC,                glAttachShader              ) \
X(PFNGLLINKPROGRAMPROC,                 glLinkProgram               ) \
X(PFNGLDELETESHADERPROC,                glDeleteShader              ) \
X(PFNGLBINDVERTEXARRAYPROC,             glBindVertexArray           ) \
X(PFNGLVERTEXATTRIBPOINTERPROC,         glVertexAttribPointer       ) \
X(PFNGLENABLEVERTEXATTRIBARRAYPROC,     glEnableVertexAttribArray   ) \
X(PFNGLUSEPROGRAMPROC,                  glUseProgram                ) \
X(PFNGLGETUNIFORMLOCATIONPROC,          glGetUniformLocation        ) \
X(PFNGLUNIFORMMATRIX4FVPROC,            glUniformMatrix4fv          ) \
X(PFNGLUNIFORM4FVPROC,                  glUniforMat4x4F32fv                ) \
X(PFNGLBUFFERSUBDATAPROC,               glBufferSubData             ) \
X(PFNGLUNIFORM1FPROC,                   glUniform1f                 ) \
X(PFNGLUNIFORM2FPROC,                   glUniform2f                 ) \
X(PFNGLUNIFORM3FPROC,                   glUniform3f                 ) \
X(PFNGLUNIFORM4FPROC,                   glUniforMat4x4F32f                 ) \
X(PFNGLUNIFORM1IPROC,                   glUniform1i                 ) \
X(PFNGLUNIFORM2IPROC,                   glUniform2i                 ) \
X(PFNGLUNIFORM3IPROC,                   glUniform3i                 ) \
X(PFNGLUNIFORM4IPROC,                   glUniforMat4x4F32i                 ) \
X(PFNGLUNIFORM1FVPROC,                  glUniform1fv                ) \
X(PFNGLUNIFORM2FVPROC,                  glUniform2fv                ) \
X(PFNGLUNIFORM3FVPROC,                  glUniform3fv                ) \
X(PFNGLUNIFORM4FVPROC,                  glUniforMat4x4F32fv                ) \
X(PFNGLTEXSUBIMAGE3DPROC,               glTexSubImage3D             ) \
X(PFNGLTEXSTORAGE3DPROC,                glTexStorage3D              ) \
X(PFNGLVERTEXATTRIBIPOINTERPROC,        glVertexAttribIPointer      ) \
X(PFNGLGETPROGRAMIVPROC,                glGetProgramiv              ) \
X(PFNGLGETPROGRAMINFOLOGPROC,           glGetProgramInfoLog         ) \
X(PFNGLVERTEXATTRIBDIVISORPROC,         glVertexAttribDivisor       ) \
X(PFNGLDRAWELEMENTSINSTANCEDPROC,       glDrawElementsInstanced     ) \
X(PFNGLGENERATEMIPMAPPROC,              glGenerateMipmap            ) \
X(PFNGLDELETEPROGRAMPROC,               glDeleteProgram             ) \
X(PFNGLDRAWARRAYSINSTANCEDPROC,         glDrawArraysInstanced       ) \

internal void OS_GL_CreateContext(OS_Window *window);
internal void OS_GL_GetWGLinternals();

#endif