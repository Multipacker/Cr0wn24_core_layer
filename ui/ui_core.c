global UI_State *ui_state;

// hampus: Color themes

internal UI_Theme
UI_DefaultTheme()
{
	UI_Theme theme = {0};

	theme.primary_color = V4(0.15f, 0.15f, 0.15f, 1.0f);
	theme.border_color = V4(0.9f, 0.9f, 0.9f, 1.0f);
	theme.text_color = V4(0.9f, 0.9f, 0.9f, 1.0f);
	theme.window_color = V4(0.1f, 0.1f, 0.1f, 1.0f);

	theme.error_color = V4(0.8f, 0.0f, 0.0f, 1.0f);
	theme.warning_color = V4(0.8f, 0.8f, 0.0f, 1.0f);

	return(theme);
}

// hampus: Helper internals to retrieve state

internal UI_Box *
UI_TopParent()
{
	if (ui_state->parent_stack.first)
	{
		return(ui_state->parent_stack.first->box);
	}
	else
	{
		return(0);
	}
}

internal MemoryArena *
UI_FrameArena()
{
	return(&ui_state->frame_arena);
}

internal MemoryArena *
UI_PermanentArena()
{
	return(&ui_state->permanent_arena);
}

// hampus: internals for managing the style, text and layout stacks

internal UI_LayoutStyle *
UI_TopLayout()
{
	return(ui_state->layout_stack.first);
}

internal UI_RectStyle *
UI_TopRectStyle()
{
	return(ui_state->rect_style_stack.first);
}

internal UI_TextStyle *
UI_TopTextStyle()
{
	return(ui_state->text_style_stack.first);
}

// hampus: Layout stack

internal void
UI_PushParent(UI_Box *box)
{
	UI_ParentStackNode *node = PushStruct(UI_FrameArena(), UI_ParentStackNode);
	node->box = box;
	StackPush(ui_state->parent_stack.first, node);
	if (box->flags & UI_BoxFlag_Clip)
	{
		R_PushClipRect(box->calc_rect);
	}
}

internal void
UI_PopParent()
{
	if (ui_state->parent_stack.first->box->flags & UI_BoxFlag_Clip)
	{
		R_PopClipRect();
	}
	StackPop(ui_state->parent_stack.first);
}

internal UI_LayoutStyle *
UI_PushLayout()
{
	UI_LayoutStyle *layout = PushStruct(&ui_state->frame_arena, UI_LayoutStyle);
	if (ui_state->layout_stack.first)
	{
		CopyStruct(layout, ui_state->layout_stack.first);
	}
	layout->stack_next = ui_state->layout_stack.first;
	ui_state->layout_stack.first = layout;

	return(layout);
}

internal void
UI_PopLayout()
{
	ui_state->layout_stack.first = ui_state->layout_stack.first->stack_next;
}

internal UI_LayoutStyle *
UI_GetAutoPopLayout()
{
	UI_LayoutStyle *layout = UI_TopLayout();
	if (!ui_state->layout_stack.auto_pop)
	{
		layout = UI_PushLayout();
		ui_state->layout_stack.auto_pop = true;
	}
	return(layout);
}

// hampus: Rect style stack

internal UI_RectStyle *
UI_PushRectStyle()
{
	UI_RectStyle *rect_style = PushStruct(&ui_state->frame_arena, UI_RectStyle);
	UI_RectStyle *first = UI_TopRectStyle();
	if (first)
	{
		CopyStruct(rect_style, first);
	}
	rect_style->stack_next = first;
	ui_state->rect_style_stack.first = rect_style;

	return(rect_style);
}

internal void UI_PopRectStyle()
{
	ui_state->rect_style_stack.first = ui_state->rect_style_stack.first->stack_next;
}

internal UI_RectStyle *UI_GetAutoPopRectStyle()
{
	UI_RectStyle *rect_style = UI_TopRectStyle();
	if (!ui_state->rect_style_stack.auto_pop)
	{
		rect_style = UI_PushRectStyle();
		ui_state->rect_style_stack.auto_pop = true;
	}
	return(rect_style);
}

// hampus: Text style stack

internal UI_TextStyle *UI_PushTextStyle()
{
	UI_TextStyle *text_style = PushStruct(&ui_state->frame_arena, UI_TextStyle);
	UI_TextStyle *first = UI_TopTextStyle();
	if (first)
	{
		CopyStruct(text_style, first);
	}
	text_style->stack_next = first;
	ui_state->text_style_stack.first = text_style;

	return(text_style);
}

internal void UI_PopTextStyle()
{
	ui_state->text_style_stack.first = ui_state->text_style_stack.first->stack_next;
}

internal UI_TextStyle *UI_GetAutoPopTextStyle()
{
	UI_TextStyle *text_style = UI_TopTextStyle();
	if (!ui_state->text_style_stack.auto_pop)
	{
		text_style = UI_PushTextStyle();
		ui_state->text_style_stack.auto_pop = true;
	}
	return(text_style);
}

// hampus: Keying

internal void UI_PushString(String8 string)
{
	String8StackNode *node = PushStruct(UI_FrameArena(), String8StackNode);
	node->string = string;
	StackPush(ui_state->string_stack.first, node);
}

internal void UI_PushStringF(String8 string)
{
	S32 triple_pound_pos = Str8FindSubStr8(string, Str8Lit("###"));
	if (triple_pound_pos >= 0)
	{
		String8 new_string = {0};
		new_string.size = string.size - (triple_pound_pos + 2);
		new_string.str = string.str + triple_pound_pos;
		UI_PushString(new_string);
	}
	else
	{
		UI_PushString(string);

	}
}

internal void UI_PopString()
{
	StackPop(ui_state->string_stack.first);
}

internal B32 UI_KeyMatch(UI_Key a, UI_Key b)
{
	return(a.key == b.key);
}

internal B32 UI_KeyIsNull(UI_Key key)
{
	return(key.key == 0);
}

global U32 crc32_loookup_table[256] = {
	0x00000000,0x77073096,0xEE0E612C,0x990951BA,0x076DC419,0x706AF48F,0xE963A535,0x9E6495A3,0x0EDB8832,0x79DCB8A4,0xE0D5E91E,0x97D2D988,0x09B64C2B,0x7EB17CBD,0xE7B82D07,0x90BF1D91,
	0x1DB71064,0x6AB020F2,0xF3B97148,0x84BE41DE,0x1ADAD47D,0x6DDDE4EB,0xF4D4B551,0x83D385C7,0x136C9856,0x646BA8C0,0xFD62F97A,0x8A65C9EC,0x14015C4F,0x63066CD9,0xFA0F3D63,0x8D080DF5,
	0x3B6E20C8,0x4C69105E,0xD56041E4,0xA2677172,0x3C03E4D1,0x4B04D447,0xD20D85FD,0xA50AB56B,0x35B5A8FA,0x42B2986C,0xDBBBC9D6,0xACBCF940,0x32D86CE3,0x45DF5C75,0xDCD60DCF,0xABD13D59,
	0x26D930AC,0x51DE003A,0xC8D75180,0xBFD06116,0x21B4F4B5,0x56B3C423,0xCFBA9599,0xB8BDA50F,0x2802B89E,0x5F058808,0xC60CD9B2,0xB10BE924,0x2F6F7C87,0x58684C11,0xC1611DAB,0xB6662D3D,
	0x76DC4190,0x01DB7106,0x98D220BC,0xEFD5102A,0x71B18589,0x06B6B51F,0x9FBFE4A5,0xE8B8D433,0x7807C9A2,0x0F00F934,0x9609A88E,0xE10E9818,0x7F6A0DBB,0x086D3D2D,0x91646C97,0xE6635C01,
	0x6B6B51F4,0x1C6C6162,0x856530D8,0xF262004E,0x6C0695ED,0x1B01A57B,0x8208F4C1,0xF50FC457,0x65B0D9C6,0x12B7E950,0x8BBEB8EA,0xFCB9887C,0x62DD1DDF,0x15DA2D49,0x8CD37CF3,0xFBD44C65,
	0x4DB26158,0x3AB551CE,0xA3BC0074,0xD4BB30E2,0x4ADFA541,0x3DD895D7,0xA4D1C46D,0xD3D6F4FB,0x4369E96A,0x346ED9FC,0xAD678846,0xDA60B8D0,0x44042D73,0x33031DE5,0xAA0A4C5F,0xDD0D7CC9,
	0x5005713C,0x270241AA,0xBE0B1010,0xC90C2086,0x5768B525,0x206F85B3,0xB966D409,0xCE61E49F,0x5EDEF90E,0x29D9C998,0xB0D09822,0xC7D7A8B4,0x59B33D17,0x2EB40D81,0xB7BD5C3B,0xC0BA6CAD,
	0xEDB88320,0x9ABFB3B6,0x03B6E20C,0x74B1D29A,0xEAD54739,0x9DD277AF,0x04DB2615,0x73DC1683,0xE3630B12,0x94643B84,0x0D6D6A3E,0x7A6A5AA8,0xE40ECF0B,0x9309FF9D,0x0A00AE27,0x7D079EB1,
	0xF00F9344,0x8708A3D2,0x1E01F268,0x6906C2FE,0xF762575D,0x806567CB,0x196C3671,0x6E6B06E7,0xFED41B76,0x89D32BE0,0x10DA7A5A,0x67DD4ACC,0xF9B9DF6F,0x8EBEEFF9,0x17B7BE43,0x60B08ED5,
	0xD6D6A3E8,0xA1D1937E,0x38D8C2C4,0x4FDFF252,0xD1BB67F1,0xA6BC5767,0x3FB506DD,0x48B2364B,0xD80D2BDA,0xAF0A1B4C,0x36034AF6,0x41047A60,0xDF60EFC3,0xA867DF55,0x316E8EEF,0x4669BE79,
	0xCB61B38C,0xBC66831A,0x256FD2A0,0x5268E236,0xCC0C7795,0xBB0B4703,0x220216B9,0x5505262F,0xC5BA3BBE,0xB2BD0B28,0x2BB45A92,0x5CB36A04,0xC2D7FFA7,0xB5D0CF31,0x2CD99E8B,0x5BDEAE1D,
	0x9B64C2B0,0xEC63F226,0x756AA39C,0x026D930A,0x9C0906A9,0xEB0E363F,0x72076785,0x05005713,0x95BF4A82,0xE2B87A14,0x7BB12BAE,0x0CB61B38,0x92D28E9B,0xE5D5BE0D,0x7CDCEFB7,0x0BDBDF21,
	0x86D3D2D4,0xF1D4E242,0x68DDB3F8,0x1FDA836E,0x81BE16CD,0xF6B9265B,0x6FB077E1,0x18B74777,0x88085AE6,0xFF0F6A70,0x66063BCA,0x11010B5C,0x8F659EFF,0xF862AE69,0x616BFFD3,0x166CCF45,
	0xA00AE278,0xD70DD2EE,0x4E048354,0x3903B3C2,0xA7672661,0xD06016F7,0x4969474D,0x3E6E77DB,0xAED16A4A,0xD9D65ADC,0x40DF0B66,0x37D83BF0,0xA9BCAE53,0xDEBB9EC5,0x47B2CF7F,0x30B5FFE9,
	0xBDBDF21C,0xCABAC28A,0x53B39330,0x24B4A3A6,0xBAD03605,0xCDD70693,0x54DE5729,0x23D967BF,0xB3667A2E,0xC4614AB8,0x5D681B02,0x2A6F2B94,0xB40BBE37,0xC30C8EA1,0x5A05DF1B,0x2D02EF8D,
};

internal U32 UI_HashStringStack(String8Stack string_stack, U32 seed)
{
	String8StackNode *node = string_stack.first;
	U32 crc = ~seed;
	while (node)
	{
		String8 string = node->string;
		U8* data = (U8 *)string.str;
		U32* crc32_lut = crc32_loookup_table;
		while (string.size-- != 0)
			crc = (crc >> 8) ^ crc32_lut[(crc & 0xFF) ^ *data++];

		node = node->next;
	}
	return ~crc;
}

internal UI_Key UI_KeyFromString(String8 string)
{
	UI_Key result = {0};

	if (string.size > 0)
	{
		S32 triple_pound_pos = Str8FindSubStr8(string, Str8Lit("###"));
		if (triple_pound_pos >= 0)
		{
			String8 new_string = {0};
			new_string.size = 3;
			new_string.str = string.str + triple_pound_pos;
			UI_PushString(new_string);
		}
		else
		{
			UI_PushString(string);
		}

		result.key = UI_HashStringStack(ui_state->string_stack, 1);
		UI_PopString();
	}

	return(result);
}

// hampus: Size kinds

internal UI_Size UI_Null()
{
	UI_Size result = {0};

	result.kind = UI_SizeKind_Null;

	return(result);
}

internal UI_Size UI_Pixels(F32 value)
{
	UI_Size result = {0};

	result.kind = UI_SizeKind_Pixels;
	result.value = value;

	return(result);
}

internal UI_Size UI_Em(F32 value)
{
	return(UI_Pixels(value * ui_state->font->max_height));
}

internal UI_Size UI_TextContent()
{
	UI_Size result = {0};

	result.kind = UI_SizeKind_TextContent;

	return(result);
}

internal UI_Size UI_Pct(F32 value)
{
	UI_Size result = {0};

	result.value = value;
	result.kind = UI_SizeKind_Pct;

	return(result);
}

internal UI_Size UI_Fill()
{
	UI_Size result = {0};

	result.kind = UI_SizeKind_Fill;

	return(result);
}

internal UI_Size UI_SumOfChildren()
{
	UI_Size result = {0};

	result.kind = UI_SizeKind_SumOfChildren;

	return(result);
}

// hampus: Box implementation

internal B32 UI_BoxHasFlag(UI_Box *box, UI_BoxFlag flag)
{
	return ((box->flags & flag) != 0);
}

internal UI_Comm UI_CommFromBox(UI_Box *box)
{
	UI_Comm result = {0};

	Vec2F32 mouse = ui_state->mouse_pos;

	B32 inside_box = R_PointInsideRect(mouse, box->calc_rect);
	B32 inside_parent_box = true;
	if (box->parent)
	{
		if (UI_BoxHasFlag(box->parent, UI_BoxFlag_Clip))
		{
			inside_parent_box = R_PointInsideRect(mouse, box->parent->calc_rect);
		}
	}

	OS_EventList *event_list = ui_state->os_event_list;

	B32 previous_still_active = false;
	if (!UI_KeyIsNull(ui_state->active_key))
	{
		if (!UI_KeyMatch(ui_state->active_key, box->key))
		{
			previous_still_active = true;
		}
	}

	B32 capture_input = false;

	if (!ui_state->inside_popup ||
		ui_state->building_popup)
	{
		if (!previous_still_active)
		{
			if (inside_box && inside_parent_box)
			{
				capture_input = true;
			}
		}
	}

	if (capture_input)
	{
		result.scroll = -OS_GetScroll();
		result.hovering = true;
		ui_state->hot_key = box->key;
		for (OS_EventNode *node = event_list->first;
			 node != 0;
			 node = node->next)
		{
			OS_Event event = node->event;
			switch (event.type)
			{
				case OS_EventType_KeyPress:
				{
					if (event.key == OS_Key_MouseLeft)
					{
						result.pressed = true;

						ui_state->active_key = box->key;
						ui_state->focus_key = box->key;

						DLL_Remove(event_list->first, event_list->last, node);
					}
				} break;

				case OS_EventType_KeyRelease:
				{
					if (event.key == OS_Key_MouseLeft)
					{
						result.clicked = true;

						DLL_Remove(event_list->first, event_list->last, node);
					}
				} break;
			}
		}
	}

	if (UI_IsFocused(box))
	{
		for (OS_EventNode *node = event_list->first;
			 node != 0;
			 node = node->next)
		{
			OS_Event event = node->event;
			switch (event.type)
			{
				case OS_EventType_KeyPress:
				{
					if (event.key == OS_Key_Return ||
						event.key == OS_Key_Escape)
					{
						result.enter = true;
						DLL_Remove(event_list->first, event_list->last, node);
					}
					else if (event.key == OS_Key_PageUp)
					{
						result.page_up = true;
						DLL_Remove(event_list->first, event_list->last, node);
					}
					else if (event.key == OS_Key_PageDown)
					{
						result.page_down = true;
						DLL_Remove(event_list->first, event_list->last, node);
					}
				} break;
			}
		}
	}

	if (UI_IsActive(box))
	{
		result.dragging = true;
		ui_state->hot_key = box->key;
	}

	result.mouse = mouse;

	result.drag_delta = V2SubV2(mouse, ui_state->old_mouse_pos);

	result.box = box;

	return(result);
}

internal String8
UI_CheckStringForPound(String8 string)
{
	String8 result = string;
	S32 double_pound_pos = Str8FindSubStr8(string, Str8Lit("##"));
	if (double_pound_pos >= 0)
	{
		result = PushStr8Size(UI_FrameArena(), (char *)string.str, double_pound_pos);
	}
	return(result);
}

internal void UI_EquipBoxWithDisplayString(UI_Box *box, String8 string)
{
	String8 display_string = UI_CheckStringForPound(string);
	box->display_string = display_string;
}

internal UI_Box *
UI_BoxAlloc()
{
	UI_Box *result = (UI_Box *)ui_state->first_free_box;

	ui_state->first_free_box = ui_state->first_free_box->next;

	ui_state->box_free_list_slots_used++;

	ZeroArray(result, 1);

	return(result);
}

internal void UI_BoxFree(UI_Box *box)
{
	UI_FreeBox *free_box = (UI_FreeBox *)box;
	free_box->next = ui_state->first_free_box;
	ui_state->first_free_box = free_box;

	ui_state->box_free_list_slots_used--;
}

internal void UI_PushBoxInTree(UI_Box *box)
{
	box->parent = UI_TopParent();
	DLL_PushBack(box->parent->first, box->parent->last, box);
}

internal UI_Box *UI_BoxFromKey(UI_Key key)
{
	UI_Box *result = 0;

	if (!UI_KeyIsNull(key))
	{
		U64 slot_index = key.key % ui_state->box_hash_map_count;
		for (result = ui_state->box_hash_map[slot_index];
			 result != 0;
			 result = result->hash_next)
		{
			if (UI_KeyMatch(key, result->key))
			{
				break;
			}
		}
	}

	return(result);
}

internal UI_Box *UI_BoxMake(UI_BoxFlag flags, String8 string)
{
	UI_Key key = {0};

	key = UI_KeyFromString(string);

	UI_Box *result = UI_BoxFromKey(key);

	if (!result)
	{
		result = UI_BoxAlloc();
		result->key = key;

		U32 slot = result->key.key % ui_state->box_hash_map_count;

		UI_Box *hash_box = ui_state->box_hash_map[slot];
		if (hash_box)
		{
			hash_box->hash_prev = result;
			result->hash_next = hash_box;

			ui_state->box_hash_map[slot] = result;
		}
		else
		{
			ui_state->box_hash_map[slot] = result;
		}

		result->frame_created_index = ui_state->frame;
	}

	UI_Box *parent = UI_TopParent();

	result->first = 0;
	result->last = 0;
	result->next = 0;
	result->prev = 0;
	if (parent)
	{
		DLL_PushBack(parent->first, parent->last, result);
		result->parent = parent;
	}
	else
	{
		ui_state->root = parent;
	}

	result->flags = flags;

	UI_LayoutStyle *layout = UI_TopLayout();

	// NOTE(hampus): Layouting

	{
		result->flags |= layout->flags;

		if (result->flags & UI_BoxFlag_FixedX)
		{
			result->calc_rel_pos[Axis2_X] = layout->relative_pos[Axis2_X];
		}

		if (result->flags & UI_BoxFlag_FixedY)
		{
			result->calc_rel_pos[Axis2_Y] = layout->relative_pos[Axis2_Y];
		}

		result->child_layout_axis = layout->child_layout_axis;
		result->child_layout_corner = layout->child_layout_corner;

		result->semantic_size[Axis2_X] = layout->pref_size[Axis2_X];
		result->semantic_size[Axis2_Y] = layout->pref_size[Axis2_Y];

		result->hover_cursor = layout->hover_cursor;

		result->target_scale = layout->scale;

		result->rect_style = *UI_TopRectStyle();
		result->text_style = *UI_TopTextStyle();

		result->clip_rect = r_state->clip_rect_stack.first->rect;

		if (ui_state->layout_stack.auto_pop)
		{
			UI_PopLayout();
			ui_state->layout_stack.auto_pop = false;
		}
		if (ui_state->rect_style_stack.auto_pop)
		{
			UI_PopRectStyle();
			ui_state->rect_style_stack.auto_pop = false;
		}
		if (ui_state->text_style_stack.auto_pop)
		{
			UI_PopTextStyle();
			ui_state->text_style_stack.auto_pop = false;
		}
	}

	result->last_frame_touched_index = ui_state->frame;

	return(result);
}

// hampus: Root parent

internal B32 UI_BeginPopup(B32 *b)
{
	if (*b)
	{
		R_PushClipRect(ui_state->root->calc_rect);

		UI_PushParent(ui_state->popup_root);
		UI_Box *base = UI_BoxMake(0, Str8Lit("PopupBase"));
		UI_PushParent(base);

		base->popup_bool = b;

		UI_Box *first = ui_state->popup_root->first;
		while (first)
		{
			if (first != base)
			{
				*first->popup_bool = false;
			}
			first = first->next;
		}

		R_PushClipRect(base->calc_rect);
		ui_state->building_popup = true;
	}
	else
	{

		if (ui_state->layout_stack.auto_pop)
		{
			UI_PopLayout();
			ui_state->layout_stack.auto_pop = false;
		}
		if (ui_state->rect_style_stack.auto_pop)
		{
			UI_PopRectStyle();
			ui_state->rect_style_stack.auto_pop = false;
		}
		if (ui_state->text_style_stack.auto_pop)
		{
			UI_PopTextStyle();
			ui_state->text_style_stack.auto_pop = false;
		}
	}

	return(*b);
}

internal void UI_EndPopup()
{
	R_PopClipRect();
	R_PopClipRect();
	UI_PopParent();
	UI_PopParent();

	ui_state->building_popup = false;
}

internal void UI_CreateRootParent()
{
	UI_NextSize2(UI_Pixels((F32)r_state->render_dim.x), UI_Pixels((F32)r_state->render_dim.y));
	UI_Box *root = UI_BoxMake(0, Str8Lit("Root"));
	ui_state->root = root;
}

// hampus: Initialization and begin/end of ui frame

internal void UI_Begin(UI_Theme theme, OS_EventList *os_event_list, F64 dt)
{
	ui_state->dt = dt;

	ui_state->os_event_list = os_event_list;

	ui_state->mouse_pos = OS_GetMousePos(ui_state->window);

	B32 left_mouse_pressed = false;
	B32 left_mouse_released = false;
	OS_EventNode *event_node = os_event_list->first;
	while (event_node)
	{
		OS_Event event = event_node->event;
		if (event.type == OS_EventType_KeyRelease)
		{
			if (event.key == OS_Key_MouseLeft)
			{
				left_mouse_released = true;
			}
		}
		else if (event.type == OS_EventType_KeyPress)
		{
			if (event.key == OS_Key_MouseLeft)
			{
				left_mouse_pressed = true;
			}
		}
		event_node = event_node->next;
	}

	ui_state->inside_popup = false;

	if (ui_state->popup_root)
	{
		UI_Box *first_popup_child = ui_state->popup_root->first;
		while (first_popup_child)
		{
			if (R_PointInsideRect(ui_state->mouse_pos, first_popup_child->calc_rect))
			{
				ui_state->inside_popup = true;
				break;
			}
			first_popup_child = first_popup_child->next;
		}
	}

	for (U32 i = 0; i < ui_state->box_hash_map_count; ++i)
	{
		UI_Box *box = ui_state->box_hash_map[i];
		while (box)
		{
			UI_Box *next = box->hash_next;
			if (!UI_KeyIsNull(box->key))
			{
				if (UI_KeyMatch(ui_state->active_key, box->key))
				{
					if (left_mouse_released)
					{
						ui_state->active_key.key = 0;
					}
				}
				if (UI_KeyMatch(ui_state->focus_key, box->key))
				{
					if (left_mouse_pressed)
					{
						ui_state->focus_key.key = 0;
					}
				}
			}
			if (!(box->flags & UI_BoxFlag_SaveState))
			{
				if (box->last_frame_touched_index < (ui_state->frame - 1) ||
					UI_KeyIsNull(box->key))
				{

					if (box == ui_state->box_hash_map[i])
					{
						ui_state->box_hash_map[i] = box->hash_next;
					}

					if (box->hash_next)
					{
						box->hash_next->hash_prev = box->hash_prev;
					}

					if (box->hash_prev)
					{
						box->hash_prev->hash_next = box->hash_next;
					}

					box->hash_next = 0;
					box->hash_prev = 0;
					UI_BoxFree(box);
				}
			}

			box = next;
		}
	}

	if (!UI_KeyIsNull(ui_state->hot_key))
	{
		UI_Box *hover_box = UI_BoxFromKey(ui_state->hot_key);
		OS_SetHoverCursor(hover_box->hover_cursor);
	}
	else
	{
		OS_SetHoverCursor(OS_Cursor_Arrow);
	}

	ui_state->hot_key.key = 0;
	ui_state->parent_stack.first = 0;
	ui_state->layout_stack.first = 0;
	ui_state->rect_style_stack.first = 0;
	ui_state->text_style_stack.first = 0;

	UI_LayoutStyle *layout = UI_PushLayout();

	layout->scale = 1.0f;

	// hampus: Set up theme

	ui_state->theme = theme;

	UI_TextStyle *text_style = UI_PushTextStyle();

	text_style->text_color = theme.text_color;

	text_style->font_size = ui_state->font->height;

	UI_RectStyle *rect_style = UI_PushRectStyle();

	rect_style->background_color = theme.primary_color;
	rect_style->hot_color = V4MulF32(rect_style->background_color, 2.0f);
	rect_style->active_color = V4MulF32(rect_style->background_color, 3.0f);
	rect_style->border_color = theme.border_color;
	rect_style->border_thickness = 0.5f;

	F32 corner_radius = text_style->font_size / 4.0f;

	rect_style->border_thickness = 0.5f;
	rect_style->corner_radius = V4(corner_radius,
								   corner_radius,
								   corner_radius,
								   corner_radius);
	rect_style->edge_softness = 1.0f;

	UI_CreateRootParent();

	UI_PushParent(ui_state->root);

	UI_NextChildLayoutAxis(Axis2_Y);
	UI_NextSize2(UI_Pct(1), UI_Pct(1));
	UI_Box *root2 = UI_BoxMake(0, Str8Lit("Root2"));

	UI_Box *popup_root = UI_BoxMake(UI_BoxFlag_FixedX |
									UI_BoxFlag_FixedY, Str8Lit("PopupRoot"));

	ui_state->popup_root = popup_root;

	UI_PushParent(root2);
}

internal void UI_SolveIndependentSizes(UI_Box *root)
{
	root->target_size[Axis2_X] = 0;
	root->target_size[Axis2_Y] = 0;

	Vec2F32 text_dim = R_GetTextDim(ui_state->font, root->display_string);

	// NOTE(hampus): Calculate its size
	switch (root->semantic_size[Axis2_X].kind)
	{
		case UI_SizeKind_Null:
		{
			// NOTE(hampus): We set the text size as the default value
			root->target_size[Axis2_X] = (text_dim.x + root->text_style.text_edge_padding[Axis2_X]);
		} break;

		case UI_SizeKind_Pixels:
		{
			root->target_size[Axis2_X] = root->semantic_size[Axis2_X].value;
		} break;

		case UI_SizeKind_TextContent:
		{
			root->target_size[Axis2_X] = (text_dim.x + root->text_style.text_edge_padding[Axis2_X]);
		} break;

		case UI_SizeKind_Pct:
		{
		} break;

		case UI_SizeKind_SumOfChildren:
		{
		} break;

		case UI_SizeKind_Fill:
		{
		} break;

		InvalidCase;
	}

	switch (root->semantic_size[Axis2_Y].kind)
	{
		case UI_SizeKind_Null:
		{
			// NOTE(hampus): We set the text size as the default value
			root->target_size[Axis2_Y] = (text_dim.y + root->text_style.text_edge_padding[Axis2_Y]);
		} break;

		case UI_SizeKind_Pixels:
		{
			root->target_size[Axis2_Y] = root->semantic_size[Axis2_Y].value;
		} break;

		case UI_SizeKind_TextContent:
		{
			root->target_size[Axis2_Y] = (text_dim.y + root->text_style.text_edge_padding[Axis2_Y]);
		} break;

		case UI_SizeKind_Pct:
		{
		} break;

		case UI_SizeKind_SumOfChildren:
		{
		} break;

		case UI_SizeKind_Fill:
		{
		} break;

		InvalidCase;
	}

	for (UI_Box *child = root->first;
		 child != 0;
		 child = child->next)
	{
		UI_SolveIndependentSizes(child);
	}
}

internal void UI_SolveFill(UI_Box *root)
{
	for (S32 axis = 0; axis < Axis2_COUNT; ++axis)
	{
		if (root->semantic_size[axis].kind == UI_SizeKind_Fill)
		{
			if (axis == root->parent->child_layout_axis)
			{
				F32 siblings_size = 0;
				F32 num_siblings_with_fill = 0;

				for (UI_Box *sibling = root->parent->first; sibling != 0; sibling = sibling->next)
				{
					if (sibling->semantic_size[axis].kind == UI_SizeKind_Fill)
					{
						num_siblings_with_fill++;
					}
					else
					{
						if (!(sibling->flags & (UI_BoxFlag_FixedX << axis)))
							siblings_size += sibling->target_size[axis];
					}
				}
				Assert(num_siblings_with_fill != 0);
				root->target_size[axis] = (root->parent->target_size[axis] - siblings_size) / num_siblings_with_fill;
			}
			else
			{
				root->target_size[axis] = root->parent->target_size[axis];
			}
		}
	}

	for (UI_Box *child = root->first;
		 child != 0;
		 child = child->next)
	{
		UI_SolveFill(child);
	}
}

internal void UI_SolveUpwardsDependentSizes(UI_Box *root)
{
	for (U32 axis = 0; axis < Axis2_COUNT; ++axis)
	{
		if (root->semantic_size[axis].kind == UI_SizeKind_Pct)
		{
			Assert(root->parent);
			F32 parent_size = 0;
			if (root->parent->semantic_size[axis].kind == UI_SizeKind_Fill)
			{
				// TODO(hampus): If some parent have already had their fill fixed,
				// we don't need to redo them later.
				UI_SolveFill(root->parent);
			}
			else
			{
				Assert(root->parent->semantic_size[axis].kind == UI_SizeKind_Pixels ||
					   root->parent->semantic_size[axis].kind == UI_SizeKind_TextContent ||
					   root->parent->semantic_size[axis].kind == UI_SizeKind_Pct);
				parent_size = root->parent->target_size[axis];
			}

			root->target_size[axis] = root->parent->target_size[axis] * root->semantic_size[axis].value;
		}
	}

	for (UI_Box *child = root->first;
		 child != 0;
		 child = child->next)
	{
		UI_SolveUpwardsDependentSizes(child);
	}
}

internal Vec2F32 UI_SolveDownwardDependentSizes(UI_Box *root)
{
	Vec2F32 result = {0};
	if (root->semantic_size[Axis2_X].kind == UI_SizeKind_SumOfChildren ||
		root->semantic_size[Axis2_Y].kind == UI_SizeKind_SumOfChildren)
	{
		for (UI_Box *child = root->first;
			 child != 0;
			 child = child->next)
		{
			Vec2F32 size = UI_SolveDownwardDependentSizes(child);
			F32 child_computed_size[2];
			child_computed_size[Axis2_X] = size.x;
			child_computed_size[Axis2_Y] = size.y;

			Axis2 child_layout_axis = root->child_layout_axis;
			Axis2 flipped_layout_axis = Flip(root->child_layout_axis);

			if (!(child->flags & (UI_BoxFlag_FixedX << child_layout_axis)))
			{
				if (root->semantic_size[child_layout_axis].kind == UI_SizeKind_SumOfChildren)
				{
					root->target_size[child_layout_axis] += (child_computed_size[child_layout_axis]);
				}
			}

			if (!(child->flags & (UI_BoxFlag_FixedX << flipped_layout_axis)))
			{
				if (root->semantic_size[flipped_layout_axis].kind == UI_SizeKind_SumOfChildren)
				{
					if ((child_computed_size[flipped_layout_axis]) > root->target_size[flipped_layout_axis])
					{
						root->target_size[flipped_layout_axis] = child_computed_size[flipped_layout_axis];
					}
				}
			}
		}
	}
	else
	{
		for (UI_Box *child = root->first;
			 child != 0;
			 child = child->next)
		{
			UI_SolveDownwardDependentSizes(child);
		}
	}

	result.x = root->target_size[Axis2_X];
	result.y = root->target_size[Axis2_Y];

	return result;
}

internal void UI_CalculateFinalRect(UI_Box *root)
{
	if (root->parent)
	{
		Axis2 axis = root->parent->child_layout_axis;
		if (!(root->flags & (UI_BoxFlag_FixedX << axis)))
		{
			if (root->prev)
			{
				// NOTE(hampus): Find the previous box without a fixed pos.
				UI_Box *prev = 0;
				for (prev = root->prev;
					 prev != 0;
					 prev = prev->prev)
				{
					if (!(prev->flags & (UI_BoxFlag_FixedX << axis)))
					{
						break;
					}
				}

				if (!prev)
				{
					root->calc_rel_pos[axis] = 0;
				}
				else
				{
					// TODO(hampus): Have a flag for deciding this?
					if (UI_BoxHasFlag(root->prev, UI_BoxFlag_AnimateScale))
					{
						root->calc_rel_pos[axis] = prev->calc_rel_pos[axis] + prev->target_size[axis];
					}
					else
					{
						root->calc_rel_pos[axis] = prev->calc_rel_pos[axis] + prev->calc_size[axis];
					}
				}
			}
			else
			{
				root->calc_rel_pos[axis] = 0;
			}
		}

		if (UI_BoxHasFlag(root, UI_BoxFlag_CenterPos))
		{
			F32 parent_center_x = root->parent->calc_pos[Axis2_X] + root->parent->calc_size[Axis2_X] / 2;
			F32 parent_center_y = root->parent->calc_pos[Axis2_Y] + root->parent->calc_size[Axis2_Y] / 2;

			root->target_pos[Axis2_X] = parent_center_x - root->target_size[Axis2_X] / 2;
			root->target_pos[Axis2_Y] = parent_center_y - root->target_size[Axis2_Y] / 2;
		}
		else
		{
			switch (root->parent->child_layout_corner)
			{
				case UI_Corner_TopLeft:
				{
					root->target_pos[Axis2_X] = (root->parent->calc_pos[Axis2_X] + root->calc_rel_pos[Axis2_X]);
					root->target_pos[Axis2_Y] = (root->parent->calc_pos[Axis2_Y] + root->calc_rel_pos[Axis2_Y]);
				} break;

				case UI_Corner_TopRight:
				{
					root->target_pos[Axis2_X] = (root->parent->calc_rect.max.x - root->calc_rel_pos[Axis2_X] - root->target_size[Axis2_X]);
					root->target_pos[Axis2_Y] = (root->parent->calc_pos[Axis2_Y] + root->calc_rel_pos[Axis2_Y]);
				} break;

				case UI_Corner_BottomLeft:
				{
					root->target_pos[Axis2_X] = (root->parent->calc_pos[Axis2_X] + root->calc_rel_pos[Axis2_X]);
					root->target_pos[Axis2_Y] = (root->parent->calc_rect.max.y - root->calc_rel_pos[Axis2_Y] - root->target_size[Axis2_Y]);
				} break;

				case UI_Corner_BottomRight:
				{
					root->target_pos[Axis2_X] = (root->parent->calc_rect.max.x - root->calc_rel_pos[Axis2_X] - root->target_size[Axis2_X]);
					root->target_pos[Axis2_Y] = (root->parent->calc_rect.max.y - root->calc_rel_pos[Axis2_Y] - root->target_size[Axis2_Y]);
				} break;

				InvalidCase;
			}
		}
	}
	else
	{
		root->target_pos[Axis2_X] = root->calc_rel_pos[Axis2_X];
		root->target_pos[Axis2_Y] = root->calc_rel_pos[Axis2_Y];
	}

	root->target_size[Axis2_X] = root->target_size[Axis2_X] * root->target_scale;
	root->target_size[Axis2_Y] = root->target_size[Axis2_Y] * root->target_scale;

	F32 animation_delta = (F32)ui_state->dt * ui_state->animation_speed;

	if (!UI_BoxHasFlag(root, UI_BoxFlag_AnimateStart))
	{
		if (root->last_frame_touched_index == root->frame_created_index)
		{
			root->calc_pos[Axis2_X] = root->target_pos[Axis2_X];
			root->calc_pos[Axis2_Y] = root->target_pos[Axis2_Y];

			root->calc_size[Axis2_X] = root->target_size[Axis2_X];
			root->calc_size[Axis2_Y] = root->target_size[Axis2_Y];

			root->calc_scale = root->target_scale;
		}
	}

	for (U32 axis = 0; axis < Axis2_COUNT; ++axis)
	{
		if (UI_BoxHasFlag(root, (UI_BoxFlag_AnimateX << axis)))
		{
			root->calc_pos[axis] = root->calc_pos[axis] + (root->target_pos[axis] - root->calc_pos[axis]) * animation_delta;
		}
		else
		{
			root->calc_pos[axis] = root->target_pos[axis];
		}
		if (Abs(root->calc_pos[axis] - root->target_pos[axis]) <= 1)
		{
			root->calc_pos[axis] = root->target_pos[axis];
		}
	}

	for (U32 axis = 0; axis < Axis2_COUNT; ++axis)
	{
		if (UI_BoxHasFlag(root, (UI_BoxFlag_AnimateWidth << axis)))
		{
			root->calc_size[axis] = root->calc_size[axis] + (root->target_size[axis] - root->calc_size[axis]) * animation_delta;
		}
		else
		{
			root->calc_size[axis] = root->target_size[axis];
		}
	}

	if (UI_BoxHasFlag(root, UI_BoxFlag_AnimateScale))
	{
		root->calc_scale = root->calc_scale + (root->target_scale - root->calc_scale) * animation_delta;
	}
	else
	{
		root->calc_scale = root->target_scale;
	}

	if (Abs(root->calc_scale - root->target_scale) <= 0.000001f)
	{
		root->calc_scale = root->target_scale;
	}

	root->calc_size[Axis2_X] *= root->calc_scale;
	root->calc_size[Axis2_Y] *= root->calc_scale;

	root->calc_rect.min.x = root->calc_pos[Axis2_X];
	root->calc_rect.min.y = root->calc_pos[Axis2_Y];

	if (root->calc_scale != root->target_scale)
	{
		root->calc_rect.min.x += (root->target_size[Axis2_X] - root->calc_size[Axis2_X]) / 2;
		root->calc_rect.min.y += (root->target_size[Axis2_Y] - root->calc_size[Axis2_Y]) / 2;
	}

	root->calc_rect.max = V2AddV2(root->calc_rect.min, V2(root->calc_size[Axis2_X], root->calc_size[Axis2_Y]));

	for (UI_Box *child = root->first;
		 child != 0;
		 child = child->next)
	{
		UI_CalculateFinalRect(child);
	}
}

internal Vec2F32 UI_AlignDimInRect(Vec2F32 dim, RectF32 calc_rect, UI_TextAlign align, F32 padding[2])
{
	Vec2F32 result = {0};

	F32 rect_width = calc_rect.x1 - calc_rect.x0;
	F32 rect_height = calc_rect.y1 - calc_rect.y0;

	switch (align)
	{
		case UI_TextAlign_Center:
		{
			result.x = calc_rect.x0 + (rect_width - dim.x) / 2;
			result.y = calc_rect.y0 + (rect_height - dim.y) / 2;
		} break;

		case UI_TextAlign_Left:
		{
			result.x = calc_rect.x0 + padding[Axis2_X];
			result.y = calc_rect.y0 + rect_height / 2 - dim.y / 2;
		} break;

		case UI_TextAlign_Right:
		{
			result.x = calc_rect.x1 - dim.x - padding[Axis2_X];
			result.y = calc_rect.y0 + rect_height / 2 - dim.y / 2;
		} break;
	}

	return(result);
}

internal void UI_Animate(UI_Box *root, F32 dt)
{
	if (UI_KeyMatch(ui_state->hot_key, root->key))
	{
		root->hot_t += 0.1f;
	}
	else
	{
		root->hot_t -= 0.1f;
	}

	if (UI_KeyMatch(ui_state->active_key, root->key))
	{
		root->active_t += 0.1f;
	}
	else
	{
		root->active_t -= 0.1f;
	}

	root->hot_t = Clamp(0.0f, root->hot_t, 1.0f);
	root->active_t = Clamp(0.0f, root->hot_t, 1.0f);

	for (UI_Box *child = root->first;
		 child != 0;
		 child = child->next)
	{
		UI_Animate(child, dt);
	}
}

internal void
UI_Draw(UI_Box *root)
{
	R_PushClipRect(root->clip_rect);

	UI_RectStyle *rect_style = &root->rect_style;
	UI_TextStyle *text_style = &root->text_style;

	Vec4F32 corner_radius = rect_style->corner_radius;
	corner_radius = V4MulF32(corner_radius, (F32)ui_state->font->height / 30.0f);

	if (root->flags & UI_BoxFlag_DrawDropShadow)
	{

		R_PushRect(V2SubV2(root->calc_rect.min, V2(10, 10)),
				   V2AddV2(root->calc_rect.max, V2(15, 15)),
				   .color = V4(0, 0, 0, 0.5f),
				   .edge_softness = 10.0f,
				   .corner_radius = corner_radius);

	}

	if (root->flags & UI_BoxFlag_DrawBackground)
	{
		Vec4F32 color = rect_style->background_color;

		if ((root->flags & UI_BoxFlag_ActiveAnimation) &&
			UI_IsActive(root))
		{
			color = rect_style->active_color;
		}
		else if (root->flags & UI_BoxFlag_HotAnimation &&
				 UI_IsHot(root))
		{
			color = rect_style->hot_color;
		}

		R_PushRect(root->calc_rect.min, root->calc_rect.max,
				   .color = color,
				   .corner_radius = corner_radius,
				   .edge_softness = rect_style->edge_softness);
	}

	if (root->flags & UI_BoxFlag_DrawBorder)
	{
		if (UI_IsFocused(root) && !UI_KeyIsNull(ui_state->focus_key) && UI_BoxHasFlag(root, UI_BoxFlag_FocusAnimation))
		{
			// TODO(hampus): Fix this. 
			F32 t = 0;
			R_PushRect(root->calc_rect.min, root->calc_rect.max,
					   .corner_radius = corner_radius,
					   .border_thickness = rect_style->border_thickness,
					   .color = V4(0.8f + 0.2f * t, 0.8f + 0.2f * t, 0.0f, 1.0f),
					   .edge_softness = 1.0f);
		}
		else
		{
			R_PushRect(root->calc_rect.min, root->calc_rect.max,
					   .corner_radius = corner_radius,
					   .border_thickness = rect_style->border_thickness,
					   .color = rect_style->border_color,
					   .edge_softness = 1.0f);
		}
	}

	if (root->flags & UI_BoxFlag_DrawText)
	{
		if (text_style->icon)
		{
			R_Glyph *glyph = &ui_state->font->glyphs[text_style->icon];
			Vec2F32 glyph_dim = V2((F32)glyph->advance, (F32)ui_state->font->max_height);

			F32 padding[Axis2_COUNT] =
			{
				text_style->text_edge_padding[Axis2_X] * root->text_style.font_size,
				text_style->text_edge_padding[Axis2_Y] * root->text_style.font_size,
			};

			R_PushGlyphIndex(UI_AlignDimInRect(glyph_dim, root->calc_rect, text_style->text_align, padding),
							 text_style->font_size, ui_state->font, text_style->icon, V4(1.0f, 1.0f, 1.0f, 1.0f));
		}
		else
		{
			Vec2F32 text_dim = R_GetTextDim(ui_state->font, root->display_string);

			F32 padding[Axis2_COUNT] =
			{
				text_style->text_edge_padding[Axis2_X] * root->text_style.font_size,
				text_style->text_edge_padding[Axis2_Y] * root->text_style.font_size,
			};

			R_PushText(UI_AlignDimInRect(text_dim, root->calc_rect, text_style->text_align, padding),
					   text_style->font_size,
					   ui_state->font,
					   root->display_string,
					   text_style->text_color);
		}
	}

	if (ui_state->show_debug_lines)
	{
		R_PushRect(root->calc_rect.min, root->calc_rect.max,
				   .border_thickness = 1.0f,
				   .color = V4(1.0f, 0.0f, 1.0f, 1.0f));
	}

	R_PopClipRect();

	for (UI_Box *child = root->first;
		 child != 0;
		 child = child->next)
	{
		UI_Draw(child);
	}
}

internal void UI_Layout(UI_Box *root)
{
	UI_SolveIndependentSizes(root);
	UI_SolveUpwardsDependentSizes(root);
	UI_SolveDownwardDependentSizes(root);
	UI_SolveFill(root);
	UI_CalculateFinalRect(root);
}

internal void
UI_End()
{
	UI_PopParent();

	UI_PopParent();

	UI_Box *root = ui_state->root;

	UI_Layout(root);
	UI_Animate(root, (F32)ui_state->dt);
	UI_Draw(root);

	ui_state->old_mouse_pos = ui_state->mouse_pos;

	ui_state->prev_frame_arena_used = UI_FrameArena()->pos;
	ArenaZero(UI_FrameArena());

	ui_state->frame++;
}

internal UI_State *
UI_Init(MemoryArena *arena, R_Font *font, OS_Window *window)
{
	UI_State *result = PushStructNoZero(arena, UI_State);

	size_t ui_permanent_storage_size = GIGABYTES(1);
	size_t ui_frame_storage_size = MEGABYTES(8);
	size_t ui_memory_size = ui_permanent_storage_size + ui_frame_storage_size;
	// TODO(hampus): Remove calloc()
	void *ui_memory = PushArrayNoZero(arena, ui_memory_size, U8);

	result->font = font;
	result->window = window;

	ArenaInit(&result->permanent_arena, ui_memory, ui_permanent_storage_size);
	ArenaInit(&result->frame_arena, ((U8 *)ui_memory) + ui_permanent_storage_size, ui_frame_storage_size);

	result->box_storage_count = 4096;
	result->box_storage = PushArrayNoZero(&result->permanent_arena, result->box_storage_count, UI_Box);

	for (U64 i = 0; i < result->box_storage_count; ++i)
	{
		UI_FreeBox *free_box = (UI_FreeBox *)(result->box_storage + i);
		free_box->next = result->first_free_box;
		result->first_free_box = free_box;
	}

	result->box_free_list_slots_used = 0;
	result->box_free_list_size = 4096;
	result->box_hash_map_count = 4096;
	result->box_hash_map = PushArrayNoZero(&result->permanent_arena, result->box_hash_map_count, UI_Box *);

	result->animation_speed = 20.0f;

	return(result);
}

internal UI_Box *ui_get_box(String8 string)
{
	UI_Key key = UI_KeyFromString(string);

	UI_Box *result = UI_BoxFromKey(key);

	return(result);
}

internal void
UI_SelectState(UI_State *state)
{
	ui_state = state;
}