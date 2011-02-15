#ifndef _PXF_BASE_STRING_H_
#define _PXF_BASE_STRING_H_

/*
	Various string routines, written a long long time ago.

	Warning: Minefield. Beware of nukes.

	TODO: StripWhitespace(char* str), StripChars(char* str, char* chars)
*/


namespace Pxf {

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
bool IsPrefix(const char *str, const char *prefix);
bool IsPrefixI(const char *str, const char *prefix);
bool IsSuffix(const char *str, const char *suffix);
bool IsSuffixI(const char *str, const char *suffix);
bool IsWhitespace(const char c);
bool IsAlpha(const char c);
bool IsNumeric(const char c);
bool IsAlphanumeric(const char c);

/* String convertion functions */
int StringToInteger(const char *number);
double StringToDouble(const char *number);

/* Other string functions  */
int StringLength(const char *str);
int StringCount(const char *target, const char *str); 
const char *SkipWhitespace(const char *str);

}
#endif // _PXF_BASE_STRING_H_