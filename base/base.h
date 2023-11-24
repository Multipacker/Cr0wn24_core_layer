#ifndef BASE_H
#define BASE_H

#include <stdint.h>
#include <stdarg.h>
#include <stdlib.h>
#include <stdio.h>
#include <conio.h>
#include <stdbool.h>
#include <time.h>

#define internal 			static
#define local_persist static
#define global 				static

typedef uint8_t 	U8;
typedef uint16_t 	U16;
typedef uint32_t 	U32;
typedef uint64_t 	U64;

typedef int8_t 		S8;
typedef int16_t 	S16;
typedef int32_t 	S32;
typedef int64_t 	S64;

typedef float 	F32;
typedef double 	F64;

typedef S32 B32;

#define FLT_MAX 3.40282e+038f
#define FLT_MIN 1.17549e-038f

#if DEBUG
#define Assert(expr) if(!(expr)) { *(volatile S32 *)0 = 0; }
#else
#define Assert(expr)
#endif

#define InvalidCase 			default: {Assert(false);} break;
#define InvalidCodePath 	Assert(false)

#define Flip(x) 						(!(x))
#define CompSizeOf(type) 		switch(true) {case sizeof(type): case sizeof(type): {} break; }
#define ArrayCount(array) 	(sizeof(array) / sizeof(array[0]))
#define Swap(a, b, type) 		{ type temp = a; a = b; b = temp; }

#define Glue(a, b) a##b

#define MemberOffset(type, member) ((size_t)&(((type*)0)->member)) 

#define IntFromPtr(p) (U64)((U8 *)p - (U8 *)0)
#define PtrFromInt(n) (void*) ((U8 *)0 + n)

#define KILOBYTES(n) (n*1024LL)
#define MEGABYTES(n) (1024LL*KILOBYTES(n))
#define GIGABYTES(n) (1024LL*MEGABYTES(n))
#define TERABYTES(n) (1024LL*GIGABYTES(n))

#define CheckNull(x) (x == 0)
#define SetNull(x) (x = 0)

#define DLL_PushBack_NP(f,l,n,next,prev) 	((f)==0?\
((f)=(l)=(n),(n)->next=(n)->prev=0):\
((n)->prev=(l),(l)->next=(n),(l)=(n),(n)->next=0))

#define DLL_PushBack(f,l,n) DLL_PushBack_NP(f,l,n,next,prev)

#define DLL_PushFront(f,l,n) DLL_PushBack_NP(l,f,n,prev,next)

#define DLL_Remove_NPZ(f,l,n,next,prev,zchk,zset) (((f)==(n))?\
((f)=(f)->next, (zchk(f) ? (zset(l)) : zset((f)->prev))):\
((l)==(n))?\
((l)=(l)->prev, (zchk(l) ? (zset(f)) : zset((l)->next))):\
((zchk((n)->next) ? (0) : ((n)->next->prev=(n)->prev)),\
(zchk((n)->prev) ? (0) : ((n)->prev->next=(n)->next))))

#define DLL_Remove(f,l,n) DLL_Remove_NPZ(f,l,n,next,prev,CheckNull,SetNull)

#define SLL_PushFront_NP(f, n, next) ((n)->next = (f), \
									  (f) = (n));

#define SLL_PushFront(f, n) SLL_PushFront_NP(f, n, next)

#define StackPush_N(f,n,next) ((n)->next=(f),(f)=(n))

#define StackPush(f,n) StackPush_N(f,n,next)

#define StackPop_N(f,next) ((f)==0?0:\
((f)=(f)->next))

#define StackPop(f) StackPop_N(f, next)

#define QueuePush_N(f,l,n,next) ((f)==0?\
(f)=(l)=(n):\
((l)->next=(n),(l)=(n)),\
(n)->next=0)

#define QueuePush(f,l,n) QueuePush_N(f,l,n,next)

#define QueuePop_N(f,l,next) 	((f)==(l)?\
(f)=(l)=0:\
((f)=(f)->next))

#define QueuePop(f,l) QueuePop_N(f,l,next)

typedef enum Month
{
	Month_None,

	Month_January,
	Month_February,
	Month_March,
	Month_April,
	Month_May,
	Month_June,
	Month_July,
	Month_August,
	Month_September,
	Month_October,
	Month_November,
	Month_December,
} Month;

typedef struct Time
{
	U32 year;
	Month month;
	U32 day;
	U32 hour;
	U32 minute;
	U32 second;
	U32 millisecond;
} Time;

#endif
