#ifndef _PXF_GRAPHICS_DEVICEDEFS_H_
#define _PXF_GRAPHICS_DEVICEDEFS_H_

namespace Pxf
{
	namespace Graphics
	{
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