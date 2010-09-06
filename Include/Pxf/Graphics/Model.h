#ifndef _PXF_GRAPHICS_MODEL_H_
#define _PXF_GRAPHICS_MODEL_H_

#include <Pxf/Base/Debug.h>
#include <Pxf/Graphics/DeviceResource.h>
#include <Pxf/Graphics/VertexBuffer.h>

namespace Pxf {
	namespace Graphics {

		class Model : public Graphics::DeviceResource
		{
		protected:
			VertexBuffer* m_VertexBuffer;
			
			virtual bool Init() { return true; }
		public:
			Model(GraphicsDevice* _pDevice)
				: Graphics::DeviceResource(_pDevice)
			{}

			virtual bool Load(const char* _FilePath) = 0;
			//virtual void LoadData(Chunk* _Data) = 0;

			virtual bool Unload() = 0;
			virtual void Draw() = 0;
		};
	}
}


#endif // __PXF_GRAPHICS_MODEL_H__
