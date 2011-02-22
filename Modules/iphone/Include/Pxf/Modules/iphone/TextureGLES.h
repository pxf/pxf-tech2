#ifndef _PXF_IPHONE_GRAPHICS_TEXTUREGLES_H_
#define _PXF_IPHONE_GRAPHICS_TEXTUREGLES_H_

#include <Pxf/Math/Vector.h>
#include <Pxf/Modules/iphone/DeviceGLES.h>
#include <Pxf/Graphics/Texture.h>
#include <Pxf/Util/String.h>

namespace Pxf
{

	namespace Graphics
	{
		
		//! Abstract texture class
		class TextureGLES : public Texture
		{
		public:
			TextureGLES(GraphicsDevice* _pDevice);
			virtual ~TextureGLES();

			bool IsValid()
			{
				return m_TextureID != 0;
			}

			void Load(const char* _filepath);
			void LoadData(const unsigned char* _datachunk, int _width, int _height, int _channels);
			void UpdateData(const unsigned char* _datachunk, int _offsetx, int _offsety, int _width, int _height);
			void Unload();
			void Reload();

			int GetWidth();
			int GetHeight();
			int GetChannels();

			void SetMagFilter(TextureFilter _Filter);
			void SetMinFilter(TextureFilter _Filter);

			void SetClampMethod(TextureClampMethod _Method);

			Math::Vec4f CreateTextureSubset(float _x1, float _y1, float _x2, float _y2);
			
			// OGL specific
			unsigned int GetTextureID();

		private:
			GLuint m_TextureID;
			Util::String m_Filepath;
			int m_Width, m_Height, m_Channels;
			unsigned m_LogTag;
		};
	} // Graphics
} // Pxf

#endif // _PXF_IPHONE_GRAPHICS_TEXTUREGLES_H_