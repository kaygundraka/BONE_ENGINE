#pragma once
#include <MultiThreadSync.h>
#include "ISingleton.h"
#include "Common.h"

namespace BONE_GRAPHICS
{
	enum { SHADER_SHAODW_CREATE, SHADER_SHADOW_APPLY };

	typedef struct _RAY
	{
        Vector3	origin;
        Vector3	direction;
        _RAY() : origin(Vector3(0, 0, 0)), direction(Vector3(0, 0, 0)) {}
	} RAY;

	class RenderManager : public ISingleton <RenderManager>, public BONE_SYSTEM::MultiThreadSync<RenderManager>
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
		RenderManager() {}
		virtual ~RenderManager() {}

		bool InitializeMembers(HWND _hWnd);
		virtual void ReleaseMembers();

		IDirect3DDevice9* GetDevice();
		int GetHeight();
		int GetWidth();
		int GetMatrixPaletteSize();
        Vector3 GetScreenPos(const Vector3& pos, Matrix* view, Matrix* proj);

		void RenderText(string text, Vector2 pos, int length, const char* font, int opt, int setting, Color color);
		
        RAY	GetPickingRayToView(bool isMouseCenter);
		RAY	GetPickingRay(bool isMouseCenter);
		RAY	TransRayToView(RAY ray);
		
        bool CheckRayInMesh(RAY* ray, const Matrix& matWorld, Mesh mesh, float* dist = nullptr);
		bool CheckRayInTriangle(RAY* ray, const Matrix& matWorld, Vector3 p0, Vector3 p1, Vector3 p2, float* u, float* v, float* dist = nullptr);
		
        void SetupPixelFog(int color, int mode);
	};
}