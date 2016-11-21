#pragma once
#include <MultiThreadSync.h>
#include "ISingleton.h"
#include "Common.h"
using namespace std;

namespace BONE_GRAPHICS
{
	enum { SHADER_SHAODW_CREATE, SHADER_SHADOW_APPLY };

	typedef struct _Ray
	{
		D3DXVECTOR3	_origin;
		D3DXVECTOR3	_direction;
		_Ray() : _origin(D3DXVECTOR3(0, 0, 0)), _direction(D3DXVECTOR3(0, 0, 0)) {}
	} Ray;

	class RenderManager : public ISingleton <RenderManager>, public BONE_SYSTEM::CMultiThreadSync<RenderManager>
	{
	private:
		HWND					hWnd;
		IDirect3D9*				d3d9;
		D3DCAPS9				caps;
		D3DPRESENT_PARAMETERS	d3dpp;
		IDirect3DDevice9*		D3D_DEVICE;
		int						width, height;
		ID3DXLine*				line;
		ID3DXFont*				font;

	public:
		// 생성자, 소멸자
		RenderManager() {}
		virtual ~RenderManager() {}
		bool InitializeMembers(HWND _hWnd);
		virtual void ReleaseMembers();

		// Get, Set 함수
		IDirect3DDevice9* GetDevice();
		int GetHeight();
		int GetWidth();
		int GetMatrixPaletteSize();
		D3DXVECTOR3 GetScreenPos(const D3DXVECTOR3& _pos, D3DXMATRIX* _view, D3DXMATRIX* _proj);

		// 랜더 및 연산
		void RenderText(string _text, D3DXVECTOR2 _position, int _size, const char* _font, DWORD _option, int _setting, D3DCOLOR _color);
		Ray	GetPickingRayToView(bool _isMouseCenter);
		Ray	GetPickingRay(bool _isMouseCenter);
		Ray	TransRayToView(Ray _ray);
		bool CheckRayInMesh(Ray* _ray, const D3DXMATRIX& _matWorld, LPD3DXMESH _mesh, float* _dist = NULL);
		bool CheckRayInTriangle(Ray* _ray, const D3DXMATRIX& _matWorld, D3DXVECTOR3 _p0, D3DXVECTOR3 _p1, D3DXVECTOR3 _p2, float* _u, float* _v, float* _dist = NULL);
		void SetupPixelFog(DWORD _color, DWORD _mode);
	};
}