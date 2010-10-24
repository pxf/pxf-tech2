#ifndef _PXF_GRAPHICS_FONT_H_
#define _PXF_GRAPHICS_FONT_H_

#include <Pxf/Graphics/GraphicsDevice.h>
#include <Pxf/Graphics/DeviceResource.h>

namespace Pxf {

namespace Resource {
	class Font;
}

namespace Graphics {

class Texture;

class Font : public DeviceResource
{
private:
	const Resource::Font* m_Font;
	Graphics::Texture* m_Texture;
public:
	Font(GraphicsDevice* _pDevice, const Resource::Font* _Font);
	~Font();

	float LineHeight() const;
	void MeasureString(const char* _Text, float *_Width, float *_Height, int _Max = -1);

	const Graphics::Texture* GetTexture() const
	{
		return m_Texture;
	}

	const Resource::Font* GetFont() const
	{
		return m_Font;
	}

	bool IsReady() const;

}; // Font

} // Graphics
} // Pxf

#endif // _ROLF_GRAPHICS_FONT_H_
