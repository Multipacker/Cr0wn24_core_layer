in vec4 f_color;
in vec2 f_uv;
flat in int f_tex_index;
in vec2 f_dst_pos;
in vec2 f_dst_center;
in vec2 f_dst_half_size;
in float f_corner_radius;
in float f_edge_softness;
in float f_border_thickness;
flat in int f_bilinear;
flat in int f_clip_rect_index;
flat in int f_is_text;

out vec4 FragColor;

uniform vec4 u_clip_rects[32];

// texture sampler
uniform sampler2DArray texture_array;

float median(float r, float g, float b) {
    return max(min(r, g), min(max(r, g), b));
}

float screenPxRange(vec2 uv) 
{
    vec2 unitRange = vec2(2)/vec2(textureSize(texture_array, 0));
    vec2 screenTexSize = vec2(1.0)/fwidth(uv);
    return max(0.5*dot(unitRange, screenTexSize), 1.0);
}

float rounded_rect_sdf(vec2 sample_pos,
                     vec2 rect_center,
                     vec2 rect_half_size,
                     float r)
{
    vec2 d2 = (abs(rect_center - sample_pos) - rect_half_size + vec2(r, r));
    return min(max(d2.x, d2.y), 0.0) + length(max(d2, 0.0)) - r;
}

float line_segment_sdf(vec2 sample_pos, vec2 v0, vec2 v1)
{
    vec2 ba = v1-v0;
    vec2 pa = sample_pos-v0;
    float h = clamp( dot(pa,ba)/dot(ba,ba), 0.0, 1.0 );
    return length(pa-h*ba);
}

void main()
{
    vec4 clip_rect = u_clip_rects[f_clip_rect_index];
    if(f_dst_pos.x >= clip_rect.x && f_dst_pos.y >= clip_rect.y && 
       f_dst_pos.x < clip_rect.z && f_dst_pos.y < clip_rect.w)
    {
        ivec3 texture_size = textureSize(texture_array, 0);
        vec2 new_uv = f_uv;

        if(f_bilinear == 0)
        {
            new_uv = (floor(f_uv * ivec2(texture_size)) + 0.5) / ivec2(texture_size);
        }

        vec3 array_uv = vec3(new_uv.x, new_uv.y, float(f_tex_index));
        if (f_is_text == 1)
        {
						vec3 msd = texture(texture_array, array_uv).rgb;
   				 	float sd = median(msd.r, msd.g, msd.b);
    				float screenPxDistance = screenPxRange(new_uv)*(sd - 0.5);
    				float opacity = clamp(screenPxDistance + 0.5, 0.0, 1.0);
    				FragColor = vec4(f_color.rgb, f_color.a * opacity);
        }
				else if (f_is_text == 2)
				{
					vec2 softness = vec2(f_edge_softness);
				
					vec2 softness_padding = vec2(max(0, softness.x*2-1),
											max(0, softness.x*2-1));
											
					float dist = line_segment_sdf(f_dst_pos,
															f_dst_center,
															f_dst_half_size);

					float sdf_factor = 1.f - smoothstep(0, 1.0, dist);

								FragColor = texture(texture_array, array_uv) * f_color * sdf_factor;
				}
        else
        {
			
				vec2 softness = vec2(f_edge_softness);
			
				vec2 softness_padding = vec2(max(0, softness.x*2-1),
										max(0, softness.x*2-1));
				float border_factor = 1.0f;
				if (f_border_thickness != 0) 
				{
					vec2 interior_half_size =
						f_dst_half_size - vec2(f_border_thickness);

					float interior_radius_reduce_f = 
						min(interior_half_size.x/f_dst_half_size.x,
							interior_half_size.y/f_dst_half_size.y);
					float interior_corner_radius =
						(f_corner_radius *
						interior_radius_reduce_f *
						interior_radius_reduce_f);

					float inside_d = rounded_rect_sdf(f_dst_pos,
													f_dst_center,
													interior_half_size-
													softness_padding,
													interior_corner_radius);

					float inside_f = smoothstep(0, 2*softness.x, inside_d);
					border_factor = inside_f;
				}

				float dist = rounded_rect_sdf(f_dst_pos,
									f_dst_center,
									f_dst_half_size-softness_padding,
									f_corner_radius);

				float sdf_factor = 1.f - smoothstep(0, 2*softness.x, dist);

				FragColor = texture(texture_array, array_uv) * f_color * sdf_factor * border_factor;
        }
    }
    else
    {
        discard;
    }
}