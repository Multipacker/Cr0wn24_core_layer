#define CORE_PATH "../"

#include "core_layer/base/base_inc.h"
#include "core_layer/os/os_win32_inc.h"
#include "core_layer/renderer/renderer.h"
#include "core_layer/renderer/backends/d3d11/d3d11.h"
#include "core_layer/ui/ui_inc.h"

#include "core_layer/base/base_inc.c"
#include "core_layer/os/os_win32_inc.c"
#include "core_layer/renderer/renderer.c"
#include "core_layer/renderer/backends/d3d11/d3d11.c"
#include "core_layer/ui/ui_inc.c"

internal void
UITest()
{
	UI_Spacer(UI_Em(0.5f));
	UI_NextSize2(UI_Em(40.0f), UI_SumOfChildren());
	UI_NextChildLayoutAxis(Axis2_X);
	UI_NextRelativePos2(300, 300);
	UI_NextBackgroundColor(ui_state->theme.window_color);
	UI_Box *box2 = UI_BoxMake(UI_BoxFlag_DrawBackground |
														UI_BoxFlag_DrawBorder |
														UI_BoxFlag_DrawDropShadow |
														UI_BoxFlag_FixedX |
														UI_BoxFlag_FixedY |
														UI_BoxFlag_AnimateWidth |
														UI_BoxFlag_AnimateHeight |
														UI_BoxFlag_Clip,
														Str8Lit("My box2"));
	UI_Size tree_spacing = UI_Em(0.3f);

	local_persist Vec4F32 color_test = {1.0f, 1.0f, 1.0f, 1.0f};

	UI_Parent(box2)
	{
		UI_Spacer(UI_Em(0.5f));
		UI_NextSize2(UI_SumOfChildren(), UI_SumOfChildren());
		UI_Column()
		{
			UI_Spacer(UI_Em(0.5f));
			UI_Text(Str8Lit("Column 1"));
			UI_Spacer(UI_Em(0.2f));

			UI_NextSize2(UI_Em(5.0f), UI_Pixels(1));
			UI_NextBackgroundColor(ui_state->theme.text_color);
			UI_BoxMake(UI_BoxFlag_DrawBackground, Str8Lit(""));
			UI_Spacer(UI_Em(0.5f));

			local_persist F32 test_value = 5.0f;

			UI_SliderF32(&test_value, -10.0f, 10, Str8Lit("Slider"));
			UI_PushSize2(UI_Em(12.0f), UI_Em(1.0f));
			UI_Spacer(UI_Em(0.5f));

			UI_Text(Str8Lit("Hello2##7"));

			UI_Spacer(UI_Em(0.5f));
			UI_ButtonF("Button %d###1", 5);
			UI_Spacer(UI_Em(0.5f));

			UI_PopSize2();

			UI_Divider();
			UI_Spacer(UI_Em(0.2f));
			UI_Text(Str8Lit("Settings"));
			UI_Spacer(UI_Em(0.2f));
			UI_Divider();

			UI_Spacer(UI_Em(0.5f));
			UI_Check(Str8Lit("Show debug lines"), &ui_state->show_debug_lines);
			UI_Spacer(UI_Em(0.5f));

			local_persist B32 check_value2 = false;
			UI_Check(Str8Lit("Test check##11"), &check_value2);
			UI_Spacer(UI_Em(0.5f));

			UI_ColorPicker(&color_test, Str8Lit("Color picker"));

			UI_Spacer(UI_Em(0.5f));
			UI_ColorPicker(&color_test, Str8Lit("Color picker##1"));
			UI_Spacer(UI_Em(0.5f));
			local_persist B32 b0 = false;
			local_persist B32 b1 = false;
			local_persist B32 b2 = false;
			local_persist B32 b3 = false;

			UI_RadioData radio_data[] =
			{
					{&b0, Str8Lit("Option 1")},
					{&b1, Str8Lit("Option 2")},
					{&b2, Str8Lit("Option 3")},
					{&b3, Str8Lit("Option 4")},
			};

			UI_Radio(radio_data, 4, Str8Lit("Radio"));
		}

		UI_Spacer(UI_Em(1.0f));

		UI_Divider();

		UI_Spacer(UI_Em(1.0f));
		UI_NextSize2(UI_SumOfChildren(), UI_SumOfChildren());
		UI_Column()
		{
			UI_Spacer(UI_Em(0.5f));
			UI_Text(Str8Lit("Column 2"));
			UI_Spacer(UI_Em(0.2f));

			UI_NextSize2(UI_Em(5.0f), UI_Pixels(1));
			UI_NextBackgroundColor(ui_state->theme.text_color);
			UI_BoxMake(UI_BoxFlag_DrawBackground, Str8Lit(""));
			UI_Spacer(UI_Em(0.5f));

			UI_Button(Str8Lit("Button"));

			UI_Spacer(UI_Em(0.5f));

			UI_Tree(Str8Lit("Entities"))
			{
				UI_Spacer(tree_spacing);
				UI_Text(Str8Lit("Text inside a tree"));
				UI_Spacer(tree_spacing);
				UI_Text(Str8Lit("Text inside a tree"));
				UI_Spacer(tree_spacing);

				UI_Tree(Str8Lit("Entity 1"))
				{
					UI_Spacer(tree_spacing);
					UI_TextF("Pos: (%.02f, %.02f)", 1.2f, 0.5f);
					UI_Spacer(tree_spacing);
					UI_TextF("Type: %s ", "static");
					UI_Spacer(tree_spacing);
					local_persist F32 health = 0;
					UI_SliderF32(&health, -10.0f, 10, Str8Lit("Health"));
					UI_Spacer(tree_spacing);
				}
				UI_Spacer(tree_spacing);

				UI_Tree(Str8Lit("Entity 2"))
				{
					UI_Spacer(tree_spacing);
					UI_Text(Str8Lit("Text inside a tree2"));
					UI_Spacer(tree_spacing);
					UI_Text(Str8Lit("Text inside a tree2"));
					UI_Spacer(tree_spacing);
					UI_Tree(Str8Lit("My tree4"))
					{
						UI_Spacer(tree_spacing);
						UI_Text(Str8Lit("Text inside a tree2"));
						UI_Spacer(tree_spacing);
						UI_Text(Str8Lit("Text inside a tree2"));
						UI_Spacer(tree_spacing);
						local_persist B32 check_value = false;
						UI_Check(Str8Lit("Test check##10"), &check_value);

						UI_Spacer(tree_spacing);
					}
				}
			}
			UI_Spacer(UI_Em(0.5f));

			UI_NextSize2(UI_Em(10), UI_Em(10));
			UI_PushScrollableContainer(Str8Lit("scrollablecontainer3"));
			UI_NextSize2(UI_SumOfChildren(), UI_SumOfChildren());
			UI_Column()
			{
				for(U32 i = 0; i < 100; ++i)
				{
					UI_Text(Str8Lit("Hello"));
				}
			}

			UI_PopScrollableContainer();
			UI_Spacer(UI_Em(0.5f));

			local_persist char input_buffer[256] = {0};
			UI_NextSize2(UI_Em(10), UI_Em(1.5f));
			if(UI_TextInput(input_buffer, sizeof(input_buffer), Str8Lit("Your username")).enter)
			{
				printf("Enter\n");
			}

			UI_Spacer(UI_Em(0.5f));
			UI_Divider();
			UI_Spacer(UI_Em(0.5f));
			UI_Text(Str8Lit("Default widget sizes test"));
			UI_Spacer(UI_Em(0.5f));
			UI_Divider();
			UI_Spacer(UI_Em(0.5f));
			UI_Button(Str8Lit("Default button size, g"));
			local_persist B32 check_test = false;
			UI_Check(Str8Lit("Default check size"), &check_test);
			local_persist F32 slider_value = 0;
			UI_SliderF32(&slider_value, 0, 10, Str8Lit("Default slider size"));
			local_persist Vec4F32 color = {1, 1, 1, 1};
			UI_ColorPicker(&color, Str8Lit("Default color picker size"));
		}
		UI_Spacer(UI_Em(0.5f));
	}
}

internal S32
EntryPoint(String8List args)
{
	MemoryArena permanent_arena;
	ArenaInit(&permanent_arena, OS_AllocMem(MEGABYTES(128)), MEGABYTES(128));

	CoreInit();

	OS_Window *window = OS_CreateWindow(Str8Lit("Test"), 0, 0, 800, 800, true);

	D3D11_Init(window);

	R_Texture tile_atlas = R_LoadTexture(Str8Lit("../res/test/Tilemap/tilemap_packed.png"));

	TextureAtlas atlas = {0};
	{
		R_LoadedBitmap loaded_bitmaps[16] = {0};
		TempMemoryArena scratch = GetScratch(0, 0);

		for(U32 i = 0; i < 16; ++i)
		{
			S32 channels = 0;
			String8 path = {0};
			if(i < 10)
			{
				path = PushStr8F(scratch.arena, "../res/test/Tiles/tile_000%d.png", i);
			}
			else
			{
				path = PushStr8F(scratch.arena, "../res/test/Tiles/tile_00%d.png", i);
			}

			loaded_bitmaps[i].data = stbi_load((const char *)path.str,
																				 &loaded_bitmaps[i].dim.width, &loaded_bitmaps[i].dim.height, &channels, 0);
		}
		ReleaseScratch(scratch);

		atlas = R_PackBitmapsIntoTextureAtlas(&permanent_arena, 128, 128, loaded_bitmaps, 16, 1);

		for(U32 i = 0; i < 16; ++i)
		{
			stbi_image_free(loaded_bitmaps[i].data);
		}
	}

	R_Font font = {0};
	R_LoadFont(&permanent_arena, &font, CORE_RESOURCE("font/liberation-mono.ttf"), CORE_RESOURCE("font/icon/mfglabsiconset-webfont.ttf"), 20);

	UI_Init(&font, window);

	F64 dt = 0;
	F64 start_counter = OS_SecondsSinceAppStart();
	B32 running = true;

	while(running)
	{
		TempMemoryArena scratch = GetScratch(0, 0);

		OS_EventList *event_list = OS_GatherEventsFromWindow(scratch.arena);

		for(OS_EventNode *node = event_list->first;
				node != 0;
				node = node->next)
		{
			switch(node->event.type)
			{
				case OS_EventType_Quit:
				{
					running = false;
				} break;

				case OS_EventType_KeyPress:
				{
					if(node->event.key == OS_Key_F11)
					{
						OS_ToggleFullscreen(window);
					}
				} break;
			}
		}

		R_Begin(scratch.arena);

		R_PushText(V2(1700, 50), 20, &font, Str8Lit("Hello, world!"), V4(1.0f, 1.0f, 1.0f, 1.0f));
		R_PushRect(V2(1200 - 50, 50), V2(1200 + 450, 50 + 65), .color = V4(0.5, 0, 0, 1), .corner_radius = V4(10, 10, 10, 10), .edge_softness = 1);
		R_PushRect(V2(1200 - 50, 50), V2(1200 + 450, 50 + 65), .color = V4(1, 1, 1, 1), .corner_radius = V4(10, 10, 10, 10), .edge_softness = 1, .border_thickness = 1);

		R_PushRect(V2(0, 0), V2(1024, 1024), .texture = font.atlas.texture, .text = true);

		R_PushRect(V2(1800, 800), V2(2400, 1400), .texture = atlas.texture);

		Vec4F32 corner_radius = V4(30, 50, 20, 10);

		R_PushRect(V2(50, 500), V2(500, 1000), .color = V4(0, 0, 0, 1), .corner_radius = corner_radius, .edge_softness = 1);
		R_PushRect(V2(50, 500), V2(500, 1000), .color = V4(1, 0, 0, 1), .corner_radius = corner_radius, .edge_softness = 1, .border_thickness = 0.5f);

		UI_Begin(UI_DefaultTheme(), 20, event_list, dt);

		UITest();

		UI_End();

		R_End();

		F64 end_counter = OS_SecondsSinceAppStart();
		dt = end_counter - start_counter;
		start_counter = end_counter;

		ReleaseScratch(scratch);
	}
	return(0);
}