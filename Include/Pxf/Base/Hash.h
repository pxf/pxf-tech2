#ifndef _PXF_BASE_HASH_H_
#define _PXF_BASE_HASH_H_

namespace Pxf {

/* Hash data using the super fast hash algorithm */
unsigned long Hash(const char *data, int len);

/* Simple hash for pointers */
unsigned long HashPtr(void *ptr);

}
#endif // _PXF_BASE_HASH_H_