struct VS_INPUT
{
	float2 min : MIN;
	float2 max : MAX;
	float2 min_uv : MIN_UV;
	float2 max_uv : MAX_UV;
	float4 color : COLOR;
	float4 corner_radius : CORNER_RADIUS;
	float4 extra_params : EXTRA_PARAMS; // x: edge_softness, y: omit_texture, z: is_text, w: border_thickness
	uint vertex_id : SV_VertexID;
};

struct PS_INPUT
{
	float4 pos : SV_POSITION;
	float2 uv : TEXCOORD;
	float4 color : COLOR;
	float corner_radius : CORNER_RADIUS;
	float2 dst_center : DST_CENTER;
	float2 dst_half_size : DST_HALF_SIZE;
	float edge_softness : EDGE_SOFTNESS;
	float omit_texture : OMIT_TEXTURE;
	float is_text : IS_TEXT;
	float border_thickness : BORDER_THICKNESS;
};

cbuffer cbuffer0 : register(b0)
{
	float4x4 uTransform;
}

sampler sampler0 : register(s0);
Texture2D<float4> texture0 : register(t0);

float
RoundedRectSDF(float2 sample_pos,
							 float2 rect_center,
							 float2 rect_half_size,
							 float r)
{
	float2 d2 = (abs(rect_center - sample_pos) -
							 rect_half_size +
							 float2(r, r));
	return min(max(d2.x, d2.y), 0.0) + length(max(d2, 0.0)) - r;
}

PS_INPUT vs(VS_INPUT input)
{
	float2 vertices[4];
	vertices[0] = float2(-1, -1);
	vertices[1] = float2(+1, -1);
	vertices[2] = float2(-1, +1);
	vertices[3] = float2(+1, +1);

	float2 dst_half_size = (input.max - input.min) / 2;
	float2 dst_center = (input.max + input.min) / 2;
	float2 dst_pos = (vertices[input.vertex_id] * dst_half_size + dst_center);

	float2 src_half_size = (input.max_uv - input.min_uv) / 2;
	float2 src_center = (input.max_uv + input.min_uv) / 2;
	float2 src_pos = (vertices[input.vertex_id] * src_half_size + src_center);

	PS_INPUT output;
	output.pos = mul(uTransform, float4(dst_pos, 0, 1));
	output.color = input.color;
	output.uv = src_pos;
	output.corner_radius = input.corner_radius[input.vertex_id];
	output.dst_center = dst_center;
	output.dst_half_size = dst_half_size;

	output.edge_softness = input.extra_params.x;
	output.omit_texture = input.extra_params.y;
	output.is_text = input.extra_params.z;
	output.border_thickness = input.extra_params.w;

	return output;
}

struct ps_out
{
	float4 color0 : SV_TARGET0;
	float4 color1 : SV_TARGET1;
};
ps_out ps(PS_INPUT input)
{
	float2 softness = float2(input.edge_softness, input.edge_softness);
	float2 softness_padding = float2(max(0, softness.x * 2 - 1),
																	 max(0, softness.x * 2 - 1));
	// sample distance
	float dist = RoundedRectSDF(input.pos.xy,
															input.dst_center,
															input.dst_half_size - softness_padding,
															input.corner_radius);

	// map distance => a blend factor
	float sdf_factor = 1.f - smoothstep(0, 2 * softness.x, dist);

	float border_factor = 1.f;
	if (input.border_thickness != 0)
	{
		float2 interior_half_size =
			input.dst_half_size - float2(input.border_thickness, input.border_thickness);

		float interior_radius_reduce_f =
			min(interior_half_size.x / input.dst_half_size.x,
					interior_half_size.y / input.dst_half_size.y);
		float interior_corner_radius =
			(input.corner_radius *
			 interior_radius_reduce_f *
			 interior_radius_reduce_f);

		float inside_d = RoundedRectSDF(input.pos.xy,
																		input.dst_center,
																		interior_half_size -
																		softness_padding,
																		interior_corner_radius);

		float inside_f = smoothstep(0, 2 * softness.x, inside_d);
		border_factor = inside_f;
	}

	float4 tex_sample = float4(1, 1, 1, 1);

	float omit_texture = input.omit_texture;

	if (omit_texture < 1)
	{
		tex_sample = texture0.Sample(sampler0, input.uv);
	}

	ps_out output;
	if (input.is_text < 1)
	{
		input.color.a *= sdf_factor * border_factor;
		float4 color = input.color * tex_sample;
		output.color0 = color;
		output.color1 = float4(color.a, color.a, color.a, color.a);
	}
	else
	{
		float gamma = 1.43;
		float inv_gamma = 1.0 / gamma;

		tex_sample = pow(abs(tex_sample), inv_gamma);
		output.color0 = float4(input.color.rgb * input.color.a, input.color.a);
		output.color1 = float4(tex_sample.rgb, input.color.a);
	}

	return output;
}