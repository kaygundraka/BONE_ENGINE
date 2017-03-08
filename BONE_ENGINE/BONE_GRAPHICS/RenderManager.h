#pragma once
#include "ISingleton.h"
#include "Common.h"
#include <MultiThreadSync.h>

namespace BONE_GRAPHICS
{
    enum SHADER_ENUM { SHADER_SHAODW_CREATE, SHADER_SHADOW_APPLY };

    typedef struct _RAY
    {
        D3DXVECTOR3	origin;
        D3DXVECTOR3	direction;
        _RAY();
    } RAY;

    class RenderManager : public ISingleton <RenderManager>, public BONE_SYSTEM::MultiThreadSync <RenderManager>
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

        bool InitializeMembers(HWND hWnd);
        virtual void ReleaseMembers();

        IDirect3DDevice9* GetDevice();
        int GetHeight();
        int GetWidth();
        int GetMatrixPaletteSize();
        D3DXVECTOR3 GetScreenPos(const D3DXVECTOR3& pos, D3DXMATRIX* view, D3DXMATRIX* proj);

        void RenderText(std::string text, D3DXVECTOR2 pos, int length, const char* font, int opt, int setting, D3DXCOLOR color);

        RAY	GetPickingRayToView(bool isMouseCenter);
        RAY	GetPickingRay(bool isMouseCenter);
        RAY	TransRayToView(RAY ray);

        bool CheckRayInMesh(RAY* ray, const D3DXMATRIX& matWorld, LPD3DXMESH mesh, float* dist = nullptr);
        bool CheckRayInTriangle(RAY* ray, const D3DXMATRIX& matWorld, D3DXVECTOR3 p0, D3DXVECTOR3 p1, D3DXVECTOR3 p2, float* u, float* v, float* dist = nullptr);

        void SetupPixelFog(int color, int mode);
    };
}