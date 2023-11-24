struct VS_INPUT
{
	uint vertex_id : SV_VertexID;
	float2 inst_pos : INST_POS;
	float2 dir : DIR;
	float thickness : THICKNESS;
};

struct PS_INPUT
{
	float4 projected_pos : SV_POSITION;
	float2 pos : POS;
	float2 ip : IP;
	float2 dir : DIR;
	float thickness : THICKNESS;
};

cbuffer cbuffer0 : register(b0)
{
	float4x4 uTransform;
}

float
LineSegmentSDF(float2 p, float2 a, float2 b)
{
	float2 ba = b - a;
	float2 pa = p - a;
	float h = clamp(dot(pa, ba) / dot(ba, ba), 0., 1.);
	return length(pa - h * ba);
}

PS_INPUT vs(VS_INPUT input)
{
	float2 vertices[6];
	vertices[0] = float2(-1, -1);
	vertices[1] = float2(-1, +1);
	vertices[2] = float2(+1, +1);

	vertices[3] = float2(-1, -1);
	vertices[4] = float2(+1, +1);
	vertices[5] = float2(+1, -1);

	float2 pos = vertices[input.vertex_id];

	float2 n = float2(-input.dir.y, input.dir.x) / length(input.dir);
	float2 apos = pos.y * input.dir + pos.x * n * input.thickness;
	float4 new_pos = float4(apos + input.inst_pos, 0.0, 1.0);

	PS_INPUT output;
	output.projected_pos = mul(uTransform, new_pos);
	output.pos = float2(new_pos.x, new_pos.y);
	output.ip = input.inst_pos;
	output.dir = input.dir;
	output.thickness = input.thickness;

	return output;
}

float4 ps(PS_INPUT input) : SV_TARGET
{
	float2 a = input.ip - input.dir / 2;
	float2 b = input.ip + input.dir / 2;

	float dist = LineSegmentSDF(float2(input.pos.x, input.pos.y), a, b) - input.thickness;

	float aaf = fwidth(dist);

	float sdf_factor = 1.f - smoothstep(-aaf, aaf, dist);

	float4 color = float4(1, 1, 1, 1);
	color.a *= sdf_factor;
	return color;
}