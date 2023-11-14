#include <stdlib.h>
#include <stdio_impl_ext.h>

double atof(const char *s)
{
	return strtod(s, 0);
}
