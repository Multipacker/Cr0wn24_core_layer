#include "win32/os_win32_inc.c"

internal void
OS_EatEvent(OS_EventList *event_list, OS_EventNode *node)
{
	if(node == event_list->first)
	{
		event_list->first = event_list->first->next;
	}
	else
	{
		if(node->prev)
		{
			node->prev->next = node->next;
		}

		if(node->next)
		{
			node->next->prev = node->prev;
		}
	}
}

internal B32
OS_TimeGreaterThanTime(Time *a, Time *b)
{
	B32 result = false;

	if(a->year > b->year)
	{
		result = true;
	}
	else if(a->month > b->month)
	{
		result = true;
	}
	else if(a->day > b->day)
	{
		result = true;
	}
	else if(a->hour > b->hour)
	{
		result = true;
	}
	else if(a->minute > b->minute)
	{
		result = true;
	}
	else if(a->second > b->second)
	{
		result = true;
	}
	else if(a->millisecond > b->millisecond)
	{
		result = true;
	}

	return(result);
}

// TODO(hampus): Move this onto the base layer!
internal B32 SameTime(Time *a, Time *b)
{
	return(a->year == b->year &&
				 a->month == b->month &&
				 a->day == b->day &&
				 a->hour == b->hour &&
				 a->minute == b->minute &&
				 a->second == b->second);
}
