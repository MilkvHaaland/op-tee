#include <stdio.h>
#include <stdarg.h>
#include <type_impl.h>

extern int vsscanf(const char *restrict s, const char *restrict fmt, va_list ap);

int sscanf(const char *restrict s, const char *restrict fmt, ...)
{
	int ret;
	va_list ap;
	va_start(ap, fmt);
	ret = vsscanf(s, fmt, ap);
	va_end(ap);
	return ret;
}

weak_alias(sscanf,__isoc99_sscanf);
