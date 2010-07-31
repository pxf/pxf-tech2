#include <Pxf/Base/String.h>
#include <Pxf/Base/Memory.h>
#include <Pxf/Base/Debug.h>
#include <Pxf/Base/Utils.h>

#include <cstdlib>
#include <cstdio>
#include <cstring>
#include <cstdarg> /* strfmt_n*/
#include <cctype>

/*
 * String manipulation functions
 *****************************************************************************/
char *Pxf::StringCopy(char *dest, const char *src, unsigned count)
{
	strncpy(dest, src, count);
	return dest;
}

char *Pxf::StringCat(char *dest, const char *src, unsigned count)
{
	strncat(dest, src, count);
	return dest;
}

char *Pxf::StringPadLeft(char *dest, const char *src, int width, char pad_char)
{
	int len = StringLength(src);
	MemorySet(dest, pad_char, width);
	StringCopy(dest, src, Min(len, width));
	/*dest[width] = 0x0; */
	return dest;
}

char *Pxf::StringPadRight(char *dest, const char *src, int width, char pad_char)
{
	int len = StringLength(src);
	int dst_off = width-len;
	int src_off = len-width;
	src_off = (src_off < 0) ? 0 : src_off;
	dst_off = (dst_off < 0) ? 0 : dst_off;
	MemorySet(dest, pad_char, width);
	MemoryCopy(dest + dst_off, src + src_off, Min(len, width));
	/*dest[width] = 0x0; */
	return dest;
}

char *Pxf::StringPadCenter(char *dest, const char *src, int width, char pad_char)
{
	int len = StringLength(src);
	int dst_off = (width-len) / 2; /* OPT (width-len) << 1 */
	int src_off = (len-width) / 2; /* OPT (len-width) << 1 */
	dst_off = (dst_off < 0) ? 0 : dst_off;
	src_off = (src_off < 0) ? 0 : src_off;
	MemorySet(dest, pad_char, width);
	StringCopy(dest + dst_off, src + src_off, Min(len, width));
	/*dest[width] = 0x0;*/
	return dest;
}

/*
 * String manipulaton functions which returns malloced memory 
 *****************************************************************************/
char *Pxf::StringDuplicate(const char *str)
{
	return (char*)strdup(str);
}

/* Copied from the manual pages for vsprintf */
char*	StringDuplicateF(const char *format, ...)
{
	/* Guess we need no more than 100 bytes. */
	int n, size = 100;
	char *p, *np;
	va_list ap;

	if ((p = (char*)malloc(size*sizeof(char))) == NULL)
	return NULL;

	while (1)
	{
		/* Try to print in the allocated space. */
		va_start(ap, format);
		n = vsnprintf(p, size, format, ap);
		va_end(ap);
		/* If that worked, return the string. */
		if (n > -1 && n < size)
			return p;
		/* Else try again with more space. */
		if (n > -1)    /* glibc 2.1 */
			size = n+1; /* precisely what is needed */
		else           /* glibc 2.0 */
			size *= 2;  /* twice the old size */
		if ((np =(char*)realloc (p, size*sizeof(char))) == NULL) {
			free(p);
			return NULL;
		}
		else
		{
			p = np;
		}
	}
}

char *Pxf::StringSub(const char *str, int start, int length)
{
	int str_length = StringLength(str);
	char *ret;

	if (length == -1) length = str_length - start;
	if (length < 0)   length = str_length - start + length;
	if (start < 0)     start = str_length + start;

	if (start > str_length || length < 0 || length-start > length) return 0;

	ret = (char*)MemoryAllocate((length+1)*sizeof(char));
	StringCopy(ret, str+start, length);
	ret[length] = 0;
	return ret;
}


char *Pxf::StringConcat(const char *str1, const char *str2)
{
	int len_str1 = StringLength(str1);
	int len_str2 = StringLength(str2);
	int len      = len_str1 + len_str2;
	char *ret = (char*)MemoryAllocate((len + 1)*sizeof(char));
	StringCopy(ret, str1, len_str1);
	StringCopy(ret+len_str1, str2, len_str2);
	ret[len] = 0;
	return ret;
}

char *Pxf::DuplicateReplaceChar(const char *str, const char find, const char replace)
{
	char *p = StringDuplicate(str);
	ReplaceChar(p, find, replace);
	return p;
}

char *Pxf::DuplicateReplaceString(const char *str, const char *find, const char *replace)
{
	int len_find = StringLength(find);
	int len_replace = StringLength(replace);
	int count = 0, newlen = 0;

	/*char *src = (char*)str; */

	char *ret, *q;
	const char *p = str;
	p = StringFind(str, find);
	if (!p)
		return strdup(str);

	/* count occurrences of find */
	do { count++; } while ((p = StringFind(p+len_find, find)));

	newlen = (StringLength(str) + count * (len_replace - len_find) + 1) * sizeof(char);
	ret = (char*)MemoryAllocate( newlen );
	if (!ret) return 0;

	p = (char*)str;
	q = ret;
	while(1)
	{
		const char *tmp = StringFind(p, find);
		if (!tmp)
		{
			StringCopy(q, p, StringLength(str)-(unsigned)(p-str)+1);
			ret[newlen-1] = 0;
			return ret;
		}
		MemoryCopy(q, p, (unsigned)(tmp-p));
		q += tmp-p;
		MemoryCopy(q, replace, len_replace);
		q += len_replace;
		p = tmp+len_find;
	}
	return 0;
}

/*
 * String manipulaton functions which modifies input string
 *****************************************************************************/
void Pxf::ReplaceChar(char *str, const char find, const char replace)
{
	if (!str || !find || !replace) return;
	for(;*str;str++)
		if (*str == find)
			*str = replace;
}

void Pxf::ReplaceString(char *str, const char *find, const char *replace)
{
	int len_find = StringLength(find);
	int len_replace = StringLength(replace);

	const char *p, *s;
	char *t;

	if (len_find - len_replace < 0) return;

	p = StringFind(str, find);
	if (!p) return;
	
	s = t = (char*)p;
	while(*s)
	{
		MemoryCopy(t, replace, len_replace);
		p += len_find;
		t += len_replace;
		s = StringFind(p, find);
		if(!s) s = p + StringLength(p);
		MemoryMove(t, p, (unsigned)(s-p));
		t += s-p;
		p = s;
	}
	*t = 0;
}

void Pxf::StringToLower(char *p)
{
	if (!p) return;
	for(;*p;p++)
		*p = tolower(*p);
}

void Pxf::StringToUpper(char *p)
{
	if (!p) return;
	for(;*p;p++)
		*p = toupper(*p);
}

void Pxf::StringRollLeft(char *str)
{
	int len_str = StringLength(str);
	char c = *str;
	MemoryMove(str, str+1, len_str-1);
	str[len_str-1] = c;
}

void Pxf::StringRollRight(char *str)
{
	int len_str = StringLength(str);
	char c = str[len_str-1];
	MemoryMove(str+1, str, len_str-1);
	*str = c;
}

/*
 * String find functions
 *****************************************************************************/
const char *Pxf::StringFind(const char *target, const char *find)
{
	return strstr(target, find);
}

const char *Pxf::StringFindI(const char *t, const char *f)
{
	int n = StringLength(f);
	const char *p;

	if(!t || !f) return 0;

	for(;(p = StringFindI(t, (char)*f));t = p+1)
		if (StringCompareI(p, f, n) == 0)
			return p;
	return 0;
}

const char *Pxf::StringFind(const char *target, const char find)
{
	return strchr(target, find);
}

const char *Pxf::StringFindI(const char *target, const char find)
{
	if(tolower(find) == toupper(find))
		return StringFind(target, find);
	return StringFind2(target, tolower(find), toupper(find));
}

const char *Pxf::StringFind2(const char *target, const char find, const char alt)
{
	for(;*target;target++)
		if (*target == find || *target == alt)
			return target;
	return 0;
}

const char *Pxf::StringFindRev2(const char *target, const char find, const char alt)
{
	int len_target = StringLength(target);

	if (!len_target || !find || !alt)
		return 0;

	target += len_target-1;
	for(;*target;target--)
		if (*target == find || *target == alt)
			return target;
	return 0;
}

const char *Pxf::StringFindRev(const char *t, const char *f)
{
	int lt = StringLength(t);
	int lf = StringLength(f);
	const char *p;

	if (!lf || !f || !t || lf > lt)
		return 0;

	p = (char*)t+lt-lf;
	for(;(p = StringFindRev(t, *f));t = p+1)
		if(StringCompare(p, f, lf) == 0)
			return p;

	return 0;
}

const char *Pxf::StringFindRevI(const char *t, const char *f)
{
	int lt = StringLength(t);
	int lf = StringLength(f);
	const char *p;

	if (!lf || !f || !t || lt > lt) return 0;

	p = (char*)t+lt-lf;
	for(;(p = StringFindRevI(t, *f));p--)
		if(StringCompareI(p, f, lf) == 0)
			return p;

	return 0;
}

const char *Pxf::StringFindRev(const char *target, const char find)
{
	return strrchr(target, find);
}

const char *Pxf::StringFindRevI(const char *target, const char find)
{
	if(tolower(find) == toupper(find))
		return StringFindRev(target, find);
	return StringFindRev2(target, tolower(find), toupper(find));
}

/*
 * String compare functions
 *****************************************************************************/
#ifdef _WIN32
	#define _stricmp(a,b) stricmp(a,b)
	#define _strnicmp(a,b,n) strnicmp(a,b,n)
#else
	#define _stricmp(a,b) strcasecmp(a,b)
	#define _strnicmp(a,b,n) strncasecmp(a,b,n)
#endif

int Pxf::StringCompare(const char *str1, const char *str2)
{
	return strcmp(str1, str2);
}

int Pxf::StringCompare(const char *str1, const char *str2, unsigned count)
{
	return strncmp(str1, str2, count);
}

int Pxf::StringCompareI(const char *str1, const char *str2)
{
	return _stricmp(str1, str2);
}

int Pxf::StringCompareI(const char *str1, const char *str2, unsigned count)
{
	return _strnicmp(str1, str2, count);
}

bool Pxf::IsPrefix(const char *s, const char *p)
{
	if (!s || !p) return 0;
	for(;*p && *s;p++,s++)
		if(*p != *s)
			return false;
	return true;
}

bool Pxf::IsPrefixI(const char *s, const char *p)
{
	if (!s || !p) return 0;
	for(;*p && *s;p++,s++)
		if(tolower(*p) != tolower(*s))
			return false;
	return true;
}

bool Pxf::IsSuffix(const char *s, const char *p)
{
	if (!s || !p) return 0;
	p += StringLength(p) - 1;
	s += StringLength(s) - 1;
	for(;*p && *s;s--,p--)
		if(*p != *s)
			return false;
	return true;
}

bool Pxf::IsSuffixI(const char *s, const char *p)
{
	if (!s || !p) return 0;
	p += StringLength(p) - 1;
	s += StringLength(s) - 1;
	for(;*p && *s;s--,p--)
		if(tolower(*p) != tolower(*s))
			return false;
	return true;
}

bool Pxf::IsWhitespace(const char c)
{
	return (c == ' ' || c == '\n' || c == '\t' || c == '\r');
}

bool Pxf::IsAlpha(const char c)
{
	if ((c >= 'a') && (c <= 'z')) return true;
	if ((c >= 'A') && (c <= 'Z')) return true;
	return false;
}

bool Pxf::IsNumeric(const char c)
{
	return (c >= '0') && (c <= '9');
}

bool Pxf::IsAlphanumeric(const char c)
{
	return IsAlpha(c) || IsNumeric(c) || c == '_';
}

/*
 * String conversion functions
 *****************************************************************************/
int Pxf::StringToInteger(const char *number)
{
	return atoi(number);
}

double Pxf::StringToDouble(const char *number)
{
	return atof(number);
}

/*
 * Other string functions
 *****************************************************************************/

int Pxf::StringLength(const char *str)
{
	return (int)strlen(str);
}

int Pxf::StringCount(const char *target, const char *str)
{
	int ret = 0;
	/* OPT remove one line with: ret++, target++); */
	for(;(target = strstr(target, str)); ret++)
		target += 1;
	return ret;
}

const char *Pxf::SkipWhitespace(const char *p)
{
	if (!p) return 0;
	while(IsWhitespace(*p)) p++;
	return p;
}