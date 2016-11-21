#include "Common.h"
#include "Interface.h"
#include "RenderManager.h"
#include "LogManager.h"
#include "SceneManager.h"
#include "ResourceManager.h"
#include "InputManager.h"
#include "etuImage.h"

namespace BONE_GRAPHICS
{
	LRESULT CALLBACK FrameworkWndProc(HWND hWnd, UINT iMessage, WPARAM wParam, LPARAM lParam)
	{
		POINT MousePosition;
		RECT ClientRect;
		int iActive;

		switch (iMessage) {
		case WM_MOUSEWHEEL:
			if (InputMgr->IsFocusedWindow())
			{
				if ((SHORT)HIWORD(wParam) > 0)
					InputMgr->SetMouseWheelStatus(MOUSE_WHEEL_UP);
				else if (((SHORT)HIWORD(wParam) < 0))
					InputMgr->SetMouseWheelStatus(MOUSE_WHEEL_DOWN);
			}
			break;

		case WM_LBUTTONDOWN:
			if (InputMgr->IsFocusedWindow())
				InputMgr->SetMouseLBButtonStatus(MOUSE_LBDOWN);
			break;

		case WM_LBUTTONUP:
			if (InputMgr->IsFocusedWindow())
				InputMgr->SetMouseLBButtonStatus(MOUSE_LBUP);
			break;

		case WM_ACTIVATE:
			iActive = LOWORD(wParam);

			switch (iActive)
			{
			case WA_ACTIVE:
			case WA_CLICKACTIVE:
				InputMgr->SetFocusWindow(true);
				break;

			case WA_INACTIVE:
				MousePosition.y = RenderMgr->GetHeight() / 2;
				MousePosition.x = RenderMgr->GetWidth() / 2;
				InputMgr->SetMousePosition(MousePosition);
				InputMgr->SetFocusWindow(false);
				break;
			}
			break;

		case WM_RBUTTONDOWN:
			if (InputMgr->IsFocusedWindow())
				InputMgr->SetMouseRBButtonStatus(MOUSE_RBDOWN);
			break;

		case WM_RBUTTONUP:
			if (InputMgr->IsFocusedWindow())
				InputMgr->SetMouseRBButtonStatus(MOUSE_RBUP);
			break;

		case WM_CHAR:
			if (InputMgr->IsFocusedWindow())
				InputMgr->SetCurrentChar((char)wParam);
			break;

		case WM_KEYUP:
			if (InputMgr->IsFocusedWindow())
				InputMgr->KeyUp(wParam);
			break;

		case WM_MOUSEMOVE:
			if (InputMgr->IsFocusedWindow()) 
			{
				GetWindowRect(hWnd, &ClientRect);
				MousePosition.y = HIWORD(lParam);
				MousePosition.x = LOWORD(lParam);
				InputMgr->SetMousePosition(MousePosition);
			}
			break;

		case WM_DESTROY:
			::PostQuitMessage(0);
			break;

		default:
			return (DefWindowProc(hWnd, iMessage, wParam, lParam));
		}

		return 0;
	}


	bool FastCreateWindow(HWND& _hWnd, HINSTANCE _hInstance, bool _showLog, int _windowWidth, int _windowHeight, const char* _windowName)
	{
		/// 윈도우창 관련 설정
		WNDCLASS WndClass;

		WndClass.cbClsExtra = 0;
		WndClass.cbWndExtra = 0;
		WndClass.hbrBackground = (HBRUSH)GetStockObject(WHITE_BRUSH);
		WndClass.hCursor = LoadCursor(NULL, IDC_ARROW);
		WndClass.hIcon = LoadIcon(NULL, IDI_APPLICATION);
		WndClass.hInstance = _hInstance;
		WndClass.lpfnWndProc = (WNDPROC)FrameworkWndProc;
		WndClass.lpszClassName = _windowName;
		WndClass.lpszMenuName = NULL;
		WndClass.style = CS_HREDRAW | CS_VREDRAW;
		RegisterClass(&WndClass);

		_hWnd = CreateWindow(_windowName, _windowName, WS_OVERLAPPEDWINDOW, 0, 0, _windowWidth, _windowHeight,
			NULL, (HMENU)NULL, _hInstance, NULL);

		ShowWindow(_hWnd, SW_SHOW);

		/// 콘솔창 관련 설정
		if (_showLog)
		{
			AllocConsole();
			SetConsoleTitleA("[Debug]");

			int ConsoleWidth = 600;
			int ConsoleHeight = 200;

			HWND ConsolehWnd = GetConsoleWindow();
			SetWindowPos(ConsolehWnd, 0, _windowWidth, 0, ConsoleWidth, ConsoleHeight, 0);
		}

		if (_hWnd != NULL)
			return true;
		else
			return false;
	}

	bool InitializeFramework(HWND _hWnd, bool _outputLog)
	{
		LogMgr->InitializeMembers(_outputLog);
		RenderMgr->InitializeMembers(_hWnd);
		InputMgr->InitializeMembers();
		SceneMgr->InitializeMembers();
		ResourceMgr->InitializeMembers();

		RECT rect = { 0, 0, RenderMgr->GetWidth(), RenderMgr->GetHeight() };
		etuImage LogoImage;
		
		if (!LogoImage.SetInformaition("LogoImage", "Resource\\GUI\\Logo.png", D3DXVECTOR3(0, 0, 0), RenderMgr->GetWidth(), RenderMgr->GetHeight(), &rect, NULL))
			return false;
		
		int Alpha = 0;

		while (Alpha <= 255)
		{
			RenderMgr->GetDevice()->Clear(0, 0, D3DCLEAR_TARGET | D3DCLEAR_ZBUFFER, RGB(0, 0, 0), 1.0f, 0);
			RenderMgr->GetDevice()->BeginScene();

			LogoImage.Render(Alpha);
			Alpha += 1;

			RenderMgr->GetDevice()->EndScene();
			RenderMgr->GetDevice()->Present(0, 0, 0, 0);
		}

		Sleep(2000);

		while (Alpha >= 0)
		{
			RenderMgr->GetDevice()->Clear(0, 0, D3DCLEAR_TARGET | D3DCLEAR_ZBUFFER, RGB(0, 0, 0), 1.0f, 0);
			RenderMgr->GetDevice()->BeginScene();

			LogoImage.Render(Alpha);
			Alpha -= 1;

			RenderMgr->GetDevice()->EndScene();
			RenderMgr->GetDevice()->Present(0, 0, 0, 0);
		}

		return true;
	}

	bool ReleaseFramework()
	{
		SceneMgr->ReleaseMembers();
		InputMgr->ReleaseMembers();
		RenderMgr->ReleaseMembers();
		ResourceMgr->ReleaseMembers();
		LogMgr->ReleaseMembers();

		return true;
	}

	const char* CheckFrameworkVersion()
	{
		return "BONE FRAMEWORK VERSION : v1.00a";
	}
}