#pragma once
#include <Windows.h>
#include "RenderManager.h"

namespace BONE_GRAPHICS
{
	bool FastCreateWindow(HWND& hWnd, HINSTANCE hInstance);
	LRESULT CALLBACK FrameworkWndProc(HWND hWnd, UINT message, WPARAM wParam, LPARAM lParam);
	bool InitializeFramework(HWND hWnd);
	bool ReleaseFramework();
	const char* CheckFrameworkVersion();
}
