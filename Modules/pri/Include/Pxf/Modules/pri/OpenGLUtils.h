#ifndef _PXF_GRAPHICS_OPENGLUTILS_H_
#define _PXF_GRAPHICS_OPENGLUTILS_H_

#include <Pxf/Modules/pri/OpenGL.h>

namespace Pxf 
{
	namespace Graphics 
	{
		/*
			enum DeviceDataType
			{
				DDT_BYTE = 0,
				DDT_UNSIGNED_BYTE,
				DDT_SHORT,
				DDT_UNSIGNED_SHORT,
				DDT_INT,
				DDT_UNSIGNED_INT,
				DDT_FLOAT,
				DDT_DOUBLE
			};
		*/
		inline GLenum LookupDDT(DeviceDataType type)
		{
			switch(type)
			{
			case DDT_BYTE: return GL_BYTE;
			case DDT_UNSIGNED_BYTE: return GL_UNSIGNED_BYTE;
			case DDT_SHORT: return GL_SHORT;
			case DDT_UNSIGNED_SHORT: return GL_UNSIGNED_SHORT;
			case DDT_INT: return GL_INT;
			case DDT_UNSIGNED_INT: return GL_UNSIGNED_INT;
			case DDT_FLOAT: return GL_FLOAT;
			case DDT_DOUBLE: return GL_DOUBLE;
			default: PXF_ASSERT(0, "No such DeviceDataType");
			}
			return 0;
		}

    /*
		// Should this be elsewhere?
		bool IsExtensionSupported( char* szTargetExtension )
		{
			const unsigned char *pszExtensions = NULL;
			const unsigned char *pszStart;
			unsigned char *pszWhere, *pszTerminator;

			// Extension names should not have spaces
			pszWhere = (unsigned char *) strchr( szTargetExtension, ' ' );
			if( pszWhere || *szTargetExtension == '\0' )
				return false;

			// Get Extensions String
			pszExtensions = glGetString( GL_EXTENSIONS );

			// Search The Extensions String For An Exact Copy
			pszStart = pszExtensions;
			for(;;)
			{
				pszWhere = (unsigned char *) strstr( (const char *) pszStart, szTargetExtension );
				if( !pszWhere )
					break;
				pszTerminator = pszWhere + strlen( szTargetExtension );
				if( pszWhere == pszStart || *( pszWhere - 1 ) == ' ' )
					if( *pszTerminator == ' ' || *pszTerminator == '\0' )
						return true;
				pszStart = pszTerminator;
			}
			return false;
		}

		void PrintGLSLInfoLog(unsigned obj)
		{
			int infologLength = 0;
			int charsWritten  = 0;
			char *infoLog;

			glGetObjectParameterivARB(obj, GL_OBJECT_INFO_LOG_LENGTH_ARB, &infologLength);

			if (infologLength > 0)
			{
				infoLog = (char *)malloc(infologLength);
				glGetInfoLogARB(obj, infologLength, &charsWritten, infoLog);
				printf("%s\n",infoLog);
				free(infoLog);
			}
		}
    */
	}
}

#endif // _PXF_GRAPHICS_OPENGLUTILS_H_
