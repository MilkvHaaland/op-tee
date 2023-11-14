#ifndef _STDIO_IMPL_EXT_H
#define _STDIO_IMPL_EXT_H

int sscanf(const char *restrict s, const char *restrict fmt, ...);

double strtod(const char *nptr, char **endptr);

long strtol(const char *restrict s, char **restrict p, int base);

#endif /* _STDIO_IMPL_EXT_H */
