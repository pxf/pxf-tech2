/*
	Todo:
	Fix weird casts & bugs
	strsplit(..,.., int *count)

	Memory manager:
	   check for memory leaks
	   attach - hirarchical memory manager

	strtrimr
	strtriml
	strtrim
	strndup
	strarr_create("aoeu","aoeu","aoue")

	string vector
	compatible with strsplit and strjoin
	    
	directorylisting
*/

#ifndef _PXF_BASE_UTILS2_H_
#define _PXF_BASE_UTILS2_H_

namespace Pxf{
namespace Utils
{
/* Hash functions */

/* Hash data using the super fast hash algorithm */
unsigned long HashData(const char *data, int len);
/* Simple hash for pointers */
size_t HashPointer(void *ptr);

/* Memory functions */

/* Allocate a buffer of size len */
void *MemoryAllocate(size_t len);      
/* Alloate a buffer with n items of size len */
void *MemoryAllocateArray(size_t n, size_t len);  
/* Re-allocate buffer p with size 'size' */
void *ReAllocate(void *p, size_t size);     
/* Free memory at p */
void MemoryFree(void *p);
/**/
void MemoryCopy(void *dest, const void *src, unsigned length);
/**/
void MemoryMove(void *dest, const void *src, unsigned length);
/**/
void MemoryZero(void *dest, unsigned length);
/**/
void MemorySet(void *dest, char what, unsigned length);
/**/
int MemoryCompare(const void *buff1, const void *buff2, unsigned length);

/* String manipulation functions */
char *StringCopy(char *dest, const char *src, unsigned count);
char *StringCat(char *dest, const char *src, unsigned count);
char *StringPadLeft(char *dest, const char *src, int width, char pad_char);
char *StringPadRight(char *dest, const char *src, int width, char pad_char);
char *StringPadCenter(char *dest, const char *src, int width, char pad_char);

/* String manipulaton functions which returns malloced memory */
char *StringDuplicate(const char *str);
char* StringDuplicateF(const char *format, ...);
char *StringSub(const char *str, int start, int length);
char *StringConcat(const char *str1, const char *str2);
char **StringSplit(const char *str, const char *split_at);
char *StringJoin(char **strs, const char *join_with);
char *DuplicateReplaceChar(const char *str, const char find, const char replace);
char *DuplicateReplaceString(const char *str, const char *find, const char *replace);

/* String manipulaton functions which modifies input string */
void ReplaceChar(char *str, const char find, const char replace);
void ReplaceString(char *str, const char *find, const char *replace);
void StringToLower(char *str);
void StringToUpper(char *str);
void StringRollLeft(char *str);
void StringRollRight(char *str);

/* String find functions */
const char *StringFind(const char *target, const char *find);
const char *StringFindI(const char *target, const char *find);
const char *StringFind(const char *target, const char find);
const char *StringFindI(const char *target, const char find);
const char *StringFind2(const char *target, const char find, const char alt);
const char *StringFindRev2(const char *target, const char find, const char alt);
const char *StringFindRev(const char *target, const char *find);
const char *StringFindRevI(const char *target, const char *find);
const char *StringFindRev(const char *target, const char find);
const char *StringFindRevI(const char *target, const char find);

/* String compare functions */
int StringCompare(const char *str1, const char *str2);
int StringCompare(const char *str1, const char *str2, unsigned count);
int StringCompareI(const char *str1, const char *str2);
int StringCompareI(const char *str1, const char *str2, unsigned count);
bool IsPrefix(char *str, char *prefix);
bool IsPrefixI(char *str, char *prefix);
bool IsSuffix(char *str, char *suffix);
bool IsSuffixI(char *str, char *suffix);
bool IsWhitespace(char c);
bool IsAlpha(char c);
bool IsNumeric(char c);
bool IsAlphanumeric(char c);

/* String convertion functions */
int StringToInteger(const char *number);
double StringToDouble(const char *number);

/* Other string functions  */
int StringLength(const char *str);
int StringCount(const char *target, const char *str); 
const char *SkipWhitespace(const char *str);

/* Random number generation */
void rand_init(unsigned long seed);
unsigned long rand_uint32();
long rand_int32();
double rand_fp64();
float rand_fp32();

/* Numeric compare functions */

/* Returns the absolute value of a */
template <typename T> 
inline T Abs(const T& a)
{ return (a < 0 ? -a : a); }

/* Returns the biggest of a and b */
template<typename T1, typename T2> 
inline T1 Max(const T1& a, const T2& b)
{ return (b < a ? a : b); }

/* Returns the smallest of a and b */
template<typename T1, typename T2> 
inline T1 Min(const T1& a,const T2& b)
{ return (a < b ? a : b); }

/* Swaps the content of a and b */
template <typename T> 
inline void Swap(T &a, T &b)
{ T t; t = a; a = b; b = t; }

/* Clamps value between min and max */
template <typename T1, typename T2, typename T3> 
inline T1 Clamp(const T1& value, const T2& min, const T3& max)
{ return (value < min ? min : (value > max ? max : value)); }

/* Usefull macros */
#define PXF_ELEMENTS_OF(t) (sizeof((t))/sizeof((t)[0]))
#define PXF_OFFSET_OF(s,m) (size_t)&reinterpret_cast<const volatile char&>((((s *)0)->m))

}
} // Pxf

#endif // _PXF_BASE_UTILS2_H_