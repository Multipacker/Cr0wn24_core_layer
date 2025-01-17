#ifndef RENDERER_H
#define RENDERER_H

typedef enum R_IconIndex
{
	R_IconIndex_Check = 183,
	R_IconIndex_Cross = 184,
	R_IconIndex_Cogwheel = 189,
	R_IconIndex_Angle_Down = 215,
	R_IconIndex_Angle_Up = 216,
	R_IconIndex_Angle_Right = 217,
	R_IconIndex_Angle_Left = 218,
	R_IconIndex_Pause = 225,
	R_IconIndex_Play = 226,
	R_IconIndex_SkipForward = 229,
	R_IconIndex_SkipBackward = 230,
} R_IconIndex;

typedef union RectS32
{
	struct
	{
		S32 x0;
		S32 y0;
		S32 x1;
		S32 y1;
	};

	struct
	{
		Vec2S32 min;
		Vec2S32 max;
	};

} RectS32;

typedef union RectF32
{
	struct
	{
		F32 x0;
		F32 y0;
		F32 x1;
		F32 y1;
	};

	struct
	{
		Vec2F32 min;
		Vec2F32 max;
	};

} RectF32;

typedef struct R_Handle
{
	U64 a;
	U64 b;
} R_Handle;

typedef struct R_Texture
{
	Vec2S32 dim;
	Vec2F32 src_p0;
	Vec2F32 src_p1;
	R_Handle handle;
} R_Texture;

typedef struct Rect
{
	Vec2F32 min;
	Vec2F32 max;
	Vec2F32 min_uv;
	Vec2F32 max_uv;
	Vec4F32 color;
	Vec4F32 corner_radius;
	F32 edge_softness;
	F32 omit_texture;
	F32 is_text;
	F32 border_thickness;
} Rect;

typedef struct LineVertex
{
	Vec2F32 inst_pos;
	Vec2F32 dir;
	F32 thickness;
	Vec4F32 color;
} LineVertex;

typedef struct R_RectParams
{
	Vec4F32 color;
	R_Texture texture;
	B32 text;
	union
	{
		Vec4F32 corner_radius;
		struct
		{
			F32 r00, r01, r10, r11;
		};
	};
	F32 edge_softness;
	F32 border_thickness;
} R_RectParams;

typedef enum Batch2DInstKind
{
	Batch2DInstKind_Line,
	Batch2DInstKind_Rect,

	Batch2DInstKind_COUNT
} Batch2DInstKind;

typedef struct Batch2D
{
	U8 data[4096 * sizeof(Rect)];
	U32 inst_count;
	R_Texture tex;
	RectF32 clip_rect;
	Batch2DInstKind inst_kind;
} Batch2D;

typedef struct Batch2DNode
{
	struct Batch2DNode *next;
	struct Batch2DNode *prev;
	Batch2D *batch;
} Batch2DNode;

typedef struct Batch2DList
{
	Batch2DNode *first;
	Batch2DNode *last;
} Batch2DList;

typedef struct RenderData
{
	Batch2DList *batch_list;
} RenderData;

typedef struct ClipRectNode
{
	struct ClipRectNode *next;
	struct ClipRectNode *prev;

	RectF32 rect;
} ClipRectNode;

typedef struct ClipRectList
{
	ClipRectNode *first;
	ClipRectNode *last;
} ClipRectList, ClipRectStack;

#define GPU_LOAD_TEXTURE(name) R_Handle name(void *data, S32 width, S32 height);
typedef GPU_LOAD_TEXTURE(GPULoadTextureProc);

typedef struct R_State
{
	MemoryArena *arena;
	RenderData render_data;

	R_Texture white_texture;

	ClipRectStack clip_rect_stack;

	GPULoadTextureProc *GPULoadTexture;

	Vec2S32 render_dim;
} R_State;

typedef struct R_Glyph
{
	R_Texture texture;
	S32 advance;
	Vec2S32 size;
	Vec2S32 bearing;
} R_Glyph;

typedef struct TextureAtlas
{
	Vec2S32 dim;
	R_Texture texture;
	R_Texture *textures;
} TextureAtlas;

typedef struct R_LoadedBitmap
{
	Vec2S32 dim;
	void *data;
} R_LoadedBitmap;

typedef struct R_Font
{
	R_Glyph glyphs[512];
	F32 max_height;
	F32 max_width;
	S32 max_ascent;
	S32 max_descent;
	S32 height;
	TextureAtlas atlas;
} R_Font;

#define R_PushRect(min, max, ...) R_PushRect_(min, max, (R_RectParams){.texture = r_state->white_texture, .color = V4(1.0f, 1.0f, 1.0f, 1.0f), __VA_ARGS__})

#ifndef CORE_PATH
#define CORE_PATH "../code/core_layer/"
#endif

#define RESOURCE_PATH Glue(CORE_PATH, "res/") 

#define CORE_RESOURCE(str) Str8Lit(Glue(RESOURCE_PATH, str))

#endif