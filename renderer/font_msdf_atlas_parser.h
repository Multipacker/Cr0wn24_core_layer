#ifndef FONT_MSDF_ATLAS_PARSER_H
#define FONT_MSDF_ATLAS_PARSER_H

typedef struct GlyphMetrics
{
	U32 unicode;
	F32 advance;
	RectF32 plane_bounds;
	RectF32 atlas_bounds;
} GlyphMetrics;

#endif