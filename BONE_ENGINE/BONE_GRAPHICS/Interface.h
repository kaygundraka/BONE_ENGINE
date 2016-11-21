#pragma once
#include <Windows.h>
#include "RenderManager.h"

namespace BONE_GRAPHICS
{
	bool FastCreateWindow(HWND& _hWnd, HINSTANCE _hInstance, bool _showLog, int _windowWidth, int _windowHeight, const char* _windowName);
	LRESULT CALLBACK FrameworkWndProc(HWND _hWnd, UINT _iMessage, WPARAM _wParam, LPARAM _lParam);
	bool InitializeFramework(HWND _hWnd, bool _outputLog);
	bool ReleaseFramework();
	const char* CheckFrameworkVersion();
}
