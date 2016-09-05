#pragma once
#include "Common.h"

namespace BONE_NETWORK
{
	class CMiniDump {
	private:
		static LPTOP_LEVEL_EXCEPTION_FILTER previousExceptionFilter;

	public:
		static BOOL Begin(VOID);
		static BOOL End(VOID);
	};
}