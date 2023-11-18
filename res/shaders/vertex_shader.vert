layout (location = 0) in vec2 v_min;
layout (location = 1) in vec2 v_max;
layout (location = 2) in vec4 v_color;
layout (location = 3) in vec2 v_min_uv;
layout (location = 4) in vec2 v_max_uv;
layout (location = 5) in vec4 v_corner_radius;
layout (location = 6) in float v_edge_softness;
layout (location = 7) in float v_border_thickness;
layout (location = 8) in int v_tex_index;
layout (location = 9) in int v_bilinear;
layout (location = 10) in int v_clip_rect_index;
layout (location = 11) in int v_is_text;

out vec4 f_color;
out vec2 f_uv;
flat out int f_tex_index;
out vec2 f_dst_pos;
out vec2 f_dst_center;
out vec2 f_dst_half_size;
out float f_corner_radius;
out float f_edge_softness;
out float f_border_thickness;
flat out int f_bilinear;
flat out int f_clip_rect_index;
flat out int f_is_text;

uniform mat4 u_projection;

void main()
{

    vec2 vertices[4];
    vertices[0] = vec2(-1, -1);
    vertices[1] = vec2(+1, -1);
    vertices[2] = vec2(+1, +1);
    vertices[3] = vec2(-1, +1);

    float corner_radius[4];
    corner_radius[0] = v_corner_radius.x;
    corner_radius[1] = v_corner_radius.y;
    corner_radius[2] = v_corner_radius.z;
    corner_radius[3] = v_corner_radius.w;

    vec2 dst_half_size = (v_max - v_min) / 2;
    vec2 dst_center = (v_max + v_min) / 2;
    vec2 dst_pos = (vertices[gl_VertexID] * dst_half_size + dst_center);

    vec2 src_half_size = (v_max_uv - v_min_uv) / 2;
    vec2 src_center = (v_max_uv + v_min_uv) / 2;
    vec2 src_pos = (vertices[gl_VertexID] * src_half_size + src_center);

    f_uv = src_pos;
    f_color = v_color;
    f_tex_index = v_tex_index;

    f_dst_pos       = dst_pos;
	if (v_is_text == 2)
	{
		f_dst_center    = v_min;
    	f_dst_half_size = v_max;
	}
	else
	{
    	f_dst_center    = dst_center;
    	f_dst_half_size = dst_half_size;
	}


    f_corner_radius = corner_radius[gl_VertexID];
    f_edge_softness = v_edge_softness;
    f_border_thickness = v_border_thickness;

    f_clip_rect_index = v_clip_rect_index;

    f_bilinear = v_bilinear;
    f_is_text = v_is_text;

    gl_Position = u_projection * vec4(dst_pos.x, dst_pos.y, 1.0f, 1.0f);
}