#include <Pxf/Base/Utils2.h>
#include <cstdlib>
#include <cstdio>
#include <cstring>
#include <cstdarg> /* strfmt_n*/
#include <cctype>
/******************************************************************************
* Hash functions
* - Super fast hash, from http://www.azillionmonkeys.com/qed/hash.html
*******************************************************************************/
#undef pxf_get16bits
#if (defined(__GNUC__) && defined(__i386__)) || defined(__WATCOMC__) || defined(_MSC_VER) || defined (__BORLANDC__) || defined (__TURBOC__)
#define pxf_get16bits(d) (*((const unsigned short *) (d)))
#endif

#if !defined (pxf_get16bits)
#define pxf_get16bits(d) ((((unsigned long)(((const unsigned char *)(d))[1])) << 8)+(unsigned long)(((const unsigned char *)(d))[0]) )
#endif

unsigned long Pxf::Utils::HashData(const char *data, int len)
{
	unsigned long hash = len, tmp;
	int rem;

	if (len <= 0 || data == NULL) return 0;

	rem = len & 3;
	len >>= 2;

	/* Main loop */
	for (;len > 0; len--) {
		hash  += pxf_get16bits (data);
		tmp    = (pxf_get16bits (data+2) << 11) ^ hash;
		hash   = (hash << 16) ^ tmp;
		data  += 2*sizeof (unsigned short);
		hash  += hash >> 11;
	}

	/* Handle end cases */
	switch (rem) {
		case 3: hash += pxf_get16bits (data);
				hash ^= hash << 16;
				hash ^= data[sizeof (unsigned short)] << 18;
				hash += hash >> 11;
				break;
		case 2: hash += pxf_get16bits (data);
				hash ^= hash << 11;
				hash += hash >> 17;
				break;
		case 1: hash += *data;
				hash ^= hash << 10;
				hash += hash >> 1;
	}

	/* Force "avalanching" of final 127 bits */
	hash ^= hash << 3;
	hash += hash >> 5;
	hash ^= hash << 4;
	hash += hash >> 17;
	hash ^= hash << 25;
	hash += hash >> 6;

	return hash;
}

size_t Pxf::Utils::HashPointer(void *ptr)
{
	size_t x = (size_t)ptr;
	x = x + (x>>6) + (x >> 19);
	x += x << 16;

	x ^= x << 3;
	x += x >> 5;
	x ^= x << 2;
	x += x >> 15;
	x ^= x << 10;
	return x + (x>>6) + (x >> 19);
}

/******************************************************************************
* Memory functions
*******************************************************************************/
void *Pxf::Utils::MemoryAllocate(size_t len)
{
	return malloc(len);
}

void *Pxf::Utils::MemoryAllocateArray(size_t n, size_t len)
{
	return calloc(n, len);
}

void *Pxf::Utils::ReAllocate(void *p, size_t size)
{
	return realloc(p, size);
}

void  Pxf::Utils::MemoryFree(void *p)
{
	free(p);
}

void Pxf::Utils::MemoryCopy(void *dest, const void *src, unsigned length)
{
	memcpy(dest, src, length);
}

void Pxf::Utils::MemoryMove(void *dest, const void *src, unsigned length)
{
	memmove(dest, src, length);
}

void Pxf::Utils::MemoryZero(void *dest, unsigned length)
{
	memset(dest, 0, length);
}

void Pxf::Utils::MemorySet(void *dest, char what, unsigned length)
{
	memset(dest, what, length);
}

int  Pxf::Utils::MemoryCompare(const void *buff1, const void *buff2, unsigned length)
{
	return memcmp(buff1, buff2, length);
}

/******************************************************************************
* String functions
*******************************************************************************/


/*
 * String manipulation functions
 *****************************************************************************/
char *Pxf::Utils::StringCopy(char *dest, const char *src, unsigned count)
{
	strncpy(dest, src, count);
	return dest;
}

char *Pxf::Utils::StringCat(char *dest, const char *src, unsigned count)
{
	strncat(dest, src, count);
	return dest;
}

char *Pxf::Utils::StringPadLeft(char *dest, const char *src, int width, char pad_char)
{
	int len = StringLength(src);
	MemorySet(dest, pad_char, width);
	StringCopy(dest, src, Min(len, width));
	/*dest[width] = 0x0; */
	return dest;
}

char *Pxf::Utils::StringPadRight(char *dest, const char *src, int width, char pad_char)
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

char *Pxf::Utils::StringPadCenter(char *dest, const char *src, int width, char pad_char)
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
char *Pxf::Utils::StringDuplicate(const char *str)
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

char *Pxf::Utils::StringSub(const char *str, int start, int length)
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


char *Pxf::Utils::StringConcat(const char *str1, const char *str2)
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

char **Pxf::Utils::StringSplit(const char *str, const char *split_at)
{
	/*
		in-memory format (on a 32-bit system):
		str1|str2|str3 ->
		  4b    4b    4b    4b   null-terminated char*'s  
		[ptr1][ptr2][ptr3][NULL][str1][str2][str3]
	*/
	char *ret      = 0;
	int len_splits = StringLength(split_at);
	int len_str    = StringLength(str);
	int num_splits = 0;

	int ptr_index = 0;
	int str_index = 0;
	int str_base  = 0;

	int n, m, *temp = 0;

	/* Precalculate number of parts */
	for (n=0; n < len_splits; n++)
	{
		for(m=0; m < len_str; m++)
		{
			if (str[m] == split_at[n])
				num_splits++;
		}
	}

	/*  Offset to strings */
	str_base = (num_splits+2) * sizeof(int);

	/*  Allocate memory chunk */
	ret = (char *)MemoryAllocate(str_base + (len_str+1)*sizeof(char));
	if (!ret) return 0; 
	
	MemoryCopy(ret+str_base, str, len_str+1);	
	/*  use a temporary integer pointer to write a pointer size zero-terminator */
	temp = (int*)ret;

	for(n=0; n < len_str+1; n++)
	{
		for (m=0; m < len_splits; m++)
		{
			if (str[n] == split_at[m] || str[n] == 0)
			{
				ret[str_base+n] = 0x0;
				temp[ptr_index++] = (int)(ret+str_base+str_index);
				str_index = n+1;
			}
		}
	}

	temp[num_splits+1] = 0x0;

	return (char**) ret;
}

char *Pxf::Utils::StringJoin(char **strs, const char *join_with)
{
	char *ret;

	int len_ret  = 0;
	int len_join = StringLength(join_with);
	int num_strs = 0;
	int pos = 0;

	/*  Calculate size of destination string */
	char **p = 0;
	for(p=strs; *p; p++)
	{
		len_ret += StringLength(*p);
		num_strs++;
	}
	num_strs--;
	len_ret += num_strs * len_join;

	/*  Allocate memory */
	ret = (char *)MemoryAllocate((len_ret+1) * sizeof(char));
	if (!ret) return 0; 

	/*  Copy data */
	for(p=strs; *p; p++)
	{
		int len = strlen(*p);
		StringCopy(ret+pos, *p, len);
		pos += len;

		if (!num_strs) break;

		/*  Add join_with */
		StringCopy(ret+pos, join_with, len_join);
		pos += len_join;
		num_strs--;
	}
	ret[pos] = 0;

	return ret;
}

char *Pxf::Utils::DuplicateReplaceChar(const char *str, const char find, const char replace)
{
	char *p = StringDuplicate(str);
	ReplaceChar(p, find, replace);
	return p;
}

char *Pxf::Utils::DuplicateReplaceString(const char *str, const char *find, const char *replace)
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
void Pxf::Utils::ReplaceChar(char *str, const char find, const char replace)
{
	if (!str || !find || !replace) return;
	for(;*str;str++)
		if (*str == find)
			*str = replace;
}

void Pxf::Utils::ReplaceString(char *str, const char *find, const char *replace)
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

void Pxf::Utils::StringToLower(char *p)
{
	if (!p) return;
	for(;*p;p++)
		*p = tolower(*p);
}

void Pxf::Utils::StringToUpper(char *p)
{
	if (!p) return;
	for(;*p;p++)
		*p = toupper(*p);
}

void Pxf::Utils::StringRollLeft(char *str)
{
	int len_str = StringLength(str);
	char c = *str;
	MemoryMove(str, str+1, len_str-1);
	str[len_str-1] = c;
}

void Pxf::Utils::StringRollRight(char *str)
{
	int len_str = StringLength(str);
	char c = str[len_str-1];
	MemoryMove(str+1, str, len_str-1);
	*str = c;
}

/*
 * String find functions
 *****************************************************************************/
const char *Pxf::Utils::StringFind(const char *target, const char *find)
{
	return strstr(target, find);
}

const char *Pxf::Utils::StringFindI(const char *t, const char *f)
{
	int n = StringLength(f);
	const char *p;

	if(!t || !f) return 0;

	for(;(p = StringFindI(t, (char)*f));t = p+1)
		if (StringCompareI(p, f, n) == 0)
			return p;
	return 0;
}

const char *Pxf::Utils::StringFind(const char *target, const char find)
{
	return strchr(target, find);
}

const char *Pxf::Utils::StringFindI(const char *target, const char find)
{
	if(tolower(find) == toupper(find))
		return StringFind(target, find);
	return StringFind2(target, tolower(find), toupper(find));
}

const char *Pxf::Utils::StringFind2(const char *target, const char find, const char alt)
{
	for(;*target;target++)
		if (*target == find || *target == alt)
			return target;
	return 0;
}

const char *Pxf::Utils::StringFindRev2(const char *target, const char find, const char alt)
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

const char *Pxf::Utils::StringFindRev(const char *t, const char *f)
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

const char *Pxf::Utils::StringFindRevI(const char *t, const char *f)
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

const char *Pxf::Utils::StringFindRev(const char *target, const char find)
{
	return strrchr(target, find);
}

const char *Pxf::Utils::StringFindRevI(const char *target, const char find)
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

int Pxf::Utils::StringCompare(const char *str1, const char *str2)
{
	return strcmp(str1, str2);
}

int Pxf::Utils::StringCompare(const char *str1, const char *str2, unsigned count)
{
	return strncmp(str1, str2, count);
}

int Pxf::Utils::StringCompareI(const char *str1, const char *str2)
{
	return _stricmp(str1, str2);
}

int Pxf::Utils::StringCompareI(const char *str1, const char *str2, unsigned count)
{
	return _strnicmp(str1, str2, count);
}

bool Pxf::Utils::IsPrefix(char *s, char *p)
{
	if (!s || !p) return 0;
	for(;*p && *s;p++,s++)
		if(*p != *s)
			return false;
	return true;
}

bool Pxf::Utils::IsPrefixI(char *s, char *p)
{
	if (!s || !p) return 0;
	for(;*p && *s;p++,s++)
		if(tolower(*p) != tolower(*s))
			return false;
	return true;
}

bool Pxf::Utils::IsSuffix(char *s, char *p)
{
	if (!s || !p) return 0;
	p += StringLength(p) - 1;
	s += StringLength(s) - 1;
	for(;*p && *s;s--,p--)
		if(*p != *s)
			return false;
	return true;
}

bool Pxf::Utils::IsSuffixI(char *s, char *p)
{
	if (!s || !p) return 0;
	p += StringLength(p) - 1;
	s += StringLength(s) - 1;
	for(;*p && *s;s--,p--)
		if(tolower(*p) != tolower(*s))
			return false;
	return true;
}

bool Pxf::Utils::IsWhitespace(char c)
{
	return (c == ' ' || c == '\n' || c == '\t' || c == '\r');
}

bool Pxf::Utils::IsAlpha(char c)
{
	if ((c >= 'a') && (c <= 'z')) return true;
	if ((c >= 'A') && (c <= 'Z')) return true;
	return false;
}

bool Pxf::Utils::IsNumeric(char c)
{
	return (c >= '0') && (c <= '9');
}

bool Pxf::Utils::IsAlphanumeric(char c)
{
	return IsAlpha(c) || IsNumeric(c) || c == '_';
}

/*
 * String conversion functions
 *****************************************************************************/
int Pxf::Utils::StringToInteger(const char *number)
{
	return atoi(number);
}

double Pxf::Utils::StringToDouble(const char *number)
{
	return atof(number);
}

/*
 * Other string functions
 *****************************************************************************/

int Pxf::Utils::StringLength(const char *str)
{
	return (int)strlen(str);
}

int Pxf::Utils::StringCount(const char *target, const char *str)
{
	int ret = 0;
	/* OPT remove one line with: ret++, target++); */
	for(;(target = strstr(target, str)); ret++)
		target += 1;
	return ret;
}

const char *Pxf::Utils::SkipWhitespace(const char *p)
{
	if (!p) return 0;
	while(IsWhitespace(*p)) p++;
	return p;
}

/*
 * Random number generation, Mersenne twister
 *
 * http://en.literateprograms.org/Mersenne_twister_(C)
 * + Optimized initiation (I think)
 *****************************************************************************/

#define PXF__MT_LEN       624
#define PXF__MT_IA        397
#define PXF__MT_IB        (PXF__MT_LEN - PXF__MT_IA)
#define PXF__UPPER_MASK   0x80000000
#define PXF__LOWER_MASK   0x7FFFFFFF
#define PXF__MATRIX_A     0x9908B0DF
#define PXF__TWIST(b,i,j) ((b)[i] & PXF__UPPER_MASK) | ((b)[j] & PXF__LOWER_MASK)
#define PXF__MAGIC(s)     (((s)&1)*PXF__MATRIX_A)


int mt_index = 0;
unsigned long mt_buffer[PXF__MT_LEN];

void rand_init(unsigned long seed)
{
	int i;
	mt_buffer[0] = seed & 0xffffffffUL;
	for (i = 1; i < PXF__MT_LEN; i++)
		mt_buffer[i] = (1812433253UL * (mt_buffer[i-1] ^ (mt_buffer[i-1] >> 30)) + i) & 0xffffffffUL;
}

// Interval: [0, 0xffffffff]
unsigned long Pxf::Utils::rand_uint32()
{
	unsigned long * b = mt_buffer;
	int idx = mt_index;
	unsigned long s;
	int i;

	if (idx == PXF__MT_LEN*sizeof(unsigned long))
	{
		idx = 0;
		i = 0;
		for (; i < PXF__MT_IB; i++) {
			s = PXF__TWIST(b, i, i+1);
			b[i] = b[i + PXF__MT_IA] ^ (s >> 1) ^ PXF__MAGIC(s);
		}
		for (; i < PXF__MT_LEN-1; i++) {
			s = PXF__TWIST(b, i, i+1);
			b[i] = b[i - PXF__MT_IB] ^ (s >> 1) ^ PXF__MAGIC(s);
		}
	    
		s = PXF__TWIST(b, PXF__MT_LEN-1, 0);
		b[PXF__MT_LEN-1] = b[PXF__MT_IA-1] ^ (s >> 1) ^ PXF__MAGIC(s);
	}
	mt_index = idx + sizeof(unsigned long);
	return *(unsigned long *)((unsigned char *)b + idx);
}

// Interval: [0, 0x7fffffff]
long  Pxf::Utils::rand_int32()
{
	return (long)(rand_uint32() >> 1);
}

// Interval: [0, 1]
double  Pxf::Utils::rand_fp64()
{
	return rand_uint32() / ((double) (1 << 16) * (1 << 16));
}

float  Pxf::Utils::rand_fp32()
{
	return rand_uint32() / ((float) (1 << 16) * (1 << 16));
}
