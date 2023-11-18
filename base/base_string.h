#ifndef BASE_STRING_H
#define BASE_STRING_H

typedef struct String8
{
	U8 *str;
	U64 size;
} String8;

typedef struct String8Node
{
	struct String8Node *next;
	struct String8Node *prev;

	String8 string;
} String8Node;

typedef struct String8List
{
	String8Node *first;
	String8Node *last;
} String8List;

typedef struct String8StackNode
{
	struct String8StackNode *next;

	String8 string;
} String8StackNode;

typedef struct String8Stack
{
	String8StackNode *first;
} String8Stack;

#endif