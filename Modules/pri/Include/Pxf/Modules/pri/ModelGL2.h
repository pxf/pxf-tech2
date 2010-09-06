#ifndef _PXF_MODULES_PRI_MODELGL2_H_
#define _PXF_MODULES_PRI_MODELGL2_H_
#include <Pxf/Graphics/Model.h>
#include <Pxf/Graphics/GraphicsDevice.h>

namespace Pxf
{

	namespace Modules
	{
		class ModelGL2 : public Graphics::Model
		{
		private:
			bool Init();
		public:
			ModelGL2(Graphics::GraphicsDevice* _pDevice)
				: Graphics::Model(_pDevice)
			{ if(!Init()) Message("ModelGL2","Unable to initialize model"); }

			bool Load(const char* _FilePath);
			bool ModelGL2::Unload();
		};
	}
}

#endif // _PXF_MODULES_PRI_MODELGL2_H_