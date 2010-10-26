#ifndef _PXF_GRAPHICS_WINDOW_H_
#define _PXF_GRAPHICS_WINDOW_H_

#include <Pxf/Graphics/DeviceResource.h>
#include <Pxf/Graphics/GraphicsDevice.h>

namespace Pxf{
	namespace Graphics
	{
		class Window : public DeviceResource
		{
		public:

			Window(GraphicsDevice* _Device)
				: DeviceResource(_Device)
			{ }

			typedef void(*WindowSizeCallback)(int _Width, int _Height);

			virtual ~Window(){};
			virtual bool Open() = 0;
			virtual bool Close() = 0;
			virtual void Swap() = 0;

			virtual void SetResizeCallback(WindowSizeCallback fun) = 0;
			virtual void SetTitle(const char *_title) = 0;

			virtual int GetFPS() = 0;
			virtual int GetWidth() = 0;
			virtual int GetHeight() = 0;
			virtual float GetAspectRatio() = 0;
			virtual const char* GetContextTypeName() = 0;

			virtual bool IsOpen() = 0;
			virtual bool IsMinimized() = 0;
			virtual bool IsActive() = 0;
		};

	} // Graphics
} // Pxf

#endif //_PXF_GRAPHICS_WINDOW_H_
