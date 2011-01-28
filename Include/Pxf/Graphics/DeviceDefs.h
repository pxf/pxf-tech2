#ifndef _PXF_GRAPHICS_DEVICEDEFS_H_
#define _PXF_GRAPHICS_DEVICEDEFS_H_

namespace Pxf
{
	namespace Graphics
	{
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

		enum DeviceType
		{
			EOpenGL2,
			EOpenGL3,
			EOpenGLES11,
			EOpenGLES2,
			EDirect3D9,
			EDirect3D10,
			EDirect3D11
		};
	}
}

#endif