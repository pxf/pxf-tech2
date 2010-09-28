#ifndef _PXF_GRAPHICS_SHADER_H_
#define _PXF_GRAPHICS_SHADER_H_

#include <Pxf/Base/Debug.h>
#include <Pxf/Graphics/DeviceResource.h>

namespace Pxf {
	namespace Graphics {
		
		class Shader : public Graphics::DeviceResource
		{
		protected:
			bool m_Valid;
		public:
			Shader(GraphicsDevice* _pDevice)
				: Graphics::DeviceResource(_pDevice)
				, m_Valid(false)
			{}

			virtual ~Shader(){};
			
			bool IsValid() const { return m_Valid; }
		};
	}
}


#endif // __PXF_GRAPHICS_SHADER_H__
