global OpenGL open_gl;

internal ShaderProgram
R_GL_CreateShader(String8 vertex_shader_path, String8 fragment_shader_path, B32 debug_callback)
{
	if(!debug_callback)
	{
		glDebugMessageCallback(0, NULL);
		glDisable(GL_DEBUG_OUTPUT_SYNCHRONOUS);
	}

	ShaderProgram result = {0};
	GLchar *header = "#version 330 core\n";

	OS_ReadFileResult vertex_shader_read_result = OS_ReadEntireFile(vertex_shader_path);
	if(vertex_shader_read_result.contents_size == 0)
	{
		printf("Failed to read vertex shader!\n");
		return result;
	}

	const GLchar *vertex_shader_source[] =
	{
		header,
		vertex_shader_read_result.content,
	};

	GLuint vertex_shader = glCreateShader(GL_VERTEX_SHADER);
	glShaderSource(vertex_shader, ArrayCount(vertex_shader_source), vertex_shader_source, 0);
	glCompileShader(vertex_shader);

	S32 success;
	char info_log[512];
	glGetShaderiv(vertex_shader, GL_COMPILE_STATUS, &success);

	if(!success)
	{
		glGetShaderInfoLog(vertex_shader, 512, NULL, info_log);
		printf("Failed to compile vertex shader: %s\n", info_log);
	}

	OS_ReadFileResult fragment_shader_read_result = OS_ReadEntireFile(fragment_shader_path);
	if(fragment_shader_read_result.contents_size == 0)
	{
		printf("Failed to read fragment shader!\n");
		return result;
	}

	const GLchar *fragment_shader_source[] =
	{
		header,
		fragment_shader_read_result.content,
	};

	GLuint fragment_shader = glCreateShader(GL_FRAGMENT_SHADER);
	glShaderSource(fragment_shader, ArrayCount(fragment_shader_source), fragment_shader_source, 0);
	glCompileShader(fragment_shader);

	if(!success)
	{
		glGetShaderInfoLog(fragment_shader, 512, NULL, info_log);
		printf("Failed to compile fragment shader: %s\n", info_log);
	}

	result.id = glCreateProgram();
	glAttachShader(result.id, vertex_shader);
	glAttachShader(result.id, fragment_shader);
	glLinkProgram(result.id);

	glGetProgramiv(result.id, GL_LINK_STATUS, &success);
	if(!success)
	{
		glGetProgramInfoLog(result.id, 512, NULL, info_log);
		printf("Failed to compile shader program: %s\n", info_log);
		glDeleteProgram(result.id);
		result.id = 0;
	}
	glUseProgram(result.id);

	glDeleteShader(vertex_shader);
	glDeleteShader(fragment_shader);

	if(!debug_callback)
	{
		// enable debug callback
		glDebugMessageCallback(&OS_GL_DebugCallback, NULL);
		glEnable(GL_DEBUG_OUTPUT_SYNCHRONOUS);
	}
	return(result);
}

internal void R_GL_Begin(OS_Window *window, MemoryArena *frame_arena)
{
	U32 num_indices_per_rect = 6;
	r_state.num_rects = 0;
	r_state.max_num_rects = 1'000'000;
	r_state.rect_instances = PushArrayNoZero(frame_arena, r_state.max_num_rects, R_RectInstance);
	r_state.rect_indices = PushArrayNoZero(frame_arena, r_state.max_num_rects * num_indices_per_rect, U32);

	OS_QueryWindowSize(window, &r_state.settings.render_dim.x, &r_state.settings.render_dim.y);

	glClear(GL_COLOR_BUFFER_BIT);
	glClearColor(0.2f, 0.3f, 0.3f, 1.0f);
}

internal void R_GL_End(OS_Window *window)
{
	glEnable(GL_BLEND);
	glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);

	glUseProgram(open_gl.shader_program.id);
	glBindVertexArray(open_gl.vao);
	glBindTexture(GL_TEXTURE_2D_ARRAY, open_gl.texture_array);

	Mat4x4F32 proj = Ortho(0, (F32)r_state.settings.render_dim.x, (F32)r_state.settings.render_dim.y, 0, -1.0f, 1.0f);

	GLuint proj_loc = glGetUniformLocation(open_gl.shader_program.id, "u_projection");
	glUniformMatrix4fv(proj_loc, 1, GL_TRUE, &proj.m[0][0]);

	GLuint clip_rect_loc = glGetUniformLocation(open_gl.shader_program.id, "u_clip_rects");
	glUniforMat4x4F32fv(clip_rect_loc, r_state.max_num_clip_rects, (GLfloat *)r_state.clip_rect_array);

	glBufferSubData(GL_ARRAY_BUFFER, 0, sizeof(R_RectInstance) * r_state.num_rects, r_state.rect_instances);

	glBufferSubData(GL_ELEMENT_ARRAY_BUFFER, 0, sizeof(U32) * 6 * r_state.num_rects, r_state.rect_indices);

	glDrawElementsInstanced(GL_TRIANGLES, 6, GL_UNSIGNED_INT, 0, r_state.num_rects);

	R_TextureQueue *texture_queue = &r_state.texture_queue;
	for(R_TextureNode *node = texture_queue->first;
			node != 0;
			node = QueuePop(texture_queue->first, texture_queue->last))
	{
		R_Texture *texture = node->texture;
		if(texture->handle)
		{
			R_GL_UpdateTextureHandle(texture);
		}
		else
		{
			texture->handle = R_GL_GenerateTextureHandle(texture);
		}
	}
}

internal void R_GL_Init()
{
	r_state.max_num_rects = 1'000'000;

	open_gl.shader_program = R_GL_CreateShader(CORE_RESOURCE("shaders/vertex_shader.vert"),
																						 CORE_RESOURCE("shaders/fragment_shader.frag"), true);

	glGenVertexArrays(1, &open_gl.vao);
	glBindVertexArray(open_gl.vao);
	glGenBuffers(1, &open_gl.vbo);

	glBindBuffer(GL_ARRAY_BUFFER, open_gl.vbo);
	glBufferData(GL_ARRAY_BUFFER, r_state.max_num_rects * sizeof(R_RectInstance), 0, GL_DYNAMIC_DRAW);

	glGenBuffers(1, &open_gl.ebo);
	glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, open_gl.ebo);
	glBufferData(GL_ELEMENT_ARRAY_BUFFER, r_state.max_num_rects * sizeof(U32) * 6, 0, GL_DYNAMIC_DRAW);

	// NOTE(hampus): Position 0
	glEnableVertexAttribArray(0);
	glVertexAttribPointer(0, 2, GL_FLOAT, GL_FALSE, sizeof(R_RectInstance), (void *)MemberOffset(R_RectInstance, min));
	glVertexAttribDivisor(0, 1);

	// NOTE(hampus): Position 1
	glEnableVertexAttribArray(1);
	glVertexAttribPointer(1, 2, GL_FLOAT, GL_FALSE, sizeof(R_RectInstance), (void *)MemberOffset(R_RectInstance, max));
	glVertexAttribDivisor(1, 1);

	// NOTE(hampus): Color 1
	glEnableVertexAttribArray(2);
	glVertexAttribPointer(2, 4, GL_FLOAT, GL_FALSE, sizeof(R_RectInstance), (void *)MemberOffset(R_RectInstance, color));
	glVertexAttribDivisor(2, 1);

	// NOTE(hampus): UV 0
	glEnableVertexAttribArray(3);
	glVertexAttribPointer(3, 2, GL_FLOAT, GL_FALSE, sizeof(R_RectInstance), (void *)MemberOffset(R_RectInstance, min_uv));
	glVertexAttribDivisor(3, 1);

	// NOTE(hampus): UV 1
	glEnableVertexAttribArray(4);
	glVertexAttribPointer(4, 2, GL_FLOAT, GL_FALSE, sizeof(R_RectInstance), (void *)MemberOffset(R_RectInstance, max_uv));
	glVertexAttribDivisor(4, 1);

	// NOTE(hampus): Corner radius 0
	glEnableVertexAttribArray(5);
	glVertexAttribPointer(5, 4, GL_FLOAT, GL_FALSE, sizeof(R_RectInstance), (void *)MemberOffset(R_RectInstance, corner_radius));
	glVertexAttribDivisor(5, 1);

	// NOTE(hampus): Edge softness
	glEnableVertexAttribArray(6);
	glVertexAttribPointer(6, 1, GL_FLOAT, GL_FALSE, sizeof(R_RectInstance), (void *)MemberOffset(R_RectInstance, edge_softness));
	glVertexAttribDivisor(6, 1);

	// NOTE(hampus): Border thickness
	glEnableVertexAttribArray(7);
	glVertexAttribPointer(7, 1, GL_FLOAT, GL_FALSE, sizeof(R_RectInstance), (void *)MemberOffset(R_RectInstance, border_thickness));
	glVertexAttribDivisor(7, 1);

	// NOTE(hampus): Texture index
	glEnableVertexAttribArray(8);
	glVertexAttribIPointer(8, 1, GL_INT, sizeof(R_RectInstance), (void *)MemberOffset(R_RectInstance, tex_handle));
	glVertexAttribDivisor(8, 1);

	// NOTE(hampus): Use nearest neighbor
	glEnableVertexAttribArray(9);
	glVertexAttribIPointer(9, 1, GL_INT, sizeof(R_RectInstance), (void *)MemberOffset(R_RectInstance, bilinear));
	glVertexAttribDivisor(9, 1);

	// NOTE(hampus): Clip calc_rect index
	glEnableVertexAttribArray(10);
	glVertexAttribIPointer(10, 1, GL_INT, sizeof(R_RectInstance), (void *)MemberOffset(R_RectInstance, clip_rect_index));
	glVertexAttribDivisor(10, 1);

	// NOTE(hampus): Is text
	glEnableVertexAttribArray(11);
	glVertexAttribIPointer(11, 1, GL_INT, sizeof(R_RectInstance), (void *)MemberOffset(R_RectInstance, is_text));
	glVertexAttribDivisor(11, 1);

	glEnable(GL_MULTISAMPLE);

	{
		// NOTE(hampus): 1 is reserved for white texture.
		// 0 is null
		open_gl.next_texture_handle = 1;
		glGenTextures(1, &open_gl.texture_array);
		glBindTexture(GL_TEXTURE_2D_ARRAY, open_gl.texture_array);
		glTexStorage3D(GL_TEXTURE_2D_ARRAY, 1, GL_RGBA8, 512, 512, 512);

		glTexParameteri(GL_TEXTURE_2D_ARRAY, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
		glTexParameteri(GL_TEXTURE_2D_ARRAY, GL_TEXTURE_MAG_FILTER, GL_LINEAR);

		U32 white = 0xffffffff;

		glBindTexture(GL_TEXTURE_2D_ARRAY, open_gl.texture_array);

		glTexSubImage3D(GL_TEXTURE_2D_ARRAY, 0, 0, 0, 1, 1, 1, 1, GL_RGBA, GL_UNSIGNED_BYTE, &white);

		glBindTexture(GL_TEXTURE_2D_ARRAY, 0);

		open_gl.next_texture_handle++;
	}
}

internal U32 R_GL_GenerateTextureHandle(R_Texture *texture)
{
	U32 texture_handle = open_gl.next_texture_handle;

	glBindTexture(GL_TEXTURE_2D_ARRAY, open_gl.texture_array);

	glTexSubImage3D(GL_TEXTURE_2D_ARRAY, 0, 0, 0, texture_handle, texture->width, texture->height, 1, GL_RGBA, GL_UNSIGNED_BYTE, texture->data);

	glBindTexture(GL_TEXTURE_2D_ARRAY, 0);

	OS_FreeMemory(texture->data);

	open_gl.next_texture_handle += 1;

	return texture_handle;
}

internal void R_GL_UpdateTextureHandle(R_Texture *texture)
{
	glBindTexture(GL_TEXTURE_2D_ARRAY, open_gl.texture_array);

	glTexSubImage3D(GL_TEXTURE_2D_ARRAY, 0, 0, 0, texture->handle, texture->width, texture->height, 1, GL_RGBA, GL_UNSIGNED_BYTE, texture->data);

	glBindTexture(GL_TEXTURE_2D_ARRAY, 0);

	OS_FreeMemory(texture->data);
}
