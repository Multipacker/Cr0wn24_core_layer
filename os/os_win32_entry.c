function S32 EntryPoint(String8List args);

function void
ParseCommandLineArguments(String8List *string_list, MemoryArena *arena, char *command_line)
{
	char *arg = command_line;
	while(*arg)
	{
		U64 i = 0;
		for(i = 0;
				*arg != ' ' && *arg != 0;
				++i, ++arg)
		{
		}

		String8Node *node = PushStruct(arena, String8Node);
		node->string = PushStr8Size(arena, arg - i, i);

		DLL_PushBack(string_list->first, string_list->last, node);

		if(*arg == ' ')
		{
			++arg;
		}
	}
}

#if 1

S32
main(S32 argc, char *argv[])
{
	MemoryArena args_arena = {0};
	ArenaInit(&args_arena, malloc(1024), 1024);

	String8List string_list = {0};
	for(S32 i = 0; i < argc; ++i)
	{
		ParseCommandLineArguments(&string_list, &args_arena, argv[i]);
	}

	S32 result = EntryPoint(string_list);

	return(result);
}

#else

S32 APIENTRY
WinMain(HINSTANCE instance, HINSTANCE prev_instance, LPSTR command_line, S32 show_code)
{
	MemoryArena args_arena = {0};
	ArenaInit(&args_arena, malloc(1024), 1024);

	String8List string8_list;
	ParseCommandLineArguments(&string8_list, &args_arena, command_line);

	S32 result = EntryPoint(string8_list);

	return(result);
}

#endif
