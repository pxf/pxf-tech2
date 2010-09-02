#ifndef _PXF_GRAPHICS_OPENGL_UTILS_H_
#define _PXF_GRAPHICS_OPENGL_UTILS__H_
#include <Pxf/Modules/pri/OpenGL.h>


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

			// Constants
			enum OpenGL_Constant
			{
				ARRAY_BUFFER = GL_ARRAY_BUFFER,
			};

			// Buffer functions

			void DeleteBuffers(GLsizei n, const GLuint* buffers)
			{
				/* OpenGL 1.5 */
				if (GLEW_VERSION_1_5)
				{
					glDeleteBuffers(n, buffers);
				}
				/* OpenGL 1.4 with VBO support as extension */
				else if (glewIsSupported("GL_VERSION_1_4  ARB_vertex_buffer_object"))
				{
					glDeleteBuffersARB(n, buffers);
				}
			}

			void BindBuffer(GLenum target, GLuint buffer)
			{
				/* OpenGL 1.5 */
				if (GLEW_VERSION_1_5)
				{
					glBindBuffer(target, buffer);
				}
				/* OpenGL 1.4 with VBO support as extension */
				else if (glewIsSupported("GL_VERSION_1_4  ARB_vertex_buffer_object"))
				{
					GLenum t = 0;
					if (target == GL_ARRAY_BUFFER)
						t = GL_ARRAY_BUFFER_ARB;
					glBindBufferARB(t, buffer);
				}
			}

			void GenBuffers(GLsizei n, GLuint* buffers)
			{
				/* OpenGL 1.5 */
				if (GLEW_VERSION_1_5)
				{
					glGenBuffers(n, buffers);
				}
				/* OpenGL 1.4 with VBO support as extension */
				else if (glewIsSupported("GL_VERSION_1_4  ARB_vertex_buffer_object"))
				{
					glGenBuffersARB(n, buffers);
				}
			}

			void BufferData(GLenum target, GLsizeiptr size, const GLvoid* data, GLenum usage)
			{
				/* OpenGL 1.5 */
				if (GLEW_VERSION_1_5)
				{
					glBufferData(target, size, data, usage);
				}
				/* OpenGL 1.4 with VBO support as extension */
				else if (glewIsSupported("GL_VERSION_1_4  ARB_vertex_buffer_object"))
				{
					GLenum t = 0;
					if (target == GL_ARRAY_BUFFER)
						t = GL_ARRAY_BUFFER_ARB;
					glBufferDataARB(t, size, data, usage);
				}
			}

			void BufferSubData(GLenum target, GLintptr offset, GLsizeiptr size, const GLvoid * data)
			{
				/* OpenGL 1.5 */
				if (GLEW_VERSION_1_5)
				{
					glBufferSubData(target, offset, size, data);
				}
				/* OpenGL 1.4 with VBO support as extension */
				else if (glewIsSupported("GL_VERSION_1_4  ARB_vertex_buffer_object"))
				{
					GLenum t = 0;
					if (target == GL_ARRAY_BUFFER)
						t = GL_ARRAY_BUFFER_ARB;
					glBufferSubDataARB(t, offset, size, data);
				}
			}

			void* MapBuffer(GLenum target, GLenum access)
			{
				/* OpenGL 1.5 */
				if (GLEW_VERSION_1_5)
				{
					return glMapBuffer(target, access);
				}
				/* OpenGL 1.4 with VBO support as extension */
				else if (glewIsSupported("GL_VERSION_1_4  ARB_vertex_buffer_object"))
				{
					GLenum t = 0;
					if (target == GL_ARRAY_BUFFER)
						t = GL_ARRAY_BUFFER_ARB;
					return glMapBufferARB(t, access);
				}
				return 0;
			}

			GLboolean UnmapBuffer(GLenum target)
			{
				/* OpenGL 1.5 */
				if (GLEW_VERSION_1_5)
				{
					return glUnmapBuffer(target);
				}
				/* OpenGL 1.4 with VBO support as extension */
				else if (glewIsSupported("GL_VERSION_1_4  ARB_vertex_buffer_object"))
				{
					GLenum t = 0;
					if (target == GL_ARRAY_BUFFER)
						t = GL_ARRAY_BUFFER_ARB;
					return glUnmapBufferARB(t);
				}
				return false;
			}

			inline void CheckError(const char* _Source)
			{
				int error = GL_NO_ERROR;
				while((error = glGetError()) != GL_NO_ERROR)
				{
					Message(_Source, "GL error %d => '%s'", error, gluErrorString(error));
#if defined(CONF_FAMILY_WINDOWS )&& defined(CONF_COMPILER_MSVC)
					__asm int 3;
#endif
				}
			}

			// OpenGL type traits
			//    http://www.gamedev.net/reference/programming/features/ogltypetraits/
			//    http://www.gamedev.net/community/forums/topic.asp?topic_id=510016&whichpage=1&#3325591

			template <class T>
			struct OpenGL_Traits {};

			OPENGL_TRAIT(unsigned int,   GL_UNSIGNED_INT,   1);
			OPENGL_TRAIT(int,            GL_INT,            1);
			OPENGL_TRAIT(unsigned short, GL_UNSIGNED_SHORT, 1);
			OPENGL_TRAIT(short,          GL_SHORT,          1);
			OPENGL_TRAIT(float,          GL_FLOAT,          1);
			OPENGL_TRAIT(double,         GL_DOUBLE,         1);
			OPENGL_TRAIT(bool,           GL_BOOL,           1);
			OPENGL_TRAIT(char,           GL_BYTE,           1);
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


#endif // _PXF_GRAPHICS_OPENGL_UTILS_H_
