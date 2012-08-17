/*
Commonly used header files, macros, and structures
*/

#include "GLES2/gl2.h"
#include <stdlib.h>
#include <assert.h>

// NEW takes a pointer and allocates zeroed memory for its contents of the appropriate size
#define NEW(p) do{ (p) = calloc(1,sizeof(*(p))); assert(p); } while(0)
#define CLEAR(p) memset(p,0,sizeof(p))
#define min(a,b) ((a)<(b)?(a):(b))
#define max(a,b) ((a)>(b)?(a):(b))
#define check() assert(glGetError() == 0)

