#include "Common.h"
#include "Scene.h"
#include "RenderManager.h"
#include "SceneManager.h"
#include "Transform3D.h"
#include "StaticMesh.h"

D3DXVECTOR3			g_vUp = D3DXVECTOR3(0.0f, 1.0f, 0.0f);
BOOL				g_bFiltered = TRUE;

#define SHADOW_MAP_SIZE	512
#define SCREEN_WIDTH	1024
#define SCREEN_HEIGHT	768

#define SAFE_RELEASE( x )	{ if( x ) { x->Release(); x = NULL; } }

// Vertex element
D3DVERTEXELEMENT9 dwElement[] =
{
    { 0,   0, D3DDECLTYPE_FLOAT3, D3DDECLMETHOD_DEFAULT, D3DDECLUSAGE_POSITION, 0 },

    { 0,  12, D3DDECLTYPE_FLOAT3, D3DDECLMETHOD_DEFAULT, D3DDECLUSAGE_NORMAL,   0 },

    { 0,  24, D3DDECLTYPE_FLOAT2, D3DDECLMETHOD_DEFAULT, D3DDECLUSAGE_TEXCOORD, 0 },

    D3DDECL_END()
};

struct QuadVertex
{
    D3DXVECTOR4 p;
    D3DXVECTOR2 t;
};

#define FVF_QUADVERTEX (D3DFVF_XYZRHW | D3DFVF_TEX1)

float GetGaussianDistribution(float x, float y, float rho) {
    float g = 1.0f / sqrt(2.0f * 3.141592654f * rho * rho);
    return g * exp(-(x * x + y * y) / (2 * rho * rho));
}

void GetGaussianOffsets(bool bHorizontal, D3DXVECTOR2 vViewportTexelSize,
    D3DXVECTOR2* vSampleOffsets, float* fSampleWeights) {
    // Get the center texel offset and weight
    fSampleWeights[0] = 1.0f * GetGaussianDistribution(0, 0, 2.0f);
    vSampleOffsets[0] = D3DXVECTOR2(0.0f, 0.0f);

    // Get the offsets and weights for the remaining taps
    if (bHorizontal) {
        for (int i = 1; i < 15; i += 2) {
            vSampleOffsets[i + 0] = D3DXVECTOR2(i * vViewportTexelSize.x, 0.0f);
            vSampleOffsets[i + 1] = D3DXVECTOR2(-i * vViewportTexelSize.x, 0.0f);

            fSampleWeights[i + 0] = 2.0f * GetGaussianDistribution(float(i + 0), 0.0f, 3.0f);
            fSampleWeights[i + 1] = 2.0f * GetGaussianDistribution(float(i + 1), 0.0f, 3.0f);
        }
    }

    else {
        for (int i = 1; i < 15; i += 2) {
            vSampleOffsets[i + 0] = D3DXVECTOR2(0.0f, i * vViewportTexelSize.y);
            vSampleOffsets[i + 1] = D3DXVECTOR2(0.0f, -i * vViewportTexelSize.y);

            fSampleWeights[i + 0] = 2.0f * GetGaussianDistribution(0.0f, float(i + 0), 3.0f);
            fSampleWeights[i + 1] = 2.0f * GetGaussianDistribution(0.0f, float(i + 1), 3.0f);
        }
    }
}

namespace BONE_GRAPHICS
{
    bool Scene::InitShader()
    {
        // Create the quad vertex buffer
        if (FAILED(RenderMgr->GetDevice()->CreateVertexBuffer(4 * sizeof(QuadVertex), D3DUSAGE_WRITEONLY, FVF_QUADVERTEX,
            D3DPOOL_DEFAULT, &quadVB, NULL)))
        {
            return E_FAIL;
        }

        // Copy the vertices
        QuadVertex* pVertices;
        quadVB->Lock(0, 0, (void**)&pVertices, 0);
        pVertices[0].p = D3DXVECTOR4(0.0f, 0.0f, 0.0f, 1.0f);
        pVertices[1].p = D3DXVECTOR4(0.0f, SCREEN_HEIGHT / 2, 0.0f, 1.0f);
        pVertices[2].p = D3DXVECTOR4(SCREEN_WIDTH / 2, 0.0f, 0.0f, 1.0f);
        pVertices[3].p = D3DXVECTOR4(SCREEN_WIDTH / 2, SCREEN_HEIGHT / 2, 0.0f, 1.0f);

        pVertices[0].t = D3DXVECTOR2(0.0f, 0.0f);
        pVertices[1].t = D3DXVECTOR2(0.0f, 1.0f);
        pVertices[2].t = D3DXVECTOR2(1.0f, 0.0f);
        pVertices[3].t = D3DXVECTOR2(1.0f, 1.0f);
        quadVB->Unlock();

        for (int i = 0; i < 8; i++)
        {
            // Create the shadow map
            if (FAILED(RenderMgr->GetDevice()->CreateTexture(SHADOW_MAP_SIZE, SHADOW_MAP_SIZE, 1, D3DUSAGE_RENDERTARGET,
                D3DFMT_R32F, D3DPOOL_DEFAULT, &shadowMap[i], NULL))) {
                return E_FAIL;
            }

            // Grab the texture's surface
            shadowMap[i]->GetSurfaceLevel(0, &shadowSurf[i]);
            
            // Create the shadow depth surface
            if (FAILED(RenderMgr->GetDevice()->CreateDepthStencilSurface(SHADOW_MAP_SIZE, SHADOW_MAP_SIZE, D3DFMT_D16,
                D3DMULTISAMPLE_NONE, 0, TRUE, &shadowDepth[i], NULL))) {
                return E_FAIL;
            }
        }

        // Create the screen-sized buffer map
        if (FAILED(RenderMgr->GetDevice()->CreateTexture(SCREEN_WIDTH / 2, SCREEN_HEIGHT / 2, 1, D3DUSAGE_RENDERTARGET,
            D3DFMT_A8R8G8B8, D3DPOOL_DEFAULT, &screenMap, NULL))) {
            return E_FAIL;
        }

        // Grab the texture's surface
        screenMap->GetSurfaceLevel(0, &screenSurf);

        // Create the blur maps
        for (int i = 0; i < 2; i++)
        {
            if (FAILED(RenderMgr->GetDevice()->CreateTexture(SCREEN_WIDTH / 2, SCREEN_HEIGHT / 2, 1, D3DUSAGE_RENDERTARGET,
                D3DFMT_A8R8G8B8, D3DPOOL_DEFAULT, &blurMap[i], NULL))) {
                return E_FAIL;
            }

            // Grab the texture's surface
            blurMap[i]->GetSurfaceLevel(0, &blurSurf[i]);
        }

        // Create the general depth surface
        if (FAILED(RenderMgr->GetDevice()->CreateDepthStencilSurface(SCREEN_WIDTH / 2, SCREEN_HEIGHT / 2, D3DFMT_D16,
            D3DMULTISAMPLE_NONE, 0, TRUE, &newDepthRT, NULL))) {
            return E_FAIL;
        }

        for (int i = 0; i < 8; i++)
        {
            PointLight* light = new PointLight;

            Transform3D* tr = new Transform3D();
            light->AddComponent(tr);
            
            light->SetAmbient(1.0f, 1.0f, 1.0f, 1.0f);
            light->SetDiffuse(1.0f, 1.0f, 1.0f, 1.0f);
            light->SetSpecular(0.5f, 0.5f, 0.5f, 0.5f);
            light->SetRadius(200);
            light->SetShadowAimPos(Vec3(0, 0, 0));

            if (i == 0)
                light->SetLight(true);
            else
                light->SetLight(false);

            light->SetTag("EditorObject");

            light->Init();
            light->LoadContents();
            light->Reference();

            pointLightList.push_back(light);
        }
    }

    bool Scene::RelShader()
    {
        SAFE_RELEASE(quadVB);

        for (int i = 0; i < 8; i++)
        {
            SAFE_RELEASE(shadowMap[i]);
            SAFE_RELEASE(shadowSurf[i]);
            SAFE_RELEASE(shadowDepth[i]);
        }

        SAFE_RELEASE(screenMap);
        SAFE_RELEASE(screenSurf);

        for (int i = 0; i < 2; i++)
        {
            SAFE_RELEASE(blurMap[i]);
            SAFE_RELEASE(blurSurf[i]);
        }

        SAFE_RELEASE(newDepthRT);
        SAFE_RELEASE(oldColorRT);
        SAFE_RELEASE(oldDepthRT);

        return true;
    }

    void Scene::Render()
    {
        std::vector<GameObject*> DefaultShaderObjects;
        
        for each(auto var in objectList)
        {
            if (var->GetPipeLine() == GameObject::PIPE_LINE::DEFAULT_SHADER)
                DefaultShaderObjects.push_back(var);
            else
                var->Render();
        }

        for each(auto var in staticObjectList)
        {
            if (var->GetPipeLine() == GameObject::PIPE_LINE::DEFAULT_SHADER)
                DefaultShaderObjects.push_back(var);
            else
                var->Render();
        }

        if (DefaultShaderObjects.size() == 0)
            return;

#pragma region ---------------- Set Shader ----------------

        auto DefaultEffect = ResourceMgr->LoadEffect("DefaultEffect.fx");
        auto MainCamera = GetCurrentCamera();

        if (MainCamera == nullptr)
            return;

        auto CameraComponent = ((Camera*)MainCamera->GetComponent("Camera"));
        
        D3DXMATRIX matWorld;
        D3DXMatrixIdentity(&matWorld);
        D3DXMATRIX matView = CameraComponent->GetViewMatrix(MainCamera);
        D3DXMATRIX matProj = CameraComponent->GetProjectionMatrix();

        D3DXMATRIX matWorldViewProj = matWorld * matView * matProj;

        D3DXMATRIX matWorldIT;
        D3DXMatrixInverse(&matWorldIT, NULL, &matWorld);
        D3DXMatrixTranspose(&matWorldIT, &matWorldIT);

        DefaultEffect->SetMatrix("g_matWorld", &matWorld);
        DefaultEffect->SetMatrix("g_matWorldIT", &matWorldIT);
        DefaultEffect->SetMatrix("g_matWorldViewProj", &matWorldViewProj);

        D3DXMATRIX matLightViewProj[8];

        // Compute the texture matrix
        float fTexOffs = 0.5 + (0.5 / (float)SHADOW_MAP_SIZE);
        D3DXMATRIX matTexAdj(0.5f, 0.0f, 0.0f, 0.0f,
            0.0f, -0.5f, 0.0f, 0.0f,
            0.0f, 0.0f, 1.0f, 0.0f,
            fTexOffs, fTexOffs, 0.0f, 1.0f);

        DefaultEffect->SetVector("g_vEyePos", (D3DXVECTOR4*)&(((Transform3D*)MainCamera->transform3D)->GetPosition()));
        DefaultEffect->SetBool("g_bFiltered", g_bFiltered);

        UINT uPasses = 0;

#pragma endregion

#pragma region ---------------- Create ShadowMap ----------------

        // Grab the old render target and depth buffer
        RenderMgr->GetDevice()->GetRenderTarget(0, &oldColorRT);
        RenderMgr->GetDevice()->GetDepthStencilSurface(&oldDepthRT);

        auto Lights = SceneMgr->CurrentScene()->GetPointLights();

        int i = 0;

        BOOL OnLight[8] = { false };
        for each (auto var in Lights)
        {
            OnLight[i] = var->IsOn();
            i++;
        }

        DefaultEffect->SetBoolArray("g_bOnLight", const_cast<const BOOL*>(OnLight), 8);

        i = 0;
        for each (auto var in Lights)
        {
            var->SetPosition(Vec3(0, 30, 30));
            D3DXMatrixLookAtLH(&matView, &Vec3(0, 30, 30), &var->GetShadowAimPos(), &g_vUp);
            D3DXMatrixOrthoLH(&matProj, 45.0f, 45.0f, 1.0f, 1024.0f);
            matLightViewProj[i] = matWorld * matView * matProj;

            RGBA diffuse = var->GetDiffuse();

            // Setup the effect variables
            DefaultEffect->SetMatrix("g_matLightViewProj", &matLightViewProj[i]);
            DefaultEffect->SetVector("g_vLightPos", (D3DXVECTOR4*)&Vec3(0, 30, 30));
            DefaultEffect->SetVector("g_vLightColor", &D3DXVECTOR4(diffuse.r, diffuse.g, diffuse.b, diffuse.a));
            DefaultEffect->CommitChanges();

            // Render the scene depth to the shadow map
            RenderMgr->GetDevice()->SetRenderTarget(0, shadowSurf[i]);
            RenderMgr->GetDevice()->SetDepthStencilSurface(shadowDepth[i]);

            // Clear the viewport
            RenderMgr->GetDevice()->Clear(0, NULL, D3DCLEAR_TARGET | D3DCLEAR_ZBUFFER, 0xFFFFFFFF, 1.0f, 0);

            // Set the technique
            DefaultEffect->SetTechnique("techShadow");

            // Render the effect
            DefaultEffect->Begin(&uPasses, 0);

            for (UINT uPass = 0; uPass < uPasses; uPass++)
            {
                DefaultEffect->BeginPass(uPass);

                for each (auto Iter in DefaultShaderObjects)
                    if (Iter->GetActive())
                    {
                        auto staticMesh = ((StaticMesh*)Iter->GetComponent("StaticMesh"));
                        if (staticMesh != nullptr)
                        {
                            auto Mesh = ResourceMgr->FindMesh(staticMesh->GetFile());

                            D3DXMATRIX matWorld, matWorldIT;
                            matWorld = ((Transform3D*)Iter->transform3D)->GetTransform();
                            D3DXMatrixInverse(&matWorldIT, NULL, &matWorld);
                            D3DXMatrixTranspose(&matWorldIT, &matWorldIT);
                            DefaultEffect->SetMatrix("g_matWorld", &matWorld);
                            DefaultEffect->SetMatrix("g_matWorldIT", &matWorldIT);
                            DefaultEffect->CommitChanges();

                            for (int i = 0; i < Mesh->numMaterials; i++)
                                Mesh->mesh->DrawSubset(i);
                        }
                    }
                
                DefaultEffect->EndPass();
            }

            DefaultEffect->End();
        }

        D3DXMATRIX matTexture[8];
        for (int i = 0; i < 8; i++)
            matTexture[i] = matLightViewProj[i] * matTexAdj;

#pragma endregion

#pragma region ---------------- Tech Unlit ----------------

        {
            // Render the shadowed scene into the screen map
            RenderMgr->GetDevice()->SetRenderTarget(0, screenSurf);
            RenderMgr->GetDevice()->SetDepthStencilSurface(newDepthRT);

            // Clear the viewport
            RenderMgr->GetDevice()->Clear(0, NULL, D3DCLEAR_TARGET | D3DCLEAR_ZBUFFER, 0x00000000, 1.0f, 0);

            // Set the technique
            DefaultEffect->SetTechnique("techUnlit");

            // Set the textures
            DefaultEffect->SetMatrix("g_matTexture", &matTexture[0]);
            DefaultEffect->SetMatrix("g_matTexture2", &matTexture[1]);
            DefaultEffect->SetMatrix("g_matTexture3", &matTexture[2]);
            DefaultEffect->SetMatrix("g_matTexture4", &matTexture[3]);
            DefaultEffect->SetMatrix("g_matTexture5", &matTexture[4]);
            DefaultEffect->SetMatrix("g_matTexture6", &matTexture[5]);
            DefaultEffect->SetMatrix("g_matTexture7", &matTexture[6]);
            DefaultEffect->SetMatrix("g_matTexture8", &matTexture[7]);
            DefaultEffect->SetTexture("tShadowMap1", shadowMap[0]);
            DefaultEffect->SetTexture("tShadowMap2", shadowMap[1]);
            DefaultEffect->SetTexture("tShadowMap3", shadowMap[2]);
            DefaultEffect->SetTexture("tShadowMap4", shadowMap[3]);
            DefaultEffect->SetTexture("tShadowMap5", shadowMap[4]);
            DefaultEffect->SetTexture("tShadowMap6", shadowMap[5]);
            DefaultEffect->SetTexture("tShadowMap7", shadowMap[6]);
            DefaultEffect->SetTexture("tShadowMap7", shadowMap[7]);

            uPasses = 0;
            DefaultEffect->Begin(&uPasses, 0);

            for (UINT uPass = 0; uPass < uPasses; uPass++)
            {
                DefaultEffect->BeginPass(uPass);

                for each (auto Iter in DefaultShaderObjects)
                    if (Iter->GetActive())
                    {
                        auto staticMesh = ((StaticMesh*)Iter->GetComponent("StaticMesh"));
                        if (staticMesh != nullptr)
                        {
                            auto Mesh = ResourceMgr->FindMesh(staticMesh->GetFile());

                            D3DXMATRIX matWorld, matWorldIT;
                            matWorld = ((Transform3D*)Iter->transform3D)->GetTransform();
                            D3DXMatrixInverse(&matWorldIT, NULL, &matWorld);
                            D3DXMatrixTranspose(&matWorldIT, &matWorldIT);
                            DefaultEffect->SetMatrix("g_matWorld", &matWorld);
                            DefaultEffect->SetMatrix("g_matWorldIT", &matWorldIT);
                            DefaultEffect->CommitChanges();

                            for (int i = 0; i < Mesh->numMaterials; i++)
                                Mesh->mesh->DrawSubset(i);
                        }
                    }

                DefaultEffect->EndPass();
            }

            DefaultEffect->End();

        }

#pragma endregion

#pragma region ---------------- Tech Blur ----------------

        // Blur the screen map horizontally
        RenderMgr->GetDevice()->SetRenderTarget(0, blurSurf[0]);
        RenderMgr->GetDevice()->SetDepthStencilSurface(newDepthRT);

        // Clear the viewport
        RenderMgr->GetDevice()->Clear(0, NULL, D3DCLEAR_TARGET | D3DCLEAR_ZBUFFER, 0x00000000, 1.0f, 0);

        // Set the technique
        DefaultEffect->SetTechnique("techBlurH");

        // Compute the Gaussian offsets
        GetGaussianOffsets(TRUE, D3DXVECTOR2(1.0f / (FLOAT)SHADOW_MAP_SIZE, 1.0f / (FLOAT)SHADOW_MAP_SIZE),
            sampleOffsets, sampleWeights);
        DefaultEffect->SetValue("g_vSampleOffsets", sampleOffsets, 15 * sizeof(D3DXVECTOR2));
        DefaultEffect->SetValue("g_fSampleWeights", sampleWeights, 15 * sizeof(FLOAT));

        // Set the textures
        DefaultEffect->SetTexture("tScreenMap", screenMap);

        // Render the effect
        uPasses = 0;
        DefaultEffect->Begin(&uPasses, 0);

        for (UINT uPass = 0; uPass < uPasses; uPass++)
        {
            DefaultEffect->BeginPass(uPass);

            RenderMgr->GetDevice()->SetStreamSource(0, quadVB, 0, sizeof(QuadVertex));
            RenderMgr->GetDevice()->SetFVF(FVF_QUADVERTEX);
            RenderMgr->GetDevice()->DrawPrimitive(D3DPT_TRIANGLESTRIP, 0, 2);

            DefaultEffect->EndPass();
        }
        DefaultEffect->End();

        // Blur the screen map vertically
        RenderMgr->GetDevice()->SetRenderTarget(0, blurSurf[1]);
        RenderMgr->GetDevice()->SetDepthStencilSurface(newDepthRT);

        // Clear the viewport
        RenderMgr->GetDevice()->Clear(0, NULL, D3DCLEAR_TARGET | D3DCLEAR_ZBUFFER, 0x00000000, 1.0f, 0);

        // Set the technique
        DefaultEffect->SetTechnique("techBlurV");

        // Compute the Gaussian offsets
        GetGaussianOffsets(FALSE, D3DXVECTOR2(1.0f / (FLOAT)SHADOW_MAP_SIZE, 1.0f / (FLOAT)SHADOW_MAP_SIZE), sampleOffsets, sampleWeights);
        DefaultEffect->SetValue("g_vSampleOffsets", sampleOffsets, 15 * sizeof(D3DXVECTOR2));
        DefaultEffect->SetValue("g_fSampleWeights", sampleWeights, 15 * sizeof(FLOAT));

        // Set the textures
        DefaultEffect->SetTexture("tBlurHMap", blurMap[0]);

        // Render the effect
        uPasses = 0;
        DefaultEffect->Begin(&uPasses, 0);

        for (UINT uPass = 0; uPass < uPasses; uPass++)
        {
            DefaultEffect->BeginPass(uPass);

            RenderMgr->GetDevice()->SetStreamSource(0, quadVB, 0, sizeof(QuadVertex));
            RenderMgr->GetDevice()->SetFVF(FVF_QUADVERTEX);
            RenderMgr->GetDevice()->DrawPrimitive(D3DPT_TRIANGLESTRIP, 0, 2);

            DefaultEffect->EndPass();
        }

        // End the effect
        DefaultEffect->End();

#pragma endregion

#pragma region ---------------- Tech Scene ----------------

        // Restore the render target and depth buffer
        RenderMgr->GetDevice()->SetRenderTarget(0, oldColorRT);
        RenderMgr->GetDevice()->SetDepthStencilSurface(oldDepthRT);
        SAFE_RELEASE(oldColorRT);
        SAFE_RELEASE(oldDepthRT);

        // Finally, render the shadowed scene
        DefaultEffect->SetTechnique("techScene");

        // Set the textures
        DefaultEffect->SetTexture("tBlurVMap", blurMap[1]);

        // Render the effect
        uPasses = 0;
        DefaultEffect->Begin(&uPasses, 0);

        // Do we need to render the scene in wireframe mode        
        //if (g_bWireframe)
        //    RenderMgr->GetDevice()->SetRenderState(D3DRS_FILLMODE, D3DFILL_WIREFRAME);

        for (UINT uPass = 0; uPass < uPasses; uPass++)
        {
            DefaultEffect->BeginPass(uPass);

            for each (auto Iter in DefaultShaderObjects)
                if (Iter->GetActive())
                {
                    auto staticMesh = ((StaticMesh*)Iter->GetComponent("StaticMesh"));
                    if (staticMesh != nullptr)
                    {
                        auto Textures = staticMesh->GetTextures();
                        auto Mesh = ResourceMgr->FindMesh(staticMesh->GetFile());

                        D3DXMATRIX matWorld, matWorldIT;
                        matWorld = ((Transform3D*)Iter->transform3D)->GetTransform();
                        D3DXMatrixInverse(&matWorldIT, NULL, &matWorld);
                        D3DXMatrixTranspose(&matWorldIT, &matWorldIT);
                        DefaultEffect->SetTexture("tColorMap", ResourceMgr->LoadTexture(Textures[i]));
                        DefaultEffect->SetMatrix("g_matWorld", &matWorld);
                        DefaultEffect->SetMatrix("g_matWorldIT", &matWorldIT);
                        DefaultEffect->CommitChanges();

                        for (int i = 0; i < Mesh->numMaterials; i++)
                            Mesh->mesh->DrawSubset(i);
                    }
                }
            
            DefaultEffect->EndPass();
        }

        DefaultEffect->End();

#pragma endregion
    }
}