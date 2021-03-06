#ifndef _PXF_IPHONE_GRAPHICS_UNIGL_H_
#define _PXF_IPHONE_GRAPHICS_UNIGL_H_
#include <Pxf/Base/Config.h>
#include <Pxf/Modules/iphone/OpenGL.h>
#include <Pxf/Math/Vector.h>
#include <Pxf/Math/Matrix.h>

/* These macros are preferred to use over GL::CheckError, since they will raise a breakpoint
   where the error occurred instead of showing you the CheckError function.
*/
#ifdef CONF_DEBUG
	#if defined(CONF_COMPILER_MSVC)
		#define PXFGLCHECK(name) do{\
			int err = GL_NO_ERROR;\
			while((err = glGetError()) != GL_NO_ERROR)\
			{Pxf::Message(name, "GL error %d => '%s'", err, gluErrorString(err));\
			__asm {int 3};}}while(0)
	#elif defined(CONF_COMPILER_GCC)
		#define PXFGLCHECK(name) do{\
			int err = GL_NO_ERROR;\
			while((err = glGetError()) != GL_NO_ERROR)\
			{Pxf::Message(name, "GL error %d => '%s'", err, gluErrorString(err));\
			asm("int $0x3\n");}}while(0)
	#else // other compilers, clang mayhaps?
		#define PXFGLCHECK(name) do{Pxf::Graphics::GL::CheckError(name);}while(0)
	#endif
#else
	#define PXFGLCHECK(name) do{}while(0)
#endif

#define OPENGL_TRAIT(Type, GLType, GLSize)\
	template <>\
struct OpenGL_Traits< Type >\
	{\
	enum {GL_TYPE = GLType, GL_SIZE = GLSize};\
	}


#define OPENGL_TRAIT_T(Type, GLType, GLSize)\
	template <typename T>\
struct OpenGL_Traits< Type >\
	{\
	enum {GL_TYPE = GLType, GL_SIZE = GLSize};\
	}

namespace Pxf
{
	namespace Graphics
	{
		namespace GL
		{
			// Generic helper functions
			inline GLvoid* BufferObjectPtr(unsigned int ptr)
			{
				return (GLvoid*)( ((char*)NULL) + ptr );
			};

			//
			// Buffer functions
			//

			void SetupExtensions();

			// Buffer
			extern unsigned int ARRAY_BUFFER;
			extern PFNGLDELETEBUFFERSPROC DeleteBuffers;
			extern PFNGLBINDBUFFERPROC BindBuffer;
			extern PFNGLGENBUFFERSPROC GenBuffers;
			extern PFNGLBUFFERDATAPROC BufferData;
			extern PFNGLBUFFERSUBDATAPROC BufferSubData;
			extern PFNGLMAPBUFFERPROC MapBuffer;
			extern PFNGLUNMAPBUFFERPROC UnmapBuffer;

			extern PFNGLDRAWBUFFERSPROC DrawBuffers;

			// Shader
			extern unsigned int COMPILE_STATUS;
			extern unsigned int LINK_STATUS;
			extern unsigned int VERTEX_SHADER;
			extern unsigned int FRAGMENT_SHADER;

			extern PFNGLCREATEPROGRAMPROC CreateProgram;
			extern PFNGLCREATESHADERPROC CreateShader;
			extern PFNGLSHADERSOURCEPROC ShaderSource;
			extern PFNGLCOMPILESHADERPROC CompileShader;
			extern PFNGLATTACHSHADERPROC AttachShader;
			extern PFNGLLINKPROGRAMPROC LinkProgram;
			extern PFNGLUSEPROGRAMPROC UseProgram;
			extern PFNGLDETACHSHADERPROC DetachShader;
			extern PFNGLDELETESHADERPROC DeleteShader;
			extern PFNGLDELETEPROGRAMPROC DeleteProgram;
			extern PFNGLGETSHADERIVPROC GetShaderiv;
			extern PFNGLGETPROGRAMIVPROC GetProgramiv;
			extern PFNGLGETSHADERINFOLOGPROC GetShaderInfoLog;
			extern PFNGLGETPROGRAMINFOLOGPROC GetProgramInfoLog;
			extern PFNGLGETUNIFORMLOCATIONPROC GetUniformLocation;

			
			// Uniform (float, float vectors)
			extern PFNGLUNIFORM1FPROC Uniform1f;
			extern PFNGLUNIFORM2FPROC Uniform2f;
			extern PFNGLUNIFORM3FPROC Uniform3f;
			extern PFNGLUNIFORM4FPROC Uniform4f;
			extern PFNGLUNIFORM1FVPROC Uniform1fv;
			extern PFNGLUNIFORM2FVPROC Uniform2fv;
			extern PFNGLUNIFORM3FVPROC Uniform3fv;
			extern PFNGLUNIFORM4FVPROC Uniform4fv;

			// Uniform (integer, integer vectors)
			extern PFNGLUNIFORM1IPROC Uniform1i;
			extern PFNGLUNIFORM2IPROC Uniform2i;
			extern PFNGLUNIFORM3IPROC Uniform3i;
			extern PFNGLUNIFORM4IPROC Uniform4i;
			extern PFNGLUNIFORM1IVPROC Uniform1iv;
			extern PFNGLUNIFORM2IVPROC Uniform2iv;
			extern PFNGLUNIFORM3IVPROC Uniform3iv;
			extern PFNGLUNIFORM4IVPROC Uniform4iv;

			// Uniform (matrices)
			extern PFNGLUNIFORMMATRIX2FVPROC UniformMatrix2fv;
			extern PFNGLUNIFORMMATRIX3FVPROC UniformMatrix3fv;
			extern PFNGLUNIFORMMATRIX4FVPROC UniformMatrix4fv;

			/* Todo: OpenGL 3.0 */
			//void BindFragDataLocation(int _Index, const char* _Name);
			//void BindAttribLocation(int _Index, const char* _Name);


			inline void CheckError(const char* _Source)
			{
				int error = GL_NO_ERROR;
				while((error = glGetError()) != GL_NO_ERROR)
				{
					Message(_Source, "GL error %d => '%s'", error, gluErrorString(error));
					#if defined(CONF_FAMILY_WINDOWS) && defined(CONF_COMPILER_MSVC)
						__asm int 3;
					#endif
				}
			}

			// OpenGL type traits
			//	http://www.gamedev.net/reference/programming/features/ogltypetraits/
			//	http://www.gamedev.net/community/forums/topic.asp?topic_id=510016&whichpage=1&#3325591

			template <class T>
			struct OpenGL_Traits {};

			OPENGL_TRAIT(unsigned int,   GL_UNSIGNED_INT,   1);
			OPENGL_TRAIT(int,			GL_INT,			1);
			OPENGL_TRAIT(unsigned short, GL_UNSIGNED_SHORT, 1);
			OPENGL_TRAIT(short,		  GL_SHORT,		  1);
			OPENGL_TRAIT(float,		  GL_FLOAT,		  1);
			OPENGL_TRAIT(double,		 GL_DOUBLE,		 1);
			OPENGL_TRAIT(bool,		   GL_BOOL,		   1);
			OPENGL_TRAIT(char,		   GL_BYTE,		   1);
			OPENGL_TRAIT_T(Math::Vector2D<T>, OpenGL_Traits<T>::GL_TYPE, 2);
			OPENGL_TRAIT_T(Math::Vector3D<T>, OpenGL_Traits<T>::GL_TYPE, 3);
			OPENGL_TRAIT_T(Math::Vector4D<T>, OpenGL_Traits<T>::GL_TYPE, 4);

			//
			// Wrapper functions for type traits, helper functions etc.
			//

			template <typename T>
			inline void VertexPointer(const T* _Ptr, GLsizei _Stride = 0)
			{
				glVertexPointer(OpenGL_Traits<T>::GL_SIZE, OpenGL_Traits<T>::GL_TYPE, _Stride, _Ptr);
			}

			template <typename T>
			inline void ColorPointer(const T* _Ptr, GLsizei _Stride = 0)
			{
				glColorPointer(OpenGL_Traits<T>::GL_SIZE, OpenGL_Traits<T>::GL_TYPE, _Stride, _Ptr);
			}

			template <typename T>
			inline void TexCoordPointer(const T* _Ptr, GLsizei _Stride = 0)
			{
				glTexCoordPointer(OpenGL_Traits<T>::GL_SIZE, OpenGL_Traits<T>::GL_TYPE, _Stride, _Ptr);
			}

			template <typename T>
			inline void NormalPointer(const Math::Vector3D<T>* _Ptr, GLsizei _Stride = 0)
			{
				glNormalPointer(OpenGL_Traits<T>::GL_TYPE, _Stride, _Ptr);
			}
		} // GL
	} // Graphics
} // Pxf

#undef OPENGL_TRAIT
#undef OPENGL_TRAIT_T


#endif // _PXF_IPHONE_GRAPHICS_UNIGL_H_
