#include <stdint.h>

typedef uint8_t  u8;
typedef uint16_t u16;
typedef uint32_t u32;
typedef uint64_t u64;

typedef int8_t  s8;
typedef int16_t s16;
typedef int32_t s32;
typedef int64_t s64;

typedef s8  b8;
typedef s32 b32;

#define global   static
#define function static

// TODO: Add #if DEBUG for Assert
#define Assert(x) if(!(x)) {*(u8*) 0 = 0;}
#define ArrayLen(x) (sizeof((x))/sizeof((x)[0]))
#define Pow2(x) (1 << (x))
