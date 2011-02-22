#ifndef _PXF_IPHONE_GRAPHICS_OPENGL_H_
#define _PXF_IPHONE_GRAPHICS_OPENGL_H_

#include <Pxf/Base/Config.h>
/*
#if defined(CONF_PLATFORM_MACOSX) 

	#include "TargetConditionals.h"
	#if defined(TARGET_OS_IPHONE) || defined(TARGET_SIMULATOR_IPHONE)
		// need something for ES2 aswell :(
		#include <OpenGLES/ES1/gl.h>
		#include <OpenGLES/ES1/glext.h>
	#endif
#else
*/
	#include <GL/glew.h>


	#if defined(CONF_PLATFORM_MACOSX)
		#include <OpenGL/OpenGL.h>
		#include <OpenGL/gl.h>
		#include <OpenGL/glu.h>
	#else
		#include <GL/gl.h>
		#include <GL/glu.h>
	#endif

	#include <GL/glfw.h>
//#endif

#endif // _PXF_IPHONE_GRAPHICS_OPENGL_H_
