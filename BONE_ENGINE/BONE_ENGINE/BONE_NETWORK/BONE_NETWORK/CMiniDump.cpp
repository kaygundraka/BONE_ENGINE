#include "Common.h"
#include "CMiniDump.h"

namespace BONE_NETWORK
{
	LPTOP_LEVEL_EXCEPTION_FILTER CMiniDump::previousExceptionFilter;
	
	BOOL CMiniDump::Begin(VOID)
	{
		// 잘못된 연산 창이 나타나지 않게 합니다.
		SetErrorMode(SEM_FAILCRITICALERRORS);

		// Unhandled Exception이 발생했을 때 진행될 콜백을 정의합니다.
		previousExceptionFilter = SetUnhandledExceptionFilter(UnhandledExceptionFilter);
		
		return TRUE;
	}

	BOOL CMiniDump::End(VOID)
	{
		SetUnhandledExceptionFilter(previousExceptionFilter);

		return TRUE;
	}
}