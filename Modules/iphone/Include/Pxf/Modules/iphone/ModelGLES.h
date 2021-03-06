#ifndef _PXF_IPHONE_MODULES_PRI_MODELGLES_H_
#define _PXF_IPHONE_MODULES_PRI_MODELGLES_H_
#include <Pxf/Graphics/Model.h>
#include <Pxf/Graphics/GraphicsDevice.h>
#include <Pxf/Math/Vector.h>

namespace Pxf
{

	namespace Modules
	{
		class ModelGLES : public Graphics::Model
		{
		public:
			struct Vertex
			{
				Math::Vec3f vertex;
				Math::Vec3f normal;
				Math::Vec2f texcoord;
			};

		private:
			unsigned m_VertexCount;
			unsigned m_TriangleCount;
			unsigned m_LogTag;

			bool Init();
		public:
			ModelGLES(Graphics::GraphicsDevice* _pDevice);

			virtual ~ModelGLES();

			bool Load(const char* _FilePath);
			bool Load(Resource::Mesh* _Mesh);
			bool Unload();

			void Draw();
		};
	}
}

#endif // _PXF_IPHONE_MODULES_PRI_MODELGLES_H_