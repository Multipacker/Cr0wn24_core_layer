#define ReleaseScratch(scratch) (EndTempArena(&scratch))

global MemoryArena scratch_arena_[2];

function void CopySize_(U8 *dst, U8 *src, size_t size)
{
	for(size_t i = 0; i < size;++i)
	{
		*dst++ = *src++;
	}
}

function void ZeroSize_(U8 *base, size_t size)
{
	for(size_t i = 0; i < size;++i)
	{
		*base++ = 0;
	}
}

function void
ArenaInit(MemoryArena *arena, U8 *base, size_t size)
{
	arena->base = base;
	arena->size = size;
	arena->pos = 0;
}

function void ArenaZero(MemoryArena *arena)
{
	arena->pos = 0;
}

function void *PushSize_(MemoryArena *arena, size_t size)
{
	Assert(arena->pos + size <= arena->size);
	void *result = arena->base + arena->pos;
	arena->pos += size;
	return result;
}

function void *PushSizeZero_(MemoryArena *arena, size_t size)
{
	void *result = PushSize_(arena, size);
	ZeroSize(result, size);
	return result;
}

function void *
PushSizeCopy_(MemoryArena *arena, U8 *src, size_t size)
{
	void *result = PushSize_(arena, size);
	CopySize_(result, src, size);

	return result;
}

function void ArenaPopTo(MemoryArena *arena, U64 pos)
{
	arena->pos = pos;
}

function TempMemoryArena BeginTempArena(MemoryArena *arena)
{
	TempMemoryArena temp;
	temp.arena = arena;
	temp.pos = arena->pos;
	return(temp);
}

function void EndTempArena(TempMemoryArena *temp)
{
	ArenaPopTo(temp->arena, temp->pos);
}

function TempMemoryArena
GetScratch(MemoryArena **conflicts, U64 conflict_count)
{
	TempMemoryArena scratch = {0};
	for(U64 i = 0; i < ArrayCount(scratch_arena_); ++i)
	{
		B32 is_conflicting = 0;
		for(MemoryArena **conflict = conflicts; conflict < conflicts + conflict_count; conflict++)
		{
			if(*conflict == (scratch_arena_ + i))
			{
				is_conflicting = 1;
				break;
			}
		}

		if(is_conflicting == 0)
		{
			scratch.arena = &scratch_arena_[i];
			scratch.pos = scratch_arena_[i].pos;
			break;
		}
	}
	return(scratch);
}