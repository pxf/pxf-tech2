#ifndef _PXF_BASE_MEMORY_H_
#define _PXF_BASE_MEMORY_H_

namespace Pxf {

/* Memory functions */

/* Allocate a buffer of size len */
void *MemoryAllocate(unsigned len);      
/* Alloate a buffer with n items of size len */
void *MemoryAllocateArray(unsigned n, unsigned len);  
/* Re-allocate buffer p with size 'size' */
void *MemoryReAllocate(void *p, unsigned size);     
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

}
#endif // _PXF_BASE_MEMORY_H_