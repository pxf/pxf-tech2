#include <Pxf/Base/Memory.h>

#include <cstdlib>
#include <cstdio>
#include <cstring>


void *Pxf::MemoryAllocate(unsigned len)
{
	return malloc(len);
}

void *Pxf::MemoryAllocateArray(unsigned n, unsigned len)
{
	return calloc(n, len);
}

void *Pxf::MemoryReAllocate(void *p, unsigned size)
{
	return realloc(p, size);
}

void  Pxf::MemoryFree(void *p)
{
	free(p);
}

void Pxf::MemoryCopy(void *dest, const void *src, unsigned length)
{
	memcpy(dest, src, length);
}

void Pxf::MemoryMove(void *dest, const void *src, unsigned length)
{
	memmove(dest, src, length);
}

void Pxf::MemoryZero(void *dest, unsigned length)
{
	memset(dest, 0, length);
}

void Pxf::MemorySet(void *dest, char what, unsigned length)
{
	memset(dest, what, length);
}

int  Pxf::MemoryCompare(const void *buff1, const void *buff2, unsigned length)
{
	return memcmp(buff1, buff2, length);
}