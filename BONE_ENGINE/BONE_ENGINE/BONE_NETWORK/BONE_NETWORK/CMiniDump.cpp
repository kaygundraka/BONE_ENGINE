#include "Common.h"
#include "CMiniDump.h"

namespace BONE_NETWORK
{
	LPTOP_LEVEL_EXCEPTION_FILTER CMiniDump::previousExceptionFilter;
	
	BOOL CMiniDump::Begin(VOID)
	{
		// �߸��� ���� â�� ��Ÿ���� �ʰ� �մϴ�.
		SetErrorMode(SEM_FAILCRITICALERRORS);

		// Unhandled Exception�� �߻����� �� ����� �ݹ��� �����մϴ�.
		previousExceptionFilter = SetUnhandledExceptionFilter(UnhandledExceptionFilter);
		
		return TRUE;
	}

	BOOL CMiniDump::End(VOID)
	{
		SetUnhandledExceptionFilter(previousExceptionFilter);

		return TRUE;
	}
}