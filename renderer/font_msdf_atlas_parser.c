internal S32 CharToS32(char ch)
{
	return (ch - 48);
}

internal U16 CharToU16(char ch)
{
	return (ch - 48);
}

internal U8 CharToU8(char ch)
{
	return (ch - 48);
}

internal S32 Str8ToS32(String8 string)
{
	S32 result = 0;

	for (U64 i = 0; i < string.size; ++i)
	{
		result = result * 10 + CharToS32(string.str[i]);
	}

	return(result);
}

internal U16 Str8ToU16(String8 string)
{
	U16 result = 0;

	for (U64 i = 0; i < string.size; ++i)
	{
		result = result * 10 + CharToU16(string.str[i]);
	}

	return(result);
}

internal U8 Str8ToU8(String8 string)
{
	U8 result = 0;

	for (U64 i = 0; i < string.size; ++i)
	{
		result = result * 10 + CharToU8(string.str[i]);
	}

	return(result);
}

internal F32 Str8ToF32(String8 string)
{
	F32 result = 0;

	char *data = (char *)string.str;

	B32 is_negative = false;

	U64 i = 0;
	if (*data == '-')
	{
		is_negative = true;
		++data;
		++i;
	}

	for (; i < string.size && *data != '.'; ++i)
	{
		result = result * 10 + CharToS32(*data);
		++data;
	}

	data += string.size - i - 1;

	F32 frac = 0;
	U64 start = i;
	for (; i < (string.size - 1); ++i)
	{
		frac = frac / 10 + CharToS32(*data);
		--data;
	}

	frac /= 10;

	result += frac;

	if (is_negative)
	{
		result = -result;
	}

	return(result);
}

internal String8 Str8CUntilChar(char *data, char ch)
{
	String8 result = {0};

	U32 length = 0;
	while (*(data + length) != ch)
	{
		length++;
	}

	result.str = (U8 *)data;
	result.size = length;

	return(result);
}

typedef struct AtlasGlyphMetrics
{
	GlyphMetrics metrics[128];
	GlyphMetrics icon_metrics[FontIcon_COUNT];
} AtlasGlyphMetrics;

internal U64 ParseGlyph(char *data, GlyphMetrics *metrics)
{
	U64 length = 0;
	{
		String8 string = Str8CUntilChar(data, ',');
		metrics->unicode = Str8ToS32(string);
		length += string.size + 1;
		data += string.size + 1;
	}

	{
		String8 string = Str8CUntilChar(data, ',');
		metrics->advance = Str8ToF32(string);
		length += string.size + 1;
		data += string.size + 1;
	}

	{
		String8 string = Str8CUntilChar(data, ',');
		metrics->plane_bounds.x0 = Str8ToF32(string);
		length += string.size + 1;
		data += string.size + 1;
	}

	{
		String8 string = Str8CUntilChar(data, ',');
		metrics->plane_bounds.y1 = Str8ToF32(string);
		length += string.size + 1;
		data += string.size + 1;
	}

	{
		String8 string = Str8CUntilChar(data, ',');
		metrics->plane_bounds.x1 = Str8ToF32(string);
		length += string.size + 1;
		data += string.size + 1;
	}

	{
		String8 string = Str8CUntilChar(data, ',');
		metrics->plane_bounds.y0 = Str8ToF32(string);
		length += string.size + 1;
		data += string.size + 1;
	}

	{
		String8 string = Str8CUntilChar(data, ',');
		metrics->atlas_bounds.x0 = Str8ToF32(string);
		length += string.size + 1;
		data += string.size + 1;
	}

	{
		String8 string = Str8CUntilChar(data, ',');
		metrics->atlas_bounds.y1 = Str8ToF32(string);
		length += string.size + 1;
		data += string.size + 1;
	}

	{
		String8 string = Str8CUntilChar(data, ',');
		metrics->atlas_bounds.x1 = Str8ToF32(string);
		length += string.size + 1;
		data += string.size + 1;
	}

	{
		String8 string = Str8CUntilChar(data, '\r');
		metrics->atlas_bounds.y0 = Str8ToF32(string);
		length += string.size + 1;
		data += string.size + 1;
	}

	return(length);
}

internal void ParseGlyphMetrics(AtlasGlyphMetrics *atlas_metrics, char *data)
{
	for (U32 i = 32; i < 127; ++i)
	{
		GlyphMetrics *metrics = &atlas_metrics->metrics[i];

		U64 length = ParseGlyph(data, metrics);
		data += length + 1;
	}

	for (S32 i = 1; i < FontIcon_COUNT; ++i)
	{
		if (*data)
		{
			GlyphMetrics *metrics = &atlas_metrics->icon_metrics[i];
			U64 length = ParseGlyph(data, metrics);
			data += length + 1;
		}
	}
}

#define GLYPH_SIZE 10

internal void LoadGlyph(GlyphMetrics *metrics, R_Glyph *glyph, R_Texture *atlas)
{
	R_Texture *texture = &glyph->texture;

	metrics->atlas_bounds.y0 = atlas->height - metrics->atlas_bounds.y0;
	metrics->atlas_bounds.y1 = atlas->height - metrics->atlas_bounds.y1;

	S32 x = (S32)metrics->atlas_bounds.x0;
	S32 y = (S32)metrics->atlas_bounds.y0;
	S32 width = (S32)(metrics->atlas_bounds.x1 - metrics->atlas_bounds.x0);
	S32 height = (S32)(metrics->atlas_bounds.y1 - metrics->atlas_bounds.y0);

	texture->data = OS_AllocMem(height * width * 4);
	texture->width = width;
	texture->height = height;

	U32 *start = (U32 *)(atlas->data + x * 4 + (y * atlas->width * 4));

	U32 *dst2 = (U32 *)texture->data;

	U32 *row = start;
	for (S32 y = 0; y < height; ++y)
	{
		U32 *pixel = row;
		for (S32 x = 0; x < width; ++x)
		{
			*dst2++ = *pixel++;
		}
		row += atlas->width;
	}

	glyph->size.x = texture->width;
	glyph->size.y = texture->height;
	glyph->advance = (S32)(metrics->advance * GLYPH_SIZE);

	glyph->bearing.x = (S32)(metrics->plane_bounds.x0 * GLYPH_SIZE);
	glyph->bearing.y = (S32)(metrics->plane_bounds.y0 * GLYPH_SIZE);
}

internal void LoadFontFromMSDFAtlas(R_Font *font, AtlasGlyphMetrics *atlas_metrics, R_Texture *atlas)
{
	for (U32 i = 32; i < 127; ++i)
	{
		R_Glyph *glyph = &font->glyphs[i];
		GlyphMetrics metrics = atlas_metrics->metrics[i];

		LoadGlyph(&metrics, glyph, atlas);

		if (glyph->size.y > font->max_height)
		{
			font->max_height = (F32)glyph->size.y;
		}

		if (glyph->size.x > font->max_width)
		{
			font->max_width = (F32)glyph->size.x;
		}
	}

	font->max_ascent = (S32)(0.83251953125 * GLYPH_SIZE);
	font->max_descent = (S32)(-0.30029296875 * GLYPH_SIZE);
	font->max_height = (S32)((1.1328125) * GLYPH_SIZE);

	for (S32 i = 1; i < FontIcon_COUNT; ++i)
	{
		GlyphMetrics metrics = atlas_metrics->icon_metrics[i];
		if (metrics.unicode)
		{
			R_Glyph *glyph = &font->icons[i];
			LoadGlyph(&metrics, glyph, atlas);
		}
	}
}

internal void InitFontFromMSDFAtlas(R_Font *font, String8 atlas_path, String8 csv_path)
{
	R_Texture font_atlas = R_LoadTexture(atlas_path);

	U8 *new_teture_data = OS_AllocMem((font_atlas.width * font_atlas.height) * 4);
	U8 *dst = new_teture_data;
	U8 *src = font_atlas.data;

	for (S32 i = 0;
			 i < font_atlas.width * font_atlas.height;
			 ++i)
	{
		for (U32 j = 0; j < 3; ++j)
		{
			dst[j] = src[j];
		}
		src += 3;
		dst += 4;
	}

	stbi_image_free(font_atlas.data);

	font_atlas.data = new_teture_data;

	OS_ReadFileResult file_read = OS_ReadEntireFile(csv_path);
	AtlasGlyphMetrics atlas_metrics = {0};
	ParseGlyphMetrics(&atlas_metrics, file_read.content);

	LoadFontFromMSDFAtlas(font, &atlas_metrics, &font_atlas);

	OS_FreeMemory(font_atlas.data);

	font->msdf = true;
}
