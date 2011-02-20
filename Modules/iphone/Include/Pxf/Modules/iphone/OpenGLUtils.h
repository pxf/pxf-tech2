#ifndef _PXF_IPHONE_GRAPHICS_OPENGLUTILS_H_
#define _PXF_IPHONE_GRAPHICS_OPENGLUTILS_H_

#include <Pxf/Modules/iphone/OpenGL.h>

namespace Pxf 
{
	namespace Graphics 
	{
		/*
			enum DeviceDataType
			{
				DDT_BYTE = 0,
				DDT_UNSIGNED_BYTE,
				DDT_SHORT,
				DDT_UNSIGNED_SHORT,
				DDT_INT,
				DDT_UNSIGNED_INT,
				DDT_FLOAT,
				DDT_DOUBLE
			};
		*/
		inline GLenum LookupDDT(DeviceDataType type)
		{
			switch(type)
			{
			case DDT_BYTE: return GL_BYTE;
			case DDT_UNSIGNED_BYTE: return GL_UNSIGNED_BYTE;
			case DDT_SHORT: return GL_SHORT;
			case DDT_UNSIGNED_SHORT: return GL_UNSIGNED_SHORT;
			case DDT_INT: return GL_INT;
			case DDT_UNSIGNED_INT: return GL_UNSIGNED_INT;
			case DDT_FLOAT: return GL_FLOAT;
			case DDT_DOUBLE: return GL_DOUBLE;
			default: PXF_ASSERT(0, "No such DeviceDataType");
			}
			return 0;
		}
	}
}

#endif // _PXF_IPHONE_GRAPHICS_OPENGLUTILS_H_
