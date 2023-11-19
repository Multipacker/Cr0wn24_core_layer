#pragma comment(lib, "freetype.lib")

#define STB_IMAGE_IMPLEMENTATION
#include "../external/stb_image.h"

// NOTE(hampus): freetype has variables named "internal"
#ifdef internal
#undef internal
#endif

#include <freetype/freetype.h>

#define internal static

global R_State r_state;

internal R_Handle D3D11_LoadTexture(void *data, S32 width, S32 height);

internal void
R_Init()
{
	U32 white = 0xffffffff;
	r_state.white_texture.handle = D3D11_LoadTexture(&white, 1, 1);
	r_state.white_texture.dim.height = 1;
	r_state.white_texture.dim.width = 1;
}

internal R_Texture
R_LoadTexture(String8 path)
{
	R_Texture result = {0};

	S32 channels;
	void *data = stbi_load((const char *)path.str, &result.dim.width, &result.dim.height, &channels, 0);

	result.handle = D3D11_LoadTexture(data, result.dim.width, result.dim.height);

	stbi_image_free(data);

	return(result);
}

internal B32
R_HandleMatch(R_Handle a, R_Handle b)
{
	return a.a == b.a && a.b == b.b;
}

internal B32
R_RectF32Match(RectF32 a, RectF32 b)
{
	return (a.x0 == b.x0 && a.x1 == b.x1 && a.y0 == b.y0 && a.y1 == b.y1);
}

#if R_DEBUG
internal void
R_PushRect_(Vec2F32 min, Vec2F32 max, R_RectParams params, char *file, S32 line)
#else
internal void
R_PushRect_(Vec2F32 min, Vec2F32 max, R_RectParams params)
#endif
{
	RenderData *render_data = &r_state.render_data;
	Batch2DNode *batch_node = render_data->batch_list->last;

	R_Texture texture = params.texture;

	B32 rect_match = false;
	if(batch_node)
	{
		rect_match = R_RectF32Match(batch_node->batch->clip_rect, r_state.clip_rect_stack.first->rect);
	}

	if(!batch_node ||
	   (!R_HandleMatch(texture.handle, batch_node->batch->tex.handle) &&
		!R_HandleMatch(texture.handle, r_state.white_texture.handle)) ||
	   !rect_match)
	{
		// NOTE(hampus): If the previus batch just contained a white texture,
		// we'll just replace it with our texture instead
		B32 previous_was_white_texture = false;
		if(batch_node)
		{
			if(R_HandleMatch(batch_node->batch->tex.handle, r_state.white_texture.handle) && !rect_match)
			{
				previous_was_white_texture = true;
			}
		}

		if(!previous_was_white_texture)
		{
			batch_node = PushStruct(r_state.arena, Batch2DNode);
			batch_node->batch = PushStruct(r_state.arena, Batch2D);
			batch_node->batch->num_rects = 0;
			batch_node->batch->clip_rect = r_state.clip_rect_stack.first->rect;

#if R_DEBUG
			batch_node->batch->file = file;
			batch_node->batch->line = line;
#endif

			DLL_PushBack(render_data->batch_list->first, render_data->batch_list->last, batch_node);
		}

		batch_node->batch->tex = texture;
	}

	Batch2D *batch = batch_node->batch;

	Rect *rect = batch->rects + batch->num_rects;

	rect->min = V2(roundf(min.x), roundf(min.y));
	rect->max = V2(roundf(max.x), roundf(max.y));
	rect->color = params.color;
	rect->min_uv = params.texture.src_p0;
	rect->max_uv = params.texture.src_p1;
	rect->corner_radius = params.corner_radius;
	rect->omit_texture = R_HandleMatch(params.texture.handle, r_state.white_texture.handle) ? 1.0f : 0.0f;
	rect->is_text = (F32)!!params.text;
	rect->edge_softness = params.edge_softness;
	rect->border_thickness = params.border_thickness;

	batch->num_rects++;
}

internal void
R_PushGlyph(Vec2F32 pos, S32 glyph_height, R_Font *font, R_Glyph *glyph, Vec4F32 color)
{
	F32 scale = (F32)glyph_height / (F32)font->height;

	F32 xpos = pos.x + glyph->bearing.x * scale;
	F32 ypos = pos.y + (-glyph->bearing.y) * scale + (font->max_ascent) * scale;

	F32 width = glyph->size.x * scale;
	F32 height = glyph->size.y * scale;

	R_PushRect(V2(xpos,
				  ypos),
			   V2(xpos + width,
				  ypos + height),
			   .texture = glyph->texture,
			   .color = color,
			   .text = true);

}

internal void
R_PushGlyphIndex(Vec2F32 pos, S32 glyph_height, R_Font *font, U32 index, Vec4F32 color)
{
	R_Glyph *glyph = font->glyphs + index;
	R_PushGlyph(pos, glyph_height, font, glyph, color);
}

internal void
R_PushText(Vec2F32 pos, S32 height, R_Font *font, String8 text, Vec4F32 color)
{
	F32 scale = (F32)height / (F32)font->height;

	for(U64 i = 0; i < text.size; ++i)
	{
		R_Glyph *glyph = font->glyphs + text.str[i];
		R_PushGlyph(pos, height, font, glyph, color);
		pos.x += (glyph->advance) * scale;
	}
}

internal TextureAtlas
R_PackBitmapsIntoTextureAtlas(MemoryArena *arena, S32 atlas_width, S32 atlas_height,
							  R_LoadedBitmap *bitmaps, S32 bitmap_count, S32 padding)
{
	// TODO(hampus): Really naive algoritm. Rewrite

	TextureAtlas result = {0};
	result.dim.width = atlas_width;
	result.dim.height = atlas_height;

	TempMemoryArena scratch = GetScratch(0, 0);

	void *data = PushArray(scratch.arena, result.dim.width * result.dim.height * 4, U8);

	result.textures = PushArray(arena, bitmap_count, R_Texture);

	S32 *indices = PushArray(scratch.arena, bitmap_count, S32);
	// NOTE(hampus): The indices are for keeping track of
	// where the bitmaps we got in are in the sorted array
	// so that we can give the textures back in the order
	// the bitmaps came in
	for(S32 i = 0; i < bitmap_count; ++i)
	{
		indices[i] = i;
	}

#if 0

	// NOTE(hampus): Sort the entries by height
	for(S32 i = 0; i < bitmap_count - 1; ++i)
	{
		for(S32 j = 0; j < bitmap_count - 1 - i; ++j)
		{
			if(bitmaps[j].dim.height < bitmaps[j + 1].dim.height)
			{
				Swap(bitmaps[j], bitmaps[j + 1], R_LoadedBitmap);
				Swap(indices[j], indices[j + 1], S32);
			}
		}
	}
#endif
	// NOTE(hampus): Atlas-packing

	S32 current_x = padding;
	S32 current_y = padding;
	S32 row_height = 0;
	for(S32 i = 0; i < bitmap_count; ++i)
	{
		R_LoadedBitmap *entry = bitmaps + i;
		R_Texture *texture = result.textures + indices[i];

		texture->dim = entry->dim;

		if(entry->dim.height > row_height)
		{
			row_height = entry->dim.height;
		}

		if((current_x + entry->dim.width) > result.dim.width)
		{
			current_x = padding;
			current_y += row_height + padding;
			row_height = 0;
		}

		// NOTE(hampus): Copy bitmap into atlas
		U8 *src = entry->data;
		U8 *dst = (U8 *)data + (current_x + current_y * result.dim.width) * 4;
		for(S32 y = 0; y < entry->dim.height; ++y)
		{
			U32 *src_row = (U32 *)src;
			U32 *dst_row = (U32 *)dst;
			for(S32 x = 0; x < entry->dim.width; ++x)
			{
				*dst_row++ = *src_row++;
			}

			dst += result.dim.width * 4;
			src += entry->dim.width * 4;
		}

		texture->src_p0.x = (F32)current_x / (F32)result.dim.width;
		texture->src_p0.y = (F32)current_y / (F32)result.dim.height;
		texture->src_p1.x = (F32)(current_x + entry->dim.width) / (F32)result.dim.width;
		texture->src_p1.y = (F32)(current_y + entry->dim.height) / (F32)result.dim.height;

		current_x += entry->dim.width + padding;
	}

	result.texture.handle = D3D11_LoadTexture(data, result.dim.width, result.dim.height);
	result.texture.dim = result.dim;
	result.texture.src_p0 = V2(0, 0);
	result.texture.src_p1 = V2(1, 1);

	// NOTE(hampus): Give the atlas' texture handle
	// to all the textures
	for(S32 i = 0; i < bitmap_count; ++i)
	{
		R_Texture *texture = result.textures + i;
		texture->handle = result.texture.handle;
	}

	ReleaseScratch(scratch);

	return result;
}

internal void
R_LoadFont(MemoryArena *arena, R_Font *font, String8 font_path, String8 icon_path, S32 size)
{
	// TODO(hampus): This is temporary for now. Rewrite

	R_LoadedBitmap loaded_bitmap_glyphs[128 + 256];

	FT_Library ft;
	if(FT_Init_FreeType(&ft))
	{
		printf("Failed to initialize freetype!\n");
		return;
	}

	FT_Face face;
	if(FT_New_Face(ft, (const char *)font_path.str, 0, &face))
	{
		printf("Failed to load face!\n");
		return;
	}

	FT_Set_Pixel_Sizes(face, 0, size);
	font->height = size;

	font->max_ascent = ((S32)(face->ascender * (face->size->metrics.y_scale / 65536.0f))) >> 6;
	font->max_descent = ((S32)Abs((F32)face->descender * ((F32)face->size->metrics.y_scale / 65536.0f))) >> 6;

	for(U32 glyph_index = 0;
		glyph_index < 128;
		++glyph_index)
	{
		FT_Load_Char(face, glyph_index, FT_LOAD_RENDER | FT_LOAD_TARGET_LCD);
		// FT_Load_Glyph(face, (FT_UInt)(glyph_index), FT_LOAD_RENDER | FT_LOAD_TARGET_LCD);

		FT_Render_Glyph(face->glyph, FT_RENDER_MODE_LCD);

		R_Glyph *glyph = font->glyphs + glyph_index;
		R_LoadedBitmap *glyph_bitmap = loaded_bitmap_glyphs + glyph_index;
		glyph_bitmap->data = face->glyph->bitmap.buffer;

		// NOTE(hampus): Divide by 3 for LCD
		S32 width = face->glyph->bitmap.width / 3;
		// S32 width = face->glyph->bitmap.width;
		S32 height = face->glyph->bitmap.rows;

		glyph_bitmap->dim.width = width;
		glyph_bitmap->dim.height = height;

		U8 *new_teture_data = OS_AllocMem(glyph_bitmap->dim.width * glyph_bitmap->dim.height * 4);
		U8 *src = glyph_bitmap->data;
		U8 *dst = new_teture_data;

		for(S32 y = 0; y < glyph_bitmap->dim.height; ++y)
		{
			U8 *dst_row = dst;
			U8 *src_row = src;
			U8 *test2 = (U8 *)src_row;
			for(S32 x = 0; x < glyph_bitmap->dim.width; ++x)
			{
				U8 r = *test2++;
				U8 g = *test2++;
				U8 b = *test2++;
				*dst_row++ = r;
				*dst_row++ = g;
				*dst_row++ = b;
				*dst_row++ = 0xff;
			}

			dst += (S32)(glyph_bitmap->dim.width * 4);

			// NOTE(hampus): Freetype actually adds padding
			// so the pitch is the correct width to increment
			// by. 
			src += (S32)(face->glyph->bitmap.pitch);
		}

		// TODO(hampus): Memory leak here
		glyph_bitmap->data = new_teture_data;

		glyph->size = V2S(width, height);
		glyph->bearing = V2S(face->glyph->bitmap_left, face->glyph->bitmap_top);
		glyph->advance = face->glyph->advance.x >> 6;
		if(glyph->size.y > font->max_height)
		{
			font->max_height = (F32)glyph->size.y;
		}
	}

	FT_Face icon_face;
	if(FT_New_Face(ft, (const char *)icon_path.str, 0, &icon_face))
	{
		printf("Failed to load face!\n");
		return;
	}

	FT_Set_Pixel_Sizes(icon_face, 0, size - 5);

	for(U32 glyph_index = 0;
		glyph_index < 256;
		++glyph_index)
	{
		FT_Load_Glyph(icon_face, (FT_UInt)(glyph_index), FT_LOAD_RENDER | FT_LOAD_TARGET_LCD);

		FT_Render_Glyph(icon_face->glyph, FT_RENDER_MODE_LCD);

		R_Glyph *glyph = font->glyphs + glyph_index + 128;
		R_LoadedBitmap *glyph_bitmap = loaded_bitmap_glyphs + glyph_index + 128;
		glyph_bitmap->data = icon_face->glyph->bitmap.buffer;

		// NOTE(hampus): Divide by 3 for LCD
		S32 width = icon_face->glyph->bitmap.width / 3;
		// S32 width = icon_face->glyph->bitmap.width;
		S32 height = icon_face->glyph->bitmap.rows;

		glyph_bitmap->dim.width = width;
		glyph_bitmap->dim.height = height;

		U8 *new_teture_data = OS_AllocMem(glyph_bitmap->dim.width * glyph_bitmap->dim.height * 4);
		U8 *src = glyph_bitmap->data;
		U8 *dst = new_teture_data;

		for(S32 y = 0; y < glyph_bitmap->dim.height; ++y)
		{
			U8 *dst_row = dst;
			U8 *src_row = src;
			U8 *test2 = (U8 *)src_row;
			for(S32 x = 0; x < glyph_bitmap->dim.width; ++x)
			{
				U8 r = *test2++;
				U8 g = *test2++;
				U8 b = *test2++;
				*dst_row++ = r;
				*dst_row++ = g;
				*dst_row++ = b;
				*dst_row++ = 0xff;
			}

			dst += (S32)(glyph_bitmap->dim.width * 4);

			// NOTE(hampus): Freetype actually adds padding
			// so the pitch is the correct width to increment
			// by. 
			src += (S32)(icon_face->glyph->bitmap.pitch);
		}

		// TODO(hampus): Memory leak here
		glyph_bitmap->data = new_teture_data;

		glyph->size = V2S(width, height);
		glyph->bearing = V2S(icon_face->glyph->bitmap_left, icon_face->glyph->bitmap_top);
		glyph->advance = icon_face->glyph->advance.x >> 6;
		if(glyph->size.y > font->max_height)
		{
			font->max_height = (F32)glyph->size.y;
		}
	}


	font->atlas = R_PackBitmapsIntoTextureAtlas(arena, 1024, 1024, loaded_bitmap_glyphs, 256 + 128, 5);
#if 1
	for(U32 i = 0; i < 256 + 128; ++i)
	{
		font->glyphs[i].texture = font->atlas.textures[i];
	}
#endif
}

internal void
R_PushClipRect(RectF32 rect)
{
	Assert(rect.x1 > rect.x0);
	Assert(rect.y1 > rect.y0);

	ClipRectNode *node = PushStruct(r_state.arena, ClipRectNode);
	node->rect = rect;
	StackPush(r_state.clip_rect_stack.first, node);
}

internal void
R_PopClipRect()
{
	StackPop(r_state.clip_rect_stack.first);
}

internal RectF32
R_MakeRectF32(F32 x0, F32 y0, F32 x1, F32 y1)
{
	RectF32 result = {x0, y0, x1, y1};

	return result;
}

internal void
R_Begin(MemoryArena *arena)
{
	r_state.arena = arena;

	r_state.render_data.batch_list = PushStruct(r_state.arena, Batch2DList);

	R_PushClipRect(R_MakeRectF32(0, 0, 2560, 1440));
}

internal void
R_End()
{
	R_PopClipRect();

	D3D11_End();
}

internal Vec2F32
R_GetGlyphDim(R_Font *font, R_Glyph *glyph)
{
	Vec2F32 result;

	result.x = (F32)glyph->size.x;
	result.y = (F32)glyph->size.y;

	return result;
}

internal Vec2F32
R_GetTextDim(R_Font *font, String8 text)
{
	Vec2F32 result = {0};

	for(U64 i = 0; i < text.size; ++i)
	{
		result.x += font->glyphs[text.str[i]].advance;
	}
	result.y = (F32)(font->max_height);

	return(result);
}

internal B32
R_PointInsideRect(Vec2F32 p, RectF32 rect)
{
	B32 result = p.x >= rect.x0 && p.x < rect.x1 && p.y >= rect.y0 && p.y < rect.y1;

	return result;
}

internal RectF32
R_IntersectRectF32(RectF32 dest, RectF32 src)
{
	RectF32 result = {0};

	result.x0 = Clamp(dest.x0, src.x0, dest.x1);
	result.x1 = Clamp(dest.x0, src.x1, dest.x1);
	result.y0 = Clamp(dest.y0, src.y0, dest.y1);
	result.y1 = Clamp(dest.y0, src.y1, dest.y1);

	return(result);
}
