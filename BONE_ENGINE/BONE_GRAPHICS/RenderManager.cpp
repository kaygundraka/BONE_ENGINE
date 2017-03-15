#include "Common.h"
#include "RenderManager.h"
#include "LogManager.h"
#include "InputManager.h"
#include "ResourceManager.h"
#include "Vertex.h"

namespace BONE_GRAPHICS
{
    _RAY::_RAY() : origin(D3DXVECTOR3(0, 0, 0)), direction(D3DXVECTOR3(0, 0, 0)) {}

    bool RenderManager::InitializeMembers(HWND hWnd, bool useImgui)
    {
        ThreadSync sync;

        this->hWnd = hWnd;

        d3d9 = Direct3DCreate9(D3D_SDK_VERSION);
        d3d9->GetDeviceCaps(D3DADAPTER_DEFAULT, D3DDEVTYPE_HAL, &caps);

        int vp = 0;

        if (caps.DevCaps & D3DDEVCAPS_HWTRANSFORMANDLIGHT)
            vp = D3DCREATE_HARDWARE_VERTEXPROCESSING | D3DCREATE_MULTITHREADED;
        else
            vp = D3DCREATE_SOFTWARE_VERTEXPROCESSING | D3DCREATE_MULTITHREADED;

        RECT ClientRect;
        GetClientRect(this->hWnd, &ClientRect);

        width = ClientRect.right;
        height = ClientRect.bottom;

        d3dpp.BackBufferWidth = width;
        d3dpp.BackBufferHeight = height;
        d3dpp.BackBufferFormat = D3DFMT_A8R8G8B8;
        d3dpp.BackBufferCount = 1;
        d3dpp.MultiSampleType = D3DMULTISAMPLE_NONE;
        d3dpp.MultiSampleQuality = 0;
        d3dpp.SwapEffect = D3DSWAPEFFECT_DISCARD;
        d3dpp.hDeviceWindow = this->hWnd;
        d3dpp.Windowed = true;
        d3dpp.EnableAutoDepthStencil = true;
        d3dpp.AutoDepthStencilFormat = D3DFMT_D24S8;
        d3dpp.Flags = 0;
        d3dpp.FullScreen_RefreshRateInHz = D3DPRESENT_RATE_DEFAULT;
        d3dpp.PresentationInterval = D3DPRESENT_INTERVAL_IMMEDIATE;

        vp = d3d9->CreateDevice(
            D3DADAPTER_DEFAULT,
            D3DDEVTYPE_HAL,
            this->hWnd,
            vp,
            &d3dpp,
            &D3D_DEVICE);

        font = nullptr;
        line = nullptr;

        if (vp != D3D_OK)
        {
            LogMgr->ShowMessage(LOG_ERROR, "CreateDevice() - FAILED");
            Is_init = false;
            return 0;
        }

        Is_init = true;
        LogMgr->ShowMessage(LOG_MESSAGE, "RenderManager is initialized");

        this->useImGUI = useImgui;

        return Is_init;
    }

    void RenderManager::RenderText(std::string text, D3DXVECTOR2 pos, int length, const char* font, int opt, int setting, D3DXCOLOR color)
    {
        ThreadSync sync;

        if (length == 0)
            length = 20;

        if (this->font != nullptr)
            this->font->Release();

        this->font = nullptr;

        D3DXCreateFont(D3D_DEVICE, length, 0, setting, 0, FALSE, DEFAULT_CHARSET,
            OUT_DEFAULT_PRECIS, ANTIALIASED_QUALITY, DEFAULT_PITCH | FF_DONTCARE, TEXT(font), &this->font);

        static SIZE size;

        GetTextExtentPoint32(this->font->GetDC(), text.c_str(), text.length(), &size);

        RECT TextRect = {
            pos.x, pos.y,
            pos.x + size.cx, pos.y + size.cy
        };

        this->font->DrawText(nullptr, text.c_str(), -1, &TextRect, opt, color);
    }

    RAY	RenderManager::GetPickingRayToView(bool isMouseCenter)
    {
        ThreadSync sync;

        return TransRayToView(GetPickingRay(isMouseCenter));
    }

    RAY	RenderManager::GetPickingRay(bool _isMouseCenter)
    {
        ThreadSync sync;

        RAY			ray;
        D3DXMATRIX	proj;
        RECT		rt;

        D3D_DEVICE->GetTransform(D3DTS_PROJECTION, &proj);
        GetClientRect(hWnd, &rt);

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

        vec.x = (((2.0f * MouseX) / rt.right) - 1.0f) / proj._11;
        vec.y = -(((2.0f * MouseY) / rt.bottom) - 1.0f) / proj._22;
        vec.z = 1.0f;

        ray.origin = D3DXVECTOR3(0.0f, 0.0f, 0.0f);
        ray.direction = vec;

        return ray;
    }

    D3DXVECTOR3 RenderManager::GetScreenPos(const D3DXVECTOR3& pos, D3DXMATRIX* view, D3DXMATRIX* proj)
    {
        D3DVIEWPORT9 vp;
        D3D_DEVICE->GetViewport(&vp);

        D3DXVECTOR3 Result = pos;

        D3DXMATRIX ViewProjMatrix = (*view) * (*proj);

        D3DXVec3TransformCoord(&Result, &Result, view);
        D3DXVec3TransformCoord(&Result, &Result, proj);

        //D3DXVECTOR3 hcsPosition = _proj * (_view * _pos);

        /*Result.x = vp.Width * (Result.x + 1.0f) / 2.0f;
        Result.y = vp.Height * (1.0f - ((Result.y + 1.0f) / 2.0f));*/

        Result.x = (float)vp.Width * ((Result.x * 0.5f) + 0.5f);
        Result.y = (float)vp.Height * (1.0f - ((Result.y * 0.5f) + 0.5f));

        //x = ((hcsPosition.x * 0.5f) + 0.5f);          // 0 <= x <= 1 // left := 0,right := 1 
        //y = 1.0f - ((hcsPosition.y * 0.5f) + 0.5f); // 0 <= y <= 1 // bottom := 1,top := 0

        //x, y 값에 SCREEN_WIDTH, SCREEN_HEIGHT를 곱해주면 된다.

        return Result;
    }

    RAY	RenderManager::TransRayToView(RAY ray)
    {
        ThreadSync sync;

        D3DXMATRIX view;		//뷰행렬 받아올 변수

        D3D_DEVICE->GetTransform(D3DTS_VIEW, &view);		//뷰행렬 가져오기

        D3DXMATRIX viewInverse;	//뷰역행렬 변수

        D3DXMatrixInverse(&viewInverse, nullptr, &view);
        D3DXVec3TransformCoord(&ray.origin, &ray.origin, &viewInverse);
        D3DXVec3TransformNormal(&ray.direction, &ray.direction, &viewInverse);
        D3DXVec3Normalize(&ray.direction, &ray.direction);

        return ray;
    }

    bool RenderManager::CheckRayInMesh(RAY* ray, const D3DXMATRIX& matWorld, LPD3DXMESH mesh, float* dist)
    {
        ThreadSync sync;

        D3DXVECTOR3 vOrg;
        D3DXVECTOR3 vDir;
        D3DXMATRIX Inversemat;

        D3DXMatrixInverse(&Inversemat, nullptr, &matWorld);
        D3DXVec3TransformCoord(&vOrg, &ray->origin, &Inversemat);
        D3DXVec3TransformNormal(&vDir, &ray->direction, &Inversemat);

        BOOL bHit = FALSE;
        D3DXIntersect(mesh, &vOrg, &vDir, &bHit, nullptr, nullptr, nullptr, dist, nullptr, nullptr);

        return bHit;
    }

    struct line_vertex
    {
        float x, y, z;
        DWORD color;
    };

    void RenderManager::DrawLine(D3DXVECTOR3 start, D3DXVECTOR3 end, D3DXCOLOR color)
    {
        Matrix Transform;
        D3DXMatrixIdentity(&Transform);
        D3D_DEVICE->SetTransform(D3DTS_WORLD, &Transform);

        line_vertex line_vertices[2];

        const DWORD line_fvf = D3DFVF_XYZ | D3DFVF_DIFFUSE;

        line_vertices[0].x = start.x;
        line_vertices[0].y = start.y;
        line_vertices[0].z = start.z;
        line_vertices[0].color = color;

        line_vertices[1].x = end.x;
        line_vertices[1].y = end.y;
        line_vertices[1].z = end.z;
        line_vertices[1].color = color;

        D3D_DEVICE->SetFVF(line_fvf);
        D3D_DEVICE->DrawPrimitiveUP(D3DPT_LINELIST, 1, line_vertices, sizeof(line_vertex));
    }

    bool RenderManager::DrawMeshBox(LPD3DXMESH mesh, D3DXVECTOR3 pos, D3DXCOLOR color)
    {
        D3DXVECTOR3 LeftBottom;
        D3DXVECTOR3 RightTop;

        LPVOID pVertices(nullptr);
        mesh->LockVertexBuffer(D3DLOCK_NOSYSLOCK, &pVertices);

        if (FAILED(D3DXComputeBoundingBox((Vector3*)pVertices,
            mesh->GetNumVertices(),
            D3DXGetFVFVertexSize(mesh->GetFVF()),
            &LeftBottom, &RightTop)))
        {
            LogMgr->ShowMessage(LOG_ERROR, "Compute BoundingBox Failed..");
            mesh->UnlockVertexBuffer();

            return false;
        }

        mesh->UnlockVertexBuffer();

        D3DXVECTOR3 Vertex[8];

        Vertex[0].x = LeftBottom.x;
        Vertex[0].y = LeftBottom.y;
        Vertex[0].z = LeftBottom.z;

        Vertex[1].x = RightTop.x;
        Vertex[1].y = LeftBottom.y;
        Vertex[1].z = LeftBottom.z;

        Vertex[2].x = LeftBottom.x;
        Vertex[2].y = RightTop.y;
        Vertex[2].z = LeftBottom.z;

        Vertex[3].x = LeftBottom.x;
        Vertex[3].y = LeftBottom.y;
        Vertex[3].z = RightTop.z;

        Vertex[4].x = RightTop.x;
        Vertex[4].y = RightTop.y;
        Vertex[4].z = LeftBottom.z;

        Vertex[5].x = RightTop.x;
        Vertex[5].y = LeftBottom.y;
        Vertex[5].z = RightTop.z;

        Vertex[6].x = LeftBottom.x;
        Vertex[6].y = RightTop.y;
        Vertex[6].z = RightTop.z;

        Vertex[7].x = RightTop.x;
        Vertex[7].y = RightTop.y;
        Vertex[7].z = RightTop.z;

        for (int i = 0; i < 8; i++)
            Vertex[i] += pos;

        this->DrawLine(Vertex[0], Vertex[1], color);
        this->DrawLine(Vertex[0], Vertex[2], color);
        this->DrawLine(Vertex[0], Vertex[3], color);

        this->DrawLine(Vertex[7], Vertex[4], color);
        this->DrawLine(Vertex[7], Vertex[5], color);
        this->DrawLine(Vertex[7], Vertex[6], color);

        this->DrawLine(Vertex[2], Vertex[4], color);
        this->DrawLine(Vertex[2], Vertex[6], color);

        this->DrawLine(Vertex[5], Vertex[1], color);
        this->DrawLine(Vertex[5], Vertex[3], color);

        return true;
    }

    bool RenderManager::CheckRayInTriangle(RAY* ray, const D3DXMATRIX& matWorld, D3DXVECTOR3 p0, D3DXVECTOR3 p1, D3DXVECTOR3 p2, float* u, float* v, float* dist)
    {
        ThreadSync sync;

        D3DXVECTOR3 vOrg;
        D3DXVECTOR3 vDir;
        D3DXMATRIX Inversemat;

        D3DXMatrixInverse(&Inversemat, nullptr, &matWorld);
        D3DXVec3TransformCoord(&vOrg, &ray->origin, &Inversemat);
        D3DXVec3TransformNormal(&vDir, &ray->direction, &Inversemat);

        bool bHit;
        bHit = D3DXIntersectTri(&p0, &p1, &p2, &vOrg, &vDir, u, v, dist);

        return bHit;
    }

    int RenderManager::GetMatrixPaletteSize()
    {
        ThreadSync sync;

        return caps.MaxVertexShaderConst / 4;
    }

    IDirect3DDevice9* RenderManager::GetDevice()
    {
        ThreadSync sync;

        if (Is_init)
            return D3D_DEVICE;
        else
        {
            LogMgr->ShowMessage(LOG_ERROR, "Not Exist Device - FAILED");
            return nullptr;
        }
    }

    void RenderManager::SetupPixelFog(int color, int mode)
    {
        ThreadSync sync;

        float Start = 0.5f;
        float End = 0.8f;
        float Density = 0.66f;

        D3D_DEVICE->SetRenderState(D3DRS_FOGENABLE, TRUE);
        D3D_DEVICE->SetRenderState(D3DRS_FOGCOLOR, color);

        if (mode == D3DFOG_LINEAR)
        {
            D3D_DEVICE->SetRenderState(D3DRS_FOGTABLEMODE, mode);
            D3D_DEVICE->SetRenderState(D3DRS_FOGSTART, *(int *)(&Start));
            D3D_DEVICE->SetRenderState(D3DRS_FOGEND, *(int *)(&End));
        }
        else
        {
            D3D_DEVICE->SetRenderState(D3DRS_FOGTABLEMODE, mode);
            D3D_DEVICE->SetRenderState(D3DRS_FOGDENSITY, *(int *)(&Density));
        }
    }

    int RenderManager::GetHeight()
    {
        ThreadSync sync;

        RECT rt;
        GetClientRect(hWnd, &rt);
        height = rt.bottom;

        return height;
    }

    int RenderManager::GetWidth()
    {
        ThreadSync sync;

        RECT rt;
        GetClientRect(hWnd, &rt);
        width = rt.right;

        return width;
    }

    void RenderManager::ReleaseMembers()
    {
        ThreadSync sync;

        if (font) font->Release();

        if (line) line->Release();

        if (D3D_DEVICE) D3D_DEVICE->Release();

        if (d3d9) d3d9->Release();

        LogMgr->ShowMessage(LOG_MESSAGE, "Render Manager is Clear.");
    }

    bool RenderManager::UseImGUI()
    {
        ThreadSync sync;

        return useImGUI;
    }
}