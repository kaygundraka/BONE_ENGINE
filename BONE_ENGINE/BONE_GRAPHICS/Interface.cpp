#include "Common.h"
#include "Interface.h"
#include "RenderManager.h"
#include "LogManager.h"
#include "SceneManager.h"
#include "ResourceManager.h"
#include "InputManager.h"
#include "ConfigManager.h"
#include "RuntimeCompiler.h"

extern LRESULT ImGui_ImplDX9_WndProcHandler(HWND hWnd, UINT msg, WPARAM wParam, LPARAM lParam);

namespace BONE_GRAPHICS
{
	LRESULT CALLBACK FrameworkWndProc(HWND hWnd, UINT message, WPARAM wParam, LPARAM lParam)
	{
        ImGui_ImplDX9_WndProcHandler(hWnd, message, wParam, lParam);
        //    return true;

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
            ReleaseFramework();
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

        std::string windowsName = ConfigMgr->GetStr(".\\info", "WindowsName");

		WndClass.cbClsExtra = 0;
		WndClass.cbWndExtra = 0;
		WndClass.hbrBackground = (HBRUSH)GetStockObject(WHITE_BRUSH);
		WndClass.hCursor = LoadCursor(NULL, IDC_ARROW);
		WndClass.hIcon = LoadIcon(NULL, IDI_APPLICATION);
		WndClass.hInstance = hInstance;
		WndClass.lpfnWndProc = (WNDPROC)FrameworkWndProc;
		WndClass.lpszClassName = windowsName.c_str();
		WndClass.lpszMenuName = NULL;
		WndClass.style = CS_HREDRAW | CS_VREDRAW;
		RegisterClass(&WndClass);

		hWnd = CreateWindow(
            windowsName.c_str(),
            windowsName.c_str(),
            WS_OVERLAPPEDWINDOW, 
            0, 
            0, 
            ConfigMgr->GetFloat(".\\info", "WindowsWidth"),
            ConfigMgr->GetFloat(".\\info", "WindowsHeight"),
			NULL, 
            (HMENU)NULL, 
            hInstance, 
            NULL
        );

		ShowWindow(hWnd, SW_SHOW);

		if (ConfigMgr->GetBool(".\\info", "ShowLogCmd"))
		{
			AllocConsole();
			SetConsoleTitleA("[Debug]");

			int ConsoleWidth = 600;
			int ConsoleHeight = 200;

			HWND ConsolehWnd = GetConsoleWindow();
			SetWindowPos(ConsolehWnd, 
                0, 
                ConfigMgr->GetFloat(".\\info", "WindowsWidth"), 
                0, 
                ConfigMgr->GetFloat(".\\info", "WindowsWidth"),
                ConfigMgr->GetFloat(".\\info", "WindowsHeight"), 
                0
            );
		}

		if (hWnd != NULL)
			return true;
		else
			return false;
	}

	bool InitializeFramework(HWND hWnd, bool useImGUI)
	{
		LogMgr->InitializeMembers(ConfigMgr->GetFloat(".\\info", "EnableLog"));
		RenderMgr->InitializeMembers(hWnd, useImGUI);
		InputMgr->InitializeMembers();
		SceneMgr->InitializeMembers();
		ResourceMgr->InitializeMembers();
        RuntimeMgr->InitializeMembers();

        if (useImGUI)
            ImGui_ImplDX9_Init(hWnd, RenderMgr->GetDevice());
        
		RECT rect = { 0, 0, 1600, 1200 };
		
        LPDIRECT3DTEXTURE9 texture = ResourceMgr->LoadTexture("Logo.png");
        LPD3DXSPRITE sprite;
        D3DXCreateSprite(RenderMgr->GetDevice(), &sprite);

        float WidthScale = (float)RenderMgr->GetWidth() / 1600;
        float HeightScale = (float)RenderMgr->GetHeight() / 1200;

        float HeightMove = ((WidthScale * 1200) - (HeightScale * 1200));

        if (HeightMove > 0)
            HeightMove /= 2;
        else
            HeightMove = 0;

        Matrix matrix;
        D3DXMatrixIdentity(&matrix);
        D3DXMatrixTransformation2D(&matrix, 0, 0, &Vec2(WidthScale, WidthScale), 0, 0, &Vec2(0, -HeightMove));
        sprite->SetTransform(&matrix);

		int Alpha = 0;

		while (Alpha < 255)
		{
			RenderMgr->GetDevice()->Clear(0, 0, D3DCLEAR_TARGET | D3DCLEAR_ZBUFFER, RGB(0, 0, 0), 1.0f, 0);
			RenderMgr->GetDevice()->BeginScene();

            RenderMgr->GetDevice()->SetRenderState(D3DRS_ALPHABLENDENABLE, TRUE);
            RenderMgr->GetDevice()->SetRenderState(D3DRS_ALPHAFUNC, D3DCMP_GREATER);

            // alpha - normal 
            RenderMgr->GetDevice()->SetRenderState(D3DRS_BLENDOP, D3DBLENDOP_ADD);
            RenderMgr->GetDevice()->SetRenderState(D3DRS_SRCBLEND, D3DBLEND_SRCALPHA);
            RenderMgr->GetDevice()->SetRenderState(D3DRS_DESTBLEND, D3DBLEND_INVSRCALPHA);

            sprite->Begin(D3DXSPRITE_ALPHABLEND | D3DXSPRITE_SORT_TEXTURE);
            sprite->Begin(D3DXSPRITE_ALPHABLEND | D3DXSPRITE_OBJECTSPACE);

            D3DCOLOR RGB = D3DCOLOR_ARGB((int)Alpha, 255, 255, 255);
            sprite->Draw(texture, &rect, nullptr, &Vec3(0, 0, 0), RGB);

            sprite->End();
			Alpha += 1;

            RenderMgr->GetDevice()->SetRenderState(D3DRS_ALPHABLENDENABLE, FALSE);

			RenderMgr->GetDevice()->EndScene();
			RenderMgr->GetDevice()->Present(0, 0, 0, 0);
		}

		Sleep(2000);

		while (Alpha >= 0)
		{
			RenderMgr->GetDevice()->Clear(0, 0, D3DCLEAR_TARGET | D3DCLEAR_ZBUFFER, RGB(0, 0, 0), 1.0f, 0);
			RenderMgr->GetDevice()->BeginScene();

            sprite->Begin(D3DXSPRITE_ALPHABLEND | D3DXSPRITE_SORT_TEXTURE);
            sprite->Begin(D3DXSPRITE_ALPHABLEND | D3DXSPRITE_OBJECTSPACE);

            RenderMgr->GetDevice()->SetRenderState(D3DRS_ALPHABLENDENABLE, TRUE);
            RenderMgr->GetDevice()->SetRenderState(D3DRS_ALPHAFUNC, D3DCMP_GREATER);

            // alpha - normal 
            RenderMgr->GetDevice()->SetRenderState(D3DRS_BLENDOP, D3DBLENDOP_ADD);
            RenderMgr->GetDevice()->SetRenderState(D3DRS_SRCBLEND, D3DBLEND_SRCALPHA);
            RenderMgr->GetDevice()->SetRenderState(D3DRS_DESTBLEND, D3DBLEND_INVSRCALPHA);

            sprite->Begin(D3DXSPRITE_ALPHABLEND | D3DXSPRITE_SORT_TEXTURE);
            sprite->Begin(D3DXSPRITE_ALPHABLEND | D3DXSPRITE_OBJECTSPACE);

            D3DCOLOR RGB = D3DCOLOR_ARGB((int)Alpha, 255, 255, 255);
            sprite->Draw(texture, &rect, nullptr, &Vec3(0, 0, 0), RGB);

            sprite->End();
            Alpha -= 1;

            RenderMgr->GetDevice()->SetRenderState(D3DRS_ALPHABLENDENABLE, FALSE);

			RenderMgr->GetDevice()->EndScene();
			RenderMgr->GetDevice()->Present(0, 0, 0, 0);
        }

		return true;
	}

	bool ReleaseFramework()
	{
        if (RenderMgr->UseImGUI())
            ImGui_ImplDX9_Shutdown();

        RuntimeMgr->ReleaseMembers();
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