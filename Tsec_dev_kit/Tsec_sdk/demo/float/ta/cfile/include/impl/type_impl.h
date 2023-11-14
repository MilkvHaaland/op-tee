#ifndef _TYPE_IMPL_H
#define _TYPE_IMPL_H

#ifndef off_t

#define off_t long long int
#endif /* #ifndef off_t */

#define hidden __attribute__((__visibility__("hidden")))

typedef struct _IO_FILE FILE_IMPL;

typedef __builtin_va_list __isoc_va_list;
typedef struct __mbstate_t { unsigned __opaque1, __opaque2; } mbstate_t;
typedef float float_t;
typedef double double_t;

#endif
