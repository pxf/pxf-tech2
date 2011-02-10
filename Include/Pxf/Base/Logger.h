#ifndef _PXF_BASE_LOGGER_H
#define _PXF_BASE_LOGGER_H

namespace Pxf
{
	class Logger
	{
	public:

		enum Flag
		{
			IS_DEBUG       = (1 << 31),
			IS_INFORMATION = (1 << 30),
			IS_WARNING     = (1 << 29),
			IS_CRITICAL    = (1 << 28),
			IS_REGISTERED  = (1 << 27),
			IS_ANY         = IS_DEBUG | IS_INFORMATION | IS_WARNING | IS_CRITICAL,
			IS_ANYREG      = IS_ANY | IS_REGISTERED
		};

		virtual bool WriteImpl(unsigned int _Tag, const char** _TagTable, unsigned int _TagTableSize, const char* _SrcBuffer, unsigned int _SrcLength) = 0;
		bool Write(unsigned int _Tag, const char** _TagTable, unsigned int _TagTableSize, const char* _Message);

		static bool IsDebug(unsigned int _Tag);
		static bool IsInformation(unsigned int _Tag);
		static bool IsWarning(unsigned int _Tag);
		static bool IsCritical(unsigned int _Tag);
		static bool IsRegistered(unsigned int _Tag);
	};

	class StdLogger : public Logger
	{
	public:
		virtual bool WriteImpl(unsigned int _Tag, const char** _TagTable, unsigned int _TagTableSize, const char* _SrcBuffer, unsigned int _SrcLength);
	};
}

#endif // _PXF_BASE_LOGGER_H