#include <string.h>
#include <stddef.h>

// From http://creativeandcritical.net/str-replace-c/
char *replace_str(const char *str, const char *oldS, const char *newS)
{
	char *ret, *r;
	const char *p, *q;
	size_t oldSlen = strlen(oldS);
	size_t count, retlen, newSlen = strlen(newS);

	if (oldSlen != newSlen) {
		for (count = 0, p = str; (q = strstr(p, oldS)) != NULL; p = q + oldSlen)
			count++;
		/* this is undefined if p - str > PTRDIFF_MAX */
		retlen = p - str + strlen(p) + count * (newSlen - oldSlen);
	} else
		retlen = strlen(str);

	if ((ret = (char*)malloc(retlen + 1)) == NULL)
		return NULL;

	for (r = ret, p = str; (q = strstr(p, oldS)) != NULL; p = q + oldSlen) {
		/* this is undefined if q - p > PTRDIFF_MAX */
		ptrdiff_t l = q - p;
		memcpy(r, p, l);
		r += l;
		memcpy(r, newS, newSlen);
		r += newSlen;
	}
	strcpy(r, p);

	return ret;
}
