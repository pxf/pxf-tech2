#ifndef _PRELOADEDRESOURCES_
#define _PRELOADEDRESOURCES_

struct PreloadedFile
{
	const char * path;
	unsigned size;
	const unsigned char* data;
};



PreloadedFile* preloaded_files = 0;
#endif
