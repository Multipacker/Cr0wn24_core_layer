#ifndef UI_CORE_H
#define UI_CORE_H

typedef enum UI_SizeKind
{
	UI_SizeKind_Null,
	UI_SizeKind_Pixels,
	UI_SizeKind_TextContent,
	UI_SizeKind_Pct,
	UI_SizeKind_SumOfChildren,
	UI_SizeKind_Fill,
} UI_SizeKind;

typedef enum UI_BoxFlag
{
	UI_BoxFlag_Clickable = (1 << 0),
	UI_BoxFlag_ViewScroll = (1 << 1),
	UI_BoxFlag_DrawText = (1 << 2),
	UI_BoxFlag_DrawBorder = (1 << 3),
	UI_BoxFlag_DrawBackground = (1 << 4),
	UI_BoxFlag_DrawDropShadow = (1 << 5),
	UI_BoxFlag_Clip = (1 << 6),
	UI_BoxFlag_HotAnimation = (1 << 7),
	UI_BoxFlag_ActiveAnimation = (1 << 8),
	UI_BoxFlag_FocusAnimation = (1 << 9),
	UI_BoxFlag_FixedX = (1 << 10),
	UI_BoxFlag_FixedY = (1 << 11),

	UI_BoxFlag_AnimateX = (1 << 12),
	UI_BoxFlag_AnimateY = (1 << 13),
	UI_BoxFlag_AnimateWidth = (1 << 14),
	UI_BoxFlag_AnimateHeight = (1 << 15),
	UI_BoxFlag_AnimateScale = (1 << 16),
	UI_BoxFlag_AnimateStart = (1 << 17),

	UI_BoxFlag_SaveState = (1 << 18),

	UI_BoxFlag_CenterPos = (1 << 19),

	UI_BoxFlag_FixedPos = UI_BoxFlag_FixedX | UI_BoxFlag_FixedY,

	UI_BoxFlag_AnimateSize = UI_BoxFlag_AnimateWidth | UI_BoxFlag_AnimateHeight
} UI_BoxFlag;

typedef enum UI_Corner
{
	UI_Corner_TopLeft,
	UI_Corner_TopRight,
	UI_Corner_BottomLeft,
	UI_Corner_BottomRight,

	UI_Corner_COUNT,
} UI_Corner;

typedef enum Axis2
{
	Axis2_X,
	Axis2_Y,

	Axis2_COUNT,
} Axis2;

typedef enum UI_TextAlign
{
	UI_TextAlign_Center,
	UI_TextAlign_Left,
	UI_TextAlign_Right,

	UI_TextAlign_COUNT,
} UI_TextAlign;

typedef struct UI_Comm
{
	struct UI_Box *box;
	Vec2F32 mouse;
	Vec2F32 drag_delta;
	B32 clicked;
	B32 double_clicked;
	B32 right_clicked;
	B32 pressed;
	B32 released;
	B32 dragging;
	B32 hovering;
	S32 scroll;
	B32 focused;
	B32 enter;
	B32 page_up;
	B32 page_down;
} UI_Comm;

typedef struct UI_Size
{
	UI_SizeKind kind;
	F32 value;
	F32 strictness;
} UI_Size;

typedef struct UI_Key
{
	U32 key;
} UI_Key;

typedef struct UI_RectStyle
{
	Vec4F32 background_color;
	Vec4F32 hot_color;
	Vec4F32 active_color;
	Vec4F32 border_color;
	F32 border_thickness;
	Vec4F32 corner_radius;
	F32 edge_softness;

	struct UI_RectStyle *stack_next;
} UI_RectStyle;

typedef struct UI_TextStyle
{
	Vec4F32 text_color;
	S32 font_size;
	UI_TextAlign text_align;
	F32 text_edge_padding[Axis2_COUNT];
	R_IconIndex icon;

	struct UI_TextStyle *stack_next;
} UI_TextStyle;

typedef struct UI_LayoutStyle
{
	UI_Size pref_size[Axis2_COUNT];
	F32 relative_pos[Axis2_COUNT];
	Axis2 child_layout_axis;
	UI_Corner child_layout_corner;
	UI_BoxFlag flags;
	OS_Cursor hover_cursor;
	F32 scale;

	struct UI_LayoutStyle *stack_next;
} UI_LayoutStyle;

typedef struct UI_TextStyleStack
{
	UI_TextStyle *first;

	B32 auto_pop;
} UI_TextStyleStack;

typedef struct UI_RectStyleStack
{
	UI_RectStyle *first;

	B32 auto_pop;
} UI_RectStyleStack;

typedef struct UI_LayoutStack
{
	UI_LayoutStyle *first;

	B32 auto_pop;
} UI_LayoutStack;

typedef struct UI_Box
{
	struct UI_Box *first;
	struct UI_Box *last;
	struct UI_Box *next;
	struct UI_Box *prev;
	struct UI_Box *parent;

	// NOTE(hampus): Hash links
	struct UI_Box *hash_next;
	struct UI_Box *hash_prev;

	// NOTE(hampus): Key+generation info
	UI_Key key;

	// NOTE(hampus): Per-frame info provided by builders
	UI_BoxFlag flags;
	String8 display_string;
	UI_Size semantic_size[Axis2_COUNT];
	Axis2 child_layout_axis;
	UI_Corner child_layout_corner;
	U32 clip_rect_index;
	OS_Cursor hover_cursor;

	UI_RectStyle rect_style;
	UI_TextStyle text_style;

	// NOTE(hampus): Computed every frame
	F32 target_pos[Axis2_COUNT];
	F32 target_size[Axis2_COUNT];
	F32 target_scale;

	F32 start_size[Axis2_COUNT];

	F32 calc_rel_pos[Axis2_COUNT];
	F32 calc_pos[Axis2_COUNT];
	F32 calc_size[Axis2_COUNT];
	F32 calc_scale;

	RectF32 calc_rect;

	U32 last_frame_touched_index;
	U32 frame_created_index;

	// NOTE(hampus): Persistent data
	F32 hot_t;
	F32 active_t;

	B32 solved_size[Axis2_COUNT];

	// NOTE(hampus): Builder code data, core don't touch.
	// This is data the user doesn't have to care about, 
	// but that the builder needs.
	Vec2F32 scroll;
	B32 show_color_wheel;
	B32 show_expanded_tree;
	B32 *popup_bool;

	RectF32 clip_rect;
} UI_Box;

typedef struct UI_FreeBox
{
	struct UI_FreeBox *next;
} UI_FreeBox;

typedef struct UI_Theme
{
	Vec4F32 primary_color;
	Vec4F32 border_color;
	Vec4F32 window_color;
	Vec4F32 text_color;

	Vec4F32 error_color;
	Vec4F32 warning_color;
} UI_Theme;

typedef struct UI_ParentStackNode
{
	struct UI_ParentStackNode *next;
	UI_Box *box;
} UI_ParentStackNode;

typedef struct UI_ParentStack
{
	UI_ParentStackNode *first;
} UI_ParentStack;

typedef struct UI_State
{
	MemoryArena permanent_arena;
	MemoryArena frame_arena;

	size_t prev_frame_arena_used;

	R_Font *font;
	OS_Window *window;

	UI_FreeBox *first_free_box;
	U32 box_free_list_slots_used;
	U32 box_free_list_size;

	UI_Box *box_storage;
	U64 box_storage_count;

	UI_Box **box_hash_map;
	U64 box_hash_map_count;

	UI_ParentStack parent_stack;

	UI_Box *root;

	UI_Box *popup_root;

	UI_LayoutStack layout_stack;
	UI_RectStyleStack rect_style_stack;
	UI_TextStyleStack text_style_stack;
	String8Stack string_stack;

	UI_Key hot_key;
	UI_Key active_key;
	UI_Key focus_key;

	UI_Theme theme;

	B32 show_debug_lines;

	Vec2F32 mouse_pos;
	Vec2F32 old_mouse_pos;

	U32 frame;
	F32 animation_speed;

	OS_EventList *os_event_list;

	B32 inside_popup;
	B32 building_popup;

	F64 dt;
} UI_State;

// hampus: Layouting

#define UI_NextSize2(width, height)     {UI_LayoutStyle *layout = UI_GetAutoPopLayout(); layout->pref_size[Axis2_X] = width; layout->pref_size[Axis2_Y] = height; }
#define UI_PushSize2(width, height)     {UI_LayoutStyle *layout = UI_PushLayout(); layout->pref_size[Axis2_X] = width; layout->pref_size[Axis2_Y] = height; }
#define UI_PopSize2()                   (UI_PopLayout())

#define UI_NextSize(axis, size)         (UI_GetAutoPopLayout()->pref_size[axis] = size)
#define UI_PushSize(axis, size)         (UI_PushLayout()->pref_size[axis] = size)
#define UI_PopSize()                    (UI_PopLayout())

#define UI_NextRelativePos(a, p)        (UI_GetAutoPopLayout()->relative_pos[a] = p)
#define UI_PushRelativePos(a, p)        (UI_PushLayout()->relative_pos[a] = p)
#define UI_PopRelativePos()             (UI_PopLayout())

#define UI_NextRelativePos2(px, py)      (UI_GetAutoPopLayout()->relative_pos[Axis2_X] = px, UI_GetAutoPopLayout()->relative_pos[Axis2_Y] = py)
#define UI_PushRelativePos2(px, py)      (UI_PushLayout()->relative_pos[Axis2_X] = px, UI_PushLayout()->relative_pos[Axis2_Y] = py)
#define UI_PopRelativePos2()             (UI_PopLayout())

#define UI_NextChildLayoutAxis(p)       (UI_GetAutoPopLayout()->child_layout_axis = p)
#define UI_PushChildLayoutAxis(p)       (UI_PushLayout()->child_layout_axis = p)
#define UI_PopChildLayoutAxis()         (UI_PopLayout())

#define UI_NextChildLayoutCorner(p)       (UI_GetAutoPopLayout()->child_layout_corner = p)
#define UI_PushChildLayoutCorner(p)       (UI_PushLayout()->child_layout_corner = p)
#define UI_PopChildLayoutCorner()         (UI_PopLayout())

#define UI_NextBoxFlags(f)       (UI_GetAutoPopLayout()->flags = f)
#define UI_PushBoxFlags(f)       (UI_PushLayout()->flags = f)
#define UI_PopBoxFlags()         (UI_PopLayout())

#define UI_NextHoverCursor(x)       (UI_GetAutoPopLayout()->hover_cursor = x)
#define UI_PushHoverCursor(x)       (UI_PushLayout()->hover_cursor = x)
#define UI_PopHoverCursor()         (UI_PopLayout())

#define UI_NextScale(x)       (UI_GetAutoPopLayout()->scale = x)
#define UI_PushScale(x)       (UI_PushLayout()->scale = x)
#define UI_PopScale()         (UI_PopLayout())

// hampus: Text style

#define UI_NextTextEdgePadding(axis, size)  (UI_GetAutoPopTextStyle()->text_edge_padding[axis] = size)
#define UI_PushTextEdgePadding(axis, size)  (UI_PushTextStyle()->text_edge_padding[axis] = size)
#define UI_PopPadding()             (UI_PopLayout())

#define UI_NextTextColor(x)         (UI_GetAutoPopTextStyle()->text_color = x)
#define UI_PushTextColor(x)         (UI_PushTextStyle()->text_color = x)
#define UI_PopTextColor()           (UI_PopTextStyle())

#define UI_NextTextAlign(x)         (UI_GetAutoPopTextStyle()->text_align = x)
#define UI_PushTextAlign(x)         (UI_PushTextStyle(), x)
#define UI_PopTextAlign()           (UI_PopTextStyle())

#define UI_NextFontSize(x)          (UI_GetAutoPopTextStyle()->font_size = x)
#define UI_PushFontSize(x)          (UI_PushTextStyle()->font_size = x)
#define UI_PopFontSize()            (UI_PopTextStyle())

#define UI_NextIcon(x)          (UI_GetAutoPopTextStyle()->icon = x)
#define UI_PushIcon(x)          (UI_PushTextStyle()->icon = x)
#define UI_PopIcon()            (UI_PopTextStyle())

// hampus: Rect style

#define UI_NextBackgroundColor(x)   (UI_GetAutoPopRectStyle()->background_color = x)
#define UI_PushBackgroundColor(x)   (UI_PushRectStyle()->background_color = x)
#define UI_PopBackgroundColor()     (UI_PopRectStyle())

#define UI_NextHotColor(x)          (UI_GetAutoPopRectStyle()->hot_color = x)
#define UI_PushHotColor(x)          (UI_PushRectStyle()->hot_color = x)
#define UI_PopHotColor()            (UI_PopRectStyle())

#define UI_NextActiveColor(x)       (UI_GetAutoPopRectStyle()->active_color = x)
#define UI_PushActiveColor(x)       (UI_PushRectStyle()->active_color = x)
#define UI_PopActiveColor()         (UI_PopRectStyle())

#define UI_NextBorderColor(x)       (UI_GetAutoPopRectStyle()->border_color = x)
#define UI_PushBorderColor(x)       (UI_PushRectStyle()->border_color = x)
#define UI_PopBorderColor()         (UI_PopRectStyle())

#define UI_NextBorderThickness(x)    (UI_GetAutoPopRectStyle()->border_thickness = x)
#define UI_PushBorderThickness(x)    (UI_PushRectStyle()->border_thickness = x)
#define ui_pop_border_thickness()      (UI_PopRectStyle())

#define UI_NextCornerRadius(c, x)   (UI_GetAutoPopRectStyle()->corner_radius.m[c] = x)
#define UI_PushCornerRadius(c, x)   (UI_PushRectStyle()->corner_radius.m[c] = x)
#define UI_PopCornerRadius()        (UI_PopRectStyle())

#define UI_NextCornerRadius4(x)  {UI_RectStyle *layout = UI_GetAutoPopRectStyle(); layout->corner_radius.m[0] = x, layout->corner_radius.m[1] = x, layout->corner_radius.m[2] = x, layout->corner_radius.m[3] = x;}
#define UI_PushCornerRadius4(x)  {UI_RectStyle *layout = UI_PushRectStyle(); layout->corner_radius.m[0] = x, layout->corner_radius.m[1] = x, layout->corner_radius.m[2] = x, layout->corner_radius.m[3] = x;}
#define UI_PopCornerRadius4()       (UI_PopRectStyle())

#define UI_NextTextEdgeSoftness(x)      (UI_GetAutoPopRectStyle()->edge_softness = x)
#define UI_PushEdgeSoftness(x)      (UI_PushRectStyle()->edge_softness = x)
#define UI_PopEdgeSoftness()        (UI_PopRectStyle())

// hampus: Helper functions

#define UI_IsHot(box)       (UI_KeyMatch(box->key, ui_state.hot_key))
#define UI_IsActive(box)    (UI_KeyMatch(box->key, ui_state.active_key))
#define UI_IsFocused(box)   (UI_KeyMatch(box->key, ui_state.focus_key))

#define UI_WasHot(box)      (UI_KeyMatch(box->key, ui_state.prev_hot_key))
#define UI_WasActive(box)   (UI_KeyMatch(box->key, ui_state.prev_active_key))
#define UI_WasFocused(box)  (UI_KeyMatch(box->key, ui_state.prev_focus_key))

#define UI_DeferLoop(begin, end) for(int _i_ = ((begin), 0); !_i_; _i_ += 1, (end))

#define UI_Parent(b) UI_DeferLoop(UI_PushParent(b), UI_PopParent())

#endif