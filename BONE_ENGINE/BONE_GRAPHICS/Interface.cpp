#include "Common.h"
#include "Interface.h"
#include "RenderManager.h"
#include "LogManager.h"
#include "SceneManager.h"
#include "ResourceManager.h"
#include "InputManager.h"
#include "ConfigManager.h"
#include "etuImage.h"

namespace BONE_GRAPHICS
{
	LRESULT CALLBACK FrameworkWndProc(HWND hWnd, UINT message, WPARAM wParam, LPARAM lParam)
	{
		POINT MousePosition;
		RECT ClientRect;
		int Active;

		switch (message) {
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
            Active = LOWORD(wParam);

			switch (Active)
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
			return (DefWindowProc(hWnd, message, wParam, lParam));
		}

		return 0;
	}


	bool FastCreateWindow(HWND& hWnd, HINSTANCE hInstance)
	{
		WNDCLASS WndClass;

		WndClass.cbClsExtra = 0;
		WndClass.cbWndExtra = 0;
		WndClass.hbrBackground = (HBRUSH)GetStockObject(WHITE_BRUSH);
		WndClass.hCursor = LoadCursor(NULL, IDC_ARROW);
		WndClass.hIcon = LoadIcon(NULL, IDI_APPLICATION);
		WndClass.hInstance = hInstance;
		WndClass.lpfnWndProc = (WNDPROC)FrameworkWndProc;
		WndClass.lpszClassName = ConfigMgr->GetStr("Info", "WindowsName").c_str();
		WndClass.lpszMenuName = NULL;
		WndClass.style = CS_HREDRAW | CS_VREDRAW;
		RegisterClass(&WndClass);

		hWnd = CreateWindow(
            ConfigMgr->GetStr("Info", "WindowsName").c_str(),
            ConfigMgr->GetStr("Info", "WindowsName").c_str(), 
            WS_OVERLAPPEDWINDOW, 
            0, 
            0, 
            ConfigMgr->GetFloat("Info", "WindowsWidth"),
            ConfigMgr->GetFloat("Info", "WindowsHeight"),
			NULL, 
            (HMENU)NULL, 
            hInstance, 
            NULL
        );

		ShowWindow(hWnd, SW_SHOW);

		if (ConfigMgr->GetBool("Info", "ShowLogCmd"))
		{
			AllocConsole();
			SetConsoleTitleA("[Debug]");

			int ConsoleWidth = 600;
			int ConsoleHeight = 200;

			HWND ConsolehWnd = GetConsoleWindow();
			SetWindowPos(ConsolehWnd, 
                0, 
                ConfigMgr->GetFloat("Info", "WindowsWidth"), 
                0, 
                ConfigMgr->GetFloat("Info", "WindowsWidth"),
                ConfigMgr->GetFloat("Info", "WindowsHeight"), 
                0
            );
		}

		if (hWnd != NULL)
			return true;
		else
			return false;
	}

	bool InitializeFramework(HWND hWnd)
	{
		LogMgr->InitializeMembers(ConfigMgr->GetFloat("Info", "EnableLog"));
		RenderMgr->InitializeMembers(hWnd);
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