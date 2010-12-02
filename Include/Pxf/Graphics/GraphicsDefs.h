#ifndef _PXF_GRAPHICS_GRAPHICSDEFS_H_
#define _PXF_GRAPHICS_GRAPHICSDEFS_H_

namespace Pxf
{
	namespace Graphics
	{
		/*	Depth comparison function types, names match those found in the OpenGL api.
			There should exist similar functions in directx, albeit with different syntax.
			http://www.toymaker.info/Games/html/render_states.html	*/

		enum DepthFuncType {
			DF_NEVER,		// never pass
			DF_ALWAYS,		// always pass
			DF_LESS,		// Passes if the incoming depth value is less than the stored depth value.	
			DF_EQUAL,		// Passes if the incoming depth value is equal to the stored depth value.
			DF_LEQUAL,		// Passes if the incoming depth value is less than or equal to the stored depth value.
			DF_GREATER,		// Passes if the incoming depth value is greater than the stored depth value.
			DF_NOTEQUAL,	// Passes if the incoming depth value is not equal to the stored depth value.
			DF_GEQUAL		// Passes if the incoming depth value is greater than or equal to the stored depth value.
		};
	}
}

#endif