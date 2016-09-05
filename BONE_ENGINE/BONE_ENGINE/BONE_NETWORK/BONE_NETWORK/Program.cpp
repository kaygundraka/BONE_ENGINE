//#include "Common.h"
//#include "CService.h"
//using namespace BONE_NETWORK;
//
//class CTest : public CService {
//public:
//	VOID Main(VOID)
//	{
//		CService::SetServiceName(_T("Test Service"));
//		CService::Begin(_T("Test Service"));
//		getchar();
//		CService::End();
//	}
//
//private:
//	virtual VOID OnStarted(VOID)
//	{
//		_tprintf(_T("OnStarted"));
//	}
//
//	virtual VOID OnStopped(VOID)
//	{
//		_tprintf(_T("OnStopped"));
//	}
//};
//
//int _tmain(int argc, _TCHAR* argv[])
//{
//	CTest Test;
//
//	cout << "command : ";
//	TCHAR Command[MAX_PATH];
//	_tscanf(_T("%s"), Command);
//
//	if (_tcscmp(Command, _T("-install")) == 0)
//		Test.Install(_T("Test Service"));
//	else if (_tcscmp(Command, _T("-uninstall")) == 0)
//		Test.Uninstall(_T("Test Service"));
//	else
//		Test.Main();
//
//	return 0;
//}