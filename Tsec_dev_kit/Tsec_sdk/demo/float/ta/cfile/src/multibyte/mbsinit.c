#include <wchar_impl.h>

int mbsinit(const mbstate_t *st)
{
	return !st || !*(unsigned *)st;
}
