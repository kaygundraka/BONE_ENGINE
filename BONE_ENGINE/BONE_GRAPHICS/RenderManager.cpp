#include "Common.h"
#include "RenderManager.h"
#include "LogManager.h"
#include "InputManager.h"
#include "ResourceManager.h"
#include "ConfigManager.h"
#include "Vertex.h"

namespace BONE_GRAPHICS
{
    _RAY::_RAY() : origin(D3DXVECTOR3(0, 0, 0)), direction(D3DXVECTOR3(0, 0, 0)) {}

    bool RenderManager::InitializeMembers(HWND hWnd, bool useImgui)
    {
        ThreadSync sync;

        lastRenderedMesh = "";

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
        d3dpp.BackBufferFormat = D3DFMT_A8R8G8B8;//D3DFMT_UNKNOWN;// D3DFMT_A8R8G8B8;
        d3dpp.BackBufferCount = 1;
        d3dpp.MultiSampleType = D3DMULTISAMPLE_NONE;
        d3dpp.MultiSampleQuality = 0;
        d3dpp.SwapEffect = D3DSWAPEFFECT_DISCARD;
        d3dpp.hDeviceWindow = this->hWnd;
        d3dpp.Windowed = ConfigMgr->GetBool(".\\info", "Windowed");
        d3dpp.EnableAutoDepthStencil = true;
        d3dpp.AutoDepthStencilFormat = D3DFMT_D16;//D3DFMT_D24S8;
        d3dpp.Flags = 0;
        d3dpp.FullScreen_RefreshRateInHz = D3DPRESENT_RATE_DEFAULT;
        d3dpp.PresentationInterval = D3DPRESENT_INTERVAL_IMMEDIATE;

        DWORD g_msaaSamples;

        //ChooseBestMSAAMode(d3dpp.BackBufferFormat, d3dpp.AutoDepthStencilFormat,
        //    d3dpp.Windowed, d3dpp.MultiSampleType, d3dpp.MultiSampleQuality,
        //    g_msaaSamples);
               
        vp = d3d9->CreateDevice(
            D3DADAPTER_DEFAULT,
            D3DDEVTYPE_HAL,
            this->hWnd,
            vp,
            &d3dpp,
            &D3D_DEVICE
        );

        font = nullptr;
        line = nullptr;

        if (vp != D3D_OK)
        {
            LogMgr->Error("CreateDevice() - FAILED");
            Is_init = false;
            return 0;
        }

        if (SUCCEEDED(D3D_DEVICE->GetDeviceCaps(&caps)))
        {
            // Prefer anisotropic texture filtering if it's supported.
            if (caps.RasterCaps & D3DPRASTERCAPS_ANISOTROPY)
                caps.MaxAnisotropy;
        }

        Is_init = true;

        LogMgr->Info("RenderManager is initialized");

        this->useImGUI = useImgui;

        return Is_init;
    }

    void RenderManager::ChooseBestMSAAMode(D3DFORMAT backBufferFmt, D3DFORMAT depthStencilFmt,
        BOOL windowed, D3DMULTISAMPLE_TYPE &type,
        DWORD &qualityLevels, DWORD &samplesPerPixel)
    {
        bool supported = false;

        struct MSAAMode
        {
            D3DMULTISAMPLE_TYPE type;
            DWORD samples;
        }
        multsamplingTypes[15] =
        {
            { D3DMULTISAMPLE_16_SAMPLES,  16 },
            { D3DMULTISAMPLE_15_SAMPLES,  15 },
            { D3DMULTISAMPLE_14_SAMPLES,  14 },
            { D3DMULTISAMPLE_13_SAMPLES,  13 },
            { D3DMULTISAMPLE_12_SAMPLES,  12 },
            { D3DMULTISAMPLE_11_SAMPLES,  11 },
            { D3DMULTISAMPLE_10_SAMPLES,  10 },
            { D3DMULTISAMPLE_9_SAMPLES,   9 },
            { D3DMULTISAMPLE_8_SAMPLES,   8 },
            { D3DMULTISAMPLE_7_SAMPLES,   7 },
            { D3DMULTISAMPLE_6_SAMPLES,   6 },
            { D3DMULTISAMPLE_5_SAMPLES,   5 },
            { D3DMULTISAMPLE_4_SAMPLES,   4 },
            { D3DMULTISAMPLE_3_SAMPLES,   3 },
            { D3DMULTISAMPLE_2_SAMPLES,   2 }
        };

        for (int i = 0; i < 15; ++i)
        {
            type = multsamplingTypes[i].type;

            supported = MSAAModeSupported(type, backBufferFmt, depthStencilFmt,
                windowed, qualityLevels);

            if (supported)
            {
                samplesPerPixel = multsamplingTypes[i].samples;
                return;
            }
        }

        type = D3DMULTISAMPLE_NONE;
        qualityLevels = 0;
        samplesPerPixel = 1;
    }

    bool RenderManager::MSAAModeSupported(D3DMULTISAMPLE_TYPE type, D3DFORMAT backBufferFmt,
        D3DFORMAT depthStencilFmt, BOOL windowed,
        DWORD &qualityLevels)
    {
        DWORD backBufferQualityLevels = 0;
        DWORD depthStencilQualityLevels = 0;

        HRESULT hr = d3d9->CheckDeviceMultiSampleType(D3DADAPTER_DEFAULT,
            D3DDEVTYPE_HAL, backBufferFmt, windowed, type,
            &backBufferQualityLevels);

        if (SUCCEEDED(hr))
        {
            hr = d3d9->CheckDeviceMultiSampleType(D3DADAPTER_DEFAULT,
                D3DDEVTYPE_HAL, depthStencilFmt, windowed, type,
                &depthStencilQualityLevels);

            if (SUCCEEDED(hr))
            {
                if (backBufferQualityLevels == depthStencilQualityLevels)
                {
                    // The valid range is between zero and one less than the level
                    // returned by IDirect3D9::CheckDeviceMultiSampleType().

                    if (backBufferQualityLevels > 0)
                        qualityLevels = backBufferQualityLevels - 1;
                    else
                        qualityLevels = backBufferQualityLevels;

                    return true;
                }
            }
        }

        return false;
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

    bool RenderManager::CheckRayInMesh(const D3DXMATRIX& matWorld, LPD3DXMESH mesh, float* dist)
    {
        ThreadSync sync;

        POINT ptCursor;
        GetCursorPos(&ptCursor);
        ScreenToClient(hWnd, &ptCursor);

        D3DVIEWPORT9 view;
        D3D_DEVICE->GetViewport(&view);

        D3DXMATRIX projMat;
        D3D_DEVICE->GetTransform(D3DTS_PROJECTION, &projMat);

        float vx = (+2.0f*ptCursor.x / view.Width - 1.0f) / projMat._11;
        float vy = (-2.0f*ptCursor.y / view.Height + 1.0f) / projMat._22;

        D3DXVECTOR3 Origin(0.0f, 0.0f, 0.0f);
        D3DXVECTOR3 Direction(vx, vy, 1.0f);

        D3DXMATRIX viewMat;
        D3D_DEVICE->GetTransform(D3DTS_VIEW, &viewMat);

        D3DXMATRIX iviewMat;
        D3DXMatrixInverse(&iviewMat, 0, &viewMat);
        D3DXVec3TransformCoord(&Origin, &Origin, &iviewMat);
        D3DXVec3TransformNormal(&Direction, &Direction, &iviewMat);

        D3DXVECTOR3 vOrg;
        D3DXVECTOR3 vDir;
        D3DXMATRIX Inversemat;

        D3DXMatrixInverse(&Inversemat, nullptr, &matWorld);
        D3DXVec3TransformCoord(&vOrg, &Origin, &Inversemat);
        D3DXVec3TransformNormal(&vDir, &Direction, &Inversemat);

        BOOL bHit = FALSE;
        
        D3DXIntersect(mesh, &vOrg, &vDir, &bHit, nullptr, nullptr, nullptr, dist, nullptr, nullptr);

        return bHit;
    }

    RAY	RenderManager::GetPickingRayToView(bool isMouseCenter)
    {
        ThreadSync sync;

        POINT ptCursor;
        GetCursorPos(&ptCursor);
        ScreenToClient(hWnd, &ptCursor);

        D3DVIEWPORT9 view;
        D3D_DEVICE->GetViewport(&view);

        D3DXMATRIX projMat;
        D3D_DEVICE->GetTransform(D3DTS_PROJECTION, &projMat);

        float vx = (+2.0f*ptCursor.x / view.Width - 1.0f) / projMat._11;
        float vy = (-2.0f*ptCursor.y / view.Height + 1.0f) / projMat._22;

        D3DXVECTOR3 Origin(0.0f, 0.0f, 0.0f);
        D3DXVECTOR3 Direction(vx, vy, 1.0f);

        D3DXMATRIX viewMat;
        D3D_DEVICE->GetTransform(D3DTS_VIEW, &viewMat);

        D3DXMATRIX iviewMat;
        D3DXMatrixInverse(&iviewMat, 0, &viewMat);
        D3DXVec3TransformCoord(&Origin, &Origin, &iviewMat);
        D3DXVec3TransformNormal(&Direction, &Direction, &iviewMat);
        
        RAY ray;
        ray.origin = Origin;
        ray.direction = Direction;

        return ray;
        ///return TransRayToView(GetPickingRay(isMouseCenter));
    }

    RAY	RenderManager::TransRayToView(RAY ray)
    {
        ThreadSync sync;

        D3DXMATRIX view;
        D3DXMATRIX viewInverse;

        D3D_DEVICE->GetTransform(D3DTS_VIEW, &view);

        D3DXMatrixInverse(&viewInverse, nullptr, &view);
        D3DXVec3TransformCoord(&ray.origin, &ray.origin, &viewInverse);
        D3DXVec3TransformNormal(&ray.direction, &ray.direction, &viewInverse);
        D3DXVec3Normalize(&ray.direction, &ray.direction);

        return ray;
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
            LogMgr->Error("Not Exist Device - FAILED");
            return nullptr;
        }
    }

    void RenderManager::SetupPixelFog(bool on, D3DXCOLOR color, float start, float end, float density, int mode)
    {
        ThreadSync sync;

        D3D_DEVICE->SetRenderState(D3DRS_FOGENABLE, on);
        D3D_DEVICE->SetRenderState(D3DRS_FOGCOLOR, color);

        if (mode == D3DFOG_LINEAR)
        {
            D3D_DEVICE->SetRenderState(D3DRS_FOGTABLEMODE, mode);
            D3D_DEVICE->SetRenderState(D3DRS_FOGSTART, *(int *)(&start));
            D3D_DEVICE->SetRenderState(D3DRS_FOGEND, *(int *)(&end));
        }
        else
        {
            D3D_DEVICE->SetRenderState(D3DRS_FOGTABLEMODE, mode);
            D3D_DEVICE->SetRenderState(D3DRS_FOGDENSITY, *(int *)(&density));
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

        LogMgr->Info("Render Manager is Clear.");
    }

    bool RenderManager::UseImGUI()
    {
        ThreadSync sync;

        return useImGUI;
    }

    void RenderManager::UseImGUI(bool use)
    {
        ThreadSync sync;

        useImGUI = use;
    }

    std::string RenderManager::GetLastRenderedMesh()
    {
        return lastRenderedMesh;
    }

    void RenderManager::SetLastRenderedMesh(std::string name)
    {
        ThreadSync sync;

        lastRenderedMesh = name;
    }
}