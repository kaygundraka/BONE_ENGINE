#include "Common.h"
#include "Scene.h"
#include "RenderManager.h"
#include "SceneManager.h"
#include "Transform3D.h"
#include "StaticMesh.h"
#include "Material.h"
#pragma warning(disable:4996)

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

            light->SetTag("EditorObject");

            light->Init();
            light->LoadContents();
            light->Reference();

            char Temp[2];
            itoa(i, Temp, 10);
            std::string name = "PointLight_";
            name += Temp;

            light->SetName(name);

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

        // ---------------- Set Shader ----------------

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

        DefaultEffect->SetMatrix("matWorld", &matWorld);
        DefaultEffect->SetMatrix("matWorldIT", &matWorldIT);
        DefaultEffect->SetMatrix("matWorldViewProj", &matWorldViewProj);

        float GlobalAmbient[4] = {
            globalAmbient.r,
            globalAmbient.g,
            globalAmbient.b,
            globalAmbient.a
        };

        DefaultEffect->SetValue("globalAmbient", &GlobalAmbient, sizeof(GlobalAmbient));

        D3DXMATRIX matLightViewProj[8];

        // Compute the texture matrix
        float fTexOffs = 0.5 + (0.5 / (float)SHADOW_MAP_SIZE);
        
        D3DXMATRIX matTexAdj(0.5f, 0.0f, 0.0f, 0.0f,
            0.0f, -0.5f, 0.0f, 0.0f,
            0.0f, 0.0f, 1.0f, 0.0f,
            fTexOffs, fTexOffs, 0.0f, 1.0f
        );

        DefaultEffect->SetVector("eyePos", (D3DXVECTOR4*)&(((Transform3D*)MainCamera->transform3D)->GetPosition()));
        
        UINT uPasses = 0;

#pragma region ---------------- Set Lighting --------------------
        D3DXHANDLE HandleLight;
        D3DXHANDLE HandleLightPos;
        D3DXHANDLE HandleLightAmbient;
        D3DXHANDLE HandleLightDiffuse;
        D3DXHANDLE HandleLightSpecular;
        D3DXHANDLE HandleLightRadius;

        int i = 0;
                
        for (auto iter = pointLightList.begin(); iter != pointLightList.end(); iter++)
        {
            onLight[i] = (*iter)->IsOn();
            
            HandleLight = DefaultEffect->GetParameterElement("lights", i++);

            HandleLightPos = DefaultEffect->GetParameterByName(HandleLight, "pos");
            HandleLightAmbient = DefaultEffect->GetParameterByName(HandleLight, "ambient");
            HandleLightDiffuse = DefaultEffect->GetParameterByName(HandleLight, "diffuse");
            HandleLightSpecular = DefaultEffect->GetParameterByName(HandleLight, "specular");
            HandleLightRadius = DefaultEffect->GetParameterByName(HandleLight, "radius");

            float Pos[3] = {
                (*iter)->GetPosition().x,
                (*iter)->GetPosition().y,
                (*iter)->GetPosition().z,
            };

            float LightAmbient[4] = {
                (*iter)->GetAmbient().r,
                (*iter)->GetAmbient().g,
                (*iter)->GetAmbient().b,
                (*iter)->GetAmbient().a
            };

            float LightDiffuse[4] = {
                (*iter)->GetDiffuse().r,
                (*iter)->GetDiffuse().g,
                (*iter)->GetDiffuse().b,
                (*iter)->GetDiffuse().a
            };

            float LightSpecular[4] = {
                (*iter)->GetSpecular().r,
                (*iter)->GetSpecular().g,
                (*iter)->GetSpecular().b,
                (*iter)->GetSpecular().a
            };

            DefaultEffect->SetValue(HandleLightPos, Pos, sizeof(Pos));
            DefaultEffect->SetValue(HandleLightAmbient, LightAmbient, sizeof(LightAmbient));
            DefaultEffect->SetValue(HandleLightDiffuse, LightDiffuse, sizeof(LightDiffuse));
            DefaultEffect->SetValue(HandleLightSpecular, LightSpecular, sizeof(LightSpecular));
            DefaultEffect->SetFloat(HandleLightRadius, (*iter)->GetRadius());
        }

        DefaultEffect->SetBoolArray("onLight", onLight, 8);

#pragma endregion

#pragma region ---------------- Create ShadowMap ----------------

        // Grab the old render target and depth buffer
        RenderMgr->GetDevice()->GetRenderTarget(0, &oldColorRT);
        RenderMgr->GetDevice()->GetDepthStencilSurface(&oldDepthRT);

        i = 0;
        for each (auto var in pointLightList)
        {
            D3DXMatrixLookAtLH(&matView, &var->GetPosition(), &var->GetShadowAimPos(), &g_vUp);
            D3DXMatrixOrthoLH(&matProj, 1024.0f, 1024.0f, 1.0f, 1024.0f);
            matLightViewProj[i] = matWorld * matView * matProj;

            RGBA diffuse = var->GetDiffuse();

            // Setup the effect variables
            DefaultEffect->SetMatrix("matLightViewProj", &matLightViewProj[i]);
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
                            DefaultEffect->SetMatrix("matWorld", &matWorld);
                            DefaultEffect->SetMatrix("matWorldIT", &matWorldIT);
                            DefaultEffect->CommitChanges();

                            for (int j = 0; j < Mesh->numMaterials; j++)
                                Mesh->mesh->DrawSubset(j);
                        }
                    }
                
                DefaultEffect->EndPass();
            }

            DefaultEffect->End();
            
            i++;
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
            DefaultEffect->SetMatrix("matTexture1", &matTexture[0]);
            DefaultEffect->SetMatrix("matTexture2", &matTexture[1]);
            DefaultEffect->SetMatrix("matTexture3", &matTexture[2]);
            DefaultEffect->SetMatrix("matTexture4", &matTexture[3]);
            DefaultEffect->SetMatrix("matTexture5", &matTexture[4]);
            DefaultEffect->SetMatrix("matTexture6", &matTexture[5]);
            DefaultEffect->SetMatrix("matTexture7", &matTexture[6]);
            DefaultEffect->SetMatrix("matTexture8", &matTexture[7]);
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
                            DefaultEffect->SetMatrix("matWorld", &matWorld);
                            DefaultEffect->SetMatrix("matWorldIT", &matWorldIT);
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
        DefaultEffect->SetValue("sampleOffsets", sampleOffsets, 15 * sizeof(D3DXVECTOR2));
        DefaultEffect->SetValue("sampleWeights", sampleWeights, 15 * sizeof(FLOAT));

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
        DefaultEffect->SetValue("sampleOffsets", sampleOffsets, 15 * sizeof(D3DXVECTOR2));
        DefaultEffect->SetValue("sampleWeights", sampleWeights, 15 * sizeof(FLOAT));

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

        i = 0;

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
                        SetRenderMatrial(Iter, DefaultEffect);

                        DefaultEffect->SetTexture("tColorMap", ResourceMgr->LoadTexture(Textures[i]));
                        DefaultEffect->SetMatrix("matWorld", &matWorld);
                        DefaultEffect->SetMatrix("matWorldIT", &matWorldIT);
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

    void Scene::SetRenderMatrial(GameObject* object, ID3DXEffect* effect) 
    {
        auto material = (Material*)object->GetComponent("Material");

        if (material == nullptr)
        {
            material = new Material();
            object->AddComponent(material);
        }

        float MatAmbient[4] = {
            material->GetAmbient().r,
            material->GetAmbient().g,
            material->GetAmbient().b,
            material->GetAmbient().a,
        };

        float MatDiffuse[4] = {
            material->GetDiffuse().r,
            material->GetDiffuse().g,
            material->GetDiffuse().b,
            material->GetDiffuse().a,
        };

        float MatEmissive[4] = {
            material->GetEmissive().r,
            material->GetEmissive().g,
            material->GetEmissive().b,
            material->GetEmissive().a,
        };

        float MatSpecular[4] = {
            material->GetSpecular().r,
            material->GetSpecular().g,
            material->GetSpecular().b,
            material->GetSpecular().a,
        };

        effect->SetValue("material.ambient", MatAmbient, sizeof(MatAmbient));
        effect->SetValue("material.diffuse", MatDiffuse, sizeof(MatDiffuse));
        effect->SetValue("material.emissive", MatEmissive, sizeof(MatEmissive));
        effect->SetValue("material.specular", MatSpecular, sizeof(MatSpecular));
        effect->SetFloat("material.shininess", material->GetShininess());
    }
}