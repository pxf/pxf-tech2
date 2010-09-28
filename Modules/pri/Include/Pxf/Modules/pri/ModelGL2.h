#ifndef _PXF_MODULES_PRI_MODELGL2_H_
#define _PXF_MODULES_PRI_MODELGL2_H_
#include <Pxf/Graphics/Model.h>
#include <Pxf/Graphics/GraphicsDevice.h>
#include <Pxf/Math/Vector.h>

namespace Pxf
{

	namespace Modules
	{
		class ModelGL2 : public Graphics::Model
		{
		public:
			struct Vertex
			{
				Math::Vec3f vertex;
				Math::Vec3f normal;
			};

		private:
			unsigned m_VertexCount;
			unsigned m_TriangleCount;
			unsigned m_LogTag;

			bool Init();
		public:
			ModelGL2(Graphics::GraphicsDevice* _pDevice);

			virtual ~ModelGL2();

			bool Load(const char* _FilePath);
			bool Load(Resource::Mesh* _Mesh);
			bool Unload();

			void Draw();
		};
	}
}

#endif // _PXF_MODULES_PRI_MODELGL2_H_