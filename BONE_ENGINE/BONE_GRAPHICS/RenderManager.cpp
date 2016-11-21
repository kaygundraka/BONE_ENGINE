#include "Common.h"
#include "RenderManager.h"
#include <mutex>
#include "LogManager.h"
#include "InputManager.h"
#pragma warning(disable : 4996)


namespace BONE_GRAPHICS
{
	//RenderManager* RenderManager::pInst = NULL;

	bool RenderManager::InitializeMembers(HWND _hWnd)
	{
		CThreadSync sync;

		this->hWnd = _hWnd;

		d3d9 = Direct3DCreate9(D3D_SDK_VERSION);
		d3d9->GetDeviceCaps(D3DADAPTER_DEFAULT, D3DDEVTYPE_HAL, &caps);

		int vp = 0;

		if (caps.DevCaps & D3DDEVCAPS_HWTRANSFORMANDLIGHT)
		{
			vp = D3DCREATE_HARDWARE_VERTEXPROCESSING | D3DCREATE_MULTITHREADED;
		}
		else
		{
			vp = D3DCREATE_SOFTWARE_VERTEXPROCESSING | D3DCREATE_MULTITHREADED;
		}

		RECT ClientRect;
		GetClientRect(hWnd, &ClientRect);

		width = ClientRect.right;
		height = ClientRect.bottom;

		d3dpp.BackBufferWidth = width;
		d3dpp.BackBufferHeight = height;
		d3dpp.BackBufferFormat = D3DFMT_A8R8G8B8;
		d3dpp.BackBufferCount = 1;
		d3dpp.MultiSampleType = D3DMULTISAMPLE_NONE;
		d3dpp.MultiSampleQuality = 0;
		d3dpp.SwapEffect = D3DSWAPEFFECT_DISCARD;
		d3dpp.hDeviceWindow = hWnd;
		d3dpp.Windowed = true;
		d3dpp.EnableAutoDepthStencil = true;
		d3dpp.AutoDepthStencilFormat = D3DFMT_D24S8;
		d3dpp.Flags = 0;
		d3dpp.FullScreen_RefreshRateInHz = D3DPRESENT_RATE_DEFAULT;
		d3dpp.PresentationInterval = D3DPRESENT_INTERVAL_IMMEDIATE;

		vp = d3d9->CreateDevice(
			D3DADAPTER_DEFAULT,
			D3DDEVTYPE_HAL,
			hWnd,
			vp,
			&d3dpp,
			&D3D_DEVICE);

		font = NULL;
		line = NULL;
		
		if (vp != D3D_OK)
		{
			::MessageBox(0, LPSTR("CreateDevice() - FAILED"), 0, 0);
			Is_init = false;
			return 0;
		}

		Is_init = true;
		LogMgr->ShowMessage(LOG_MESSAGE, "RenderManager is initialized");

		return Is_init;
	}

	void RenderManager::RenderText(string _text, D3DXVECTOR2 _position, int _size, const char* _font, DWORD _option, int _setting, D3DCOLOR _color)
	{
		CThreadSync sync;

		if (_size == 0)
			_size = 20;

		if (font != NULL)
			font->Release();

		font = NULL;

		D3DXCreateFont(D3D_DEVICE, _size, 0, _setting, 0, FALSE, DEFAULT_CHARSET,
			OUT_DEFAULT_PRECIS, ANTIALIASED_QUALITY, DEFAULT_PITCH | FF_DONTCARE, TEXT(_font), &font);

		static SIZE size;

		GetTextExtentPoint32(font->GetDC(), _text.c_str(), _text.length(), &size);

		RECT TextRect = {
			_position.x, _position.y,
			_position.x + size.cx, _position.y + size.cy
		};

		font->DrawText(NULL, _text.c_str(), -1, &TextRect, _option, _color);
	}

	Ray	RenderManager::GetPickingRayToView(bool _isMouseCenter)
	{
		CThreadSync sync;

		return TransRayToView(GetPickingRay(_isMouseCenter));
	}

	Ray	RenderManager::GetPickingRay(bool _isMouseCenter)
	{
		CThreadSync sync;

		Ray			ray;	// Ray의 값을 가지는 구조체 생성
		D3DXMATRIX	proj;	// 투영행렬 받아올 변수
		RECT		rt;		// 화면 크기 받아올 변수

		D3D_DEVICE->GetTransform(D3DTS_PROJECTION, &proj);	//투영행렬 가져오기
		GetClientRect(hWnd, &rt);		//화면크기 받아오기

		D3DXVECTOR3		vec;

		int MouseX;
		int MouseY;

		if (_isMouseCenter)
		{
			MouseX = GetWidth() / 2;
			MouseY = GetHeight() / 2;
		}
		else
		{
			MouseX = InputMgr->GetMousePosition().x;
			MouseY = InputMgr->GetMousePosition().y;
		}

		// 방향 벡터 구하기
		vec.x = (((2.0f * MouseX) / rt.right) - 1.0f) / proj._11;
		vec.y = -(((2.0f * MouseY) / rt.bottom) - 1.0f) / proj._22;
		vec.z = 1.0f;

		ray._origin = D3DXVECTOR3(0.0f, 0.0f, 0.0f);
		ray._direction = vec;

		return ray;
	}

	D3DXVECTOR3 RenderManager::GetScreenPos(const D3DXVECTOR3& _pos, D3DXMATRIX* _view, D3DXMATRIX* _proj)
	{
		D3DVIEWPORT9 vp;
		D3D_DEVICE->GetViewport(&vp);

		Vector3 Result = _pos;

		D3DXMATRIX ViewProjMatrix = (*_view) * (*_proj);

		D3DXVec3TransformCoord(&Result, &Result, _view);
		D3DXVec3TransformCoord(&Result, &Result, _proj);

		//Vector3 hcsPosition = _proj * (_view * _pos);

		/*Result.x = vp.Width * (Result.x + 1.0f) / 2.0f;
		Result.y = vp.Height * (1.0f - ((Result.y + 1.0f) / 2.0f));*/

		Result.x = (float)vp.Width * ((Result.x * 0.5f) + 0.5f);
		Result.y = (float)vp.Height * (1.0f - ((Result.y * 0.5f) + 0.5f));


		//x = ((hcsPosition.x * 0.5f) + 0.5f);          // 0 <= x <= 1 // left := 0,right := 1 
		//y = 1.0f - ((hcsPosition.y * 0.5f) + 0.5f); // 0 <= y <= 1 // bottom := 1,top := 0

		//x, y 값에 SCREEN_WIDTH, SCREEN_HEIGHT를 곱해주면 된다.

		return Result;
	}

	Ray	RenderManager::TransRayToView(Ray _ray)
	{
		CThreadSync sync;

		D3DXMATRIX view;		//뷰행렬 받아올 변수

		D3D_DEVICE->GetTransform(D3DTS_VIEW, &view);		//뷰행렬 가져오기

		D3DXMATRIX viewInverse;	//뷰역행렬 변수

		D3DXMatrixInverse(&viewInverse, NULL, &view);
		D3DXVec3TransformCoord(&_ray._origin, &_ray._origin, &viewInverse);
		D3DXVec3TransformNormal(&_ray._direction, &_ray._direction, &viewInverse);
		D3DXVec3Normalize(&_ray._direction, &_ray._direction);

		return _ray;
	}

	bool RenderManager::CheckRayInMesh(Ray* _ray, const D3DXMATRIX& _matWorld, LPD3DXMESH _mesh, float* _dist)
	{
		CThreadSync sync;

		D3DXVECTOR3 vOrg;
		D3DXVECTOR3 vDir;
		D3DXMATRIX Inversemat;

		D3DXMatrixInverse(&Inversemat, NULL, &_matWorld);
		D3DXVec3TransformCoord(&vOrg, &_ray->_origin, &Inversemat);
		D3DXVec3TransformNormal(&vDir, &_ray->_direction, &Inversemat);

		BOOL bHit = FALSE;
		//D3D 지원 메쉬 충돌 검사 함수
		D3DXIntersect(_mesh, &vOrg, &vDir, &bHit, NULL,
			NULL, NULL, _dist, NULL, NULL);

		return bHit;
	}

	bool RenderManager::CheckRayInTriangle(Ray* _ray, const D3DXMATRIX& _matWorld, D3DXVECTOR3 _p0, D3DXVECTOR3 _p1, D3DXVECTOR3 _p2, float* _u, float* _v, float* _dist)
	{
		CThreadSync sync;

		D3DXVECTOR3 vOrg;
		D3DXVECTOR3 vDir;
		D3DXMATRIX Inversemat;

		D3DXMatrixInverse(&Inversemat, NULL, &_matWorld);
		D3DXVec3TransformCoord(&vOrg, &_ray->_origin, &Inversemat);
		D3DXVec3TransformNormal(&vDir, &_ray->_direction, &Inversemat);

		BOOL bHit;
		//D3D 지원 메쉬 충돌 검사 함수
		bHit = D3DXIntersectTri(&_p0, &_p1, &_p2, &vOrg, &vDir, _u, _v, _dist);

		return bHit;
	}
	
	int RenderManager::GetMatrixPaletteSize()
	{
		CThreadSync sync;

		return caps.MaxVertexShaderConst / 4;
	}

	IDirect3DDevice9* RenderManager::GetDevice()
	{
		CThreadSync sync;

		if (Is_init)
			return D3D_DEVICE;
		else
		{
			::MessageBox(0, LPSTR("Not Exist Device - FAILED"), 0, 0);
			return NULL;
		}
	}

	void RenderManager::SetupPixelFog(DWORD _color, DWORD _mode)
	{
		CThreadSync sync;

		float Start = 0.5f;    // For linear mode
		float End = 0.8f;
		float Density = 0.66f;   // For exponential modes

								 // Enable fog blending.
		D3D_DEVICE->SetRenderState(D3DRS_FOGENABLE, TRUE);

		// Set the fog color.
		D3D_DEVICE->SetRenderState(D3DRS_FOGCOLOR, _color);

		// Set fog parameters.
		if (_mode == D3DFOG_LINEAR)
		{
			D3D_DEVICE->SetRenderState(D3DRS_FOGTABLEMODE, _mode);
			D3D_DEVICE->SetRenderState(D3DRS_FOGSTART, *(DWORD *)(&Start));
			D3D_DEVICE->SetRenderState(D3DRS_FOGEND, *(DWORD *)(&End));
		}
		else
		{
			D3D_DEVICE->SetRenderState(D3DRS_FOGTABLEMODE, _mode);
			D3D_DEVICE->SetRenderState(D3DRS_FOGDENSITY, *(DWORD *)(&Density));
		}
	}

	int RenderManager::GetHeight()
	{
		CThreadSync sync;

		RECT rt;
		GetClientRect(hWnd, &rt);
		height = rt.bottom;

		return height;
	}

	int RenderManager::GetWidth()
	{
		CThreadSync sync;

		RECT rt;
		GetClientRect(hWnd, &rt);
		width = rt.right;

		return width;
	}

	void RenderManager::ReleaseMembers()
	{
		CThreadSync sync;

		if (font)
			font->Release();

		if (line)
			line->Release();

		if (D3D_DEVICE)
			D3D_DEVICE->Release();

		if (d3d9)
			d3d9->Release();

		LogMgr->ShowMessage(LOG_MESSAGE, "Render Manager is Clear.");
	}
}