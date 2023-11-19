internal U64 CStringLength(char * str)
{
	U64 result = 0;

	while(*(str + result))
	{
		++result;
	}

	return result;
}

internal B32 CStringsAreEqual(char *string0, char *string1)
{
	B32 result = true;

	U64 length0 = CStringLength(string0);
	U64 length1 = CStringLength(string1);

	if(length0 == length1)
	{
		for(U64 i = 0; i < length0; ++i)
		{
			if(string0[i] != string1[i])
			{
				result = false;
				break;
			}
		}
	}
	else
	{
		result = false;
	}

	return(result);
}

internal void CStringCopy(char *dest, char *src, U32 src_size, U32 dest_size)
{
	Assert(src_size >= dest_size);
	for(U32 i = 0; i < dest_size; ++i)
	{
		*dest++ = *src++;
	}
}

internal String8 Str8(U8 *str, U64 size)
{
	String8 string;
	string.str = str;
	string.size = size;
	return(string);
}

internal String8 PushStr8FV(MemoryArena *arena, char *fmt, va_list args)
{
	String8 result = {0};
	U64 needed_bytes = vsnprintf(0, 0, fmt, args) + 1;
	result.str = PushArray(arena, needed_bytes, U8);
	result.size = needed_bytes - 1;
	vsnprintf((char *)result.str, needed_bytes, fmt, args);
	return(result);
}

internal String8 PushStr8F(MemoryArena *arena, char *fmt, ...)
{
	String8 result = {0};
	va_list args;
	va_start(args, fmt);
	result = PushStr8FV(arena, fmt, args);
	va_end(args);
	return(result);
}

internal String8 PushStr8Size(MemoryArena *arena, char *src, U64 src_size)
{
	String8 result = {0};
	result.size = src_size;
	result.str = PushArray(arena, src_size + 1, U8);
	CopyArray(result.str, src, result.size);
	result.str[result.size] = 0;
	return(result);
}

internal B32 Str8Match(String8 a, String8 b)
{
	B32 result = true;

	if(a.size == b.size)
	{
		for(U64 i = 0; i < a.size; ++i)
		{
			if(a.str[i] != b.str[i])
			{
				result = false;
				break;
			}
		}
	}
	else
	{
		result = false;
	}

	return result;
}

internal String8 Str8Append(MemoryArena *arena, String8 a, String8 b)
{
	String8 result = {0};
	result.size = a.size + b.size;
	result.str = PushArrayNoZero(arena, result.size + 1, U8);

	CopyArray(result.str, a.str, a.size);
	CopyArray(result.str + a.size, b.str, b.size);
	result.str[result.size] = 0;

	return result;
}

internal S32 Str8FindSubStr8(String8 haystack, String8 needle)
{
	if(haystack.size <= needle.size)
	{
		return (-1);
	}

	S32 pos = -1;
	for(U64 i = 0; i < haystack.size; ++i)
	{
		if((haystack.size - i) < needle.size)
		{
			break;
		}

		if(haystack.str[i] == needle.str[0])
		{
			B32 found = true;
			for(U64 j = 1; j < needle.size; ++j)
			{
				if(haystack.str[i + j] != needle.str[j])
				{
					found = false;
				}
			}

			if(found)
			{
				pos = (S32)i;
				break;
			}
		}
	}

	return pos;
}

internal String8List Str8Split(MemoryArena *arena, String8 string, char separator)
{
	String8List result = {0};

	char *start = (char*)string.str;
	char *current = (char*)string.str;

	while(true)
	{
		char ch = *current;
		if(ch == separator ||
			 ch == '\0')
		{
			String8Node *node = PushStruct(arena, String8Node);
			node->string = PushStr8Size(arena, start, current - start);
			DLL_PushBack(result.first, result.last, node);

			++current;
			start = current;
		}

		if(ch == '\0')
		{
			break;
		}

		++current;
	}

	return(result);
}