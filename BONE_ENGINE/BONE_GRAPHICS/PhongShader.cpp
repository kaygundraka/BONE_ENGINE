#include "Common.h"
#include "PhongShader.h"
#include "ResourceManager.h"
#include "Material.h"
#include "StaticMesh.h"
#include "SkinnedMesh.h"
#include "Scene.h"
#include "SceneManager.h"
#include "Transform3D.h"
#include "Camera.h"

namespace BONE_GRAPHICS
{
    PhongShader::PhongShader()
    {
        SetAddress("blinn_phong_sm30.fx");
        effect = nullptr;
    }

    PhongShader::~PhongShader()
    {

    }

    void PhongShader::SetMaterial(GameObject* parent)
    {
        auto material = (Material*)parent->GetComponent("Material");

        if (material == nullptr)
        {
            material = new Material();
            parent->AddComponent(material);
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

    void PhongShader::SetEnvironment(GameObject* parent)
    {/*
        auto CurScene = SceneMgr->CurrentScene();

        auto LightList = CurScene->GetPointLights();
        int LightNumber = LightList.size();

        auto CurCameraObject = (GameObject*)SceneMgr->CurrentScene()->GetCurrentCamera();
        auto CurCameraComponent = (Camera*)CurCameraObject->GetComponent("Camera");
        auto CurCameraTransform = (Transform3D*)CurCameraObject->GetComponent("Transform3D");

        float CameraPos[3] = {
            CurCameraTransform->GetPosition().x,
            CurCameraTransform->GetPosition().y,
            CurCameraTransform->GetPosition().z,
        };

        float GlobalAmbient[4] = {
            CurScene->GetAmbientColor().r,
            CurScene->GetAmbientColor().g,
            CurScene->GetAmbientColor().b,
            CurScene->GetAmbientColor().a
        };

        auto MatViewProj = CurCameraComponent->GetViewMatrix(CurCameraObject) * CurCameraComponent->GetProjectionMatrix();

        Matrix MatWorld = ((Transform3D*)parent->transform3D)->GetTransform();

        effect->SetMatrix("worldMatrix", &MatWorld);
        effect->SetMatrix("worldInverseTransposeMatrix", &MatWorld);
        effect->SetMatrix("worldViewProjectionMatrix", &MatViewProj);

        effect->SetValue("cameraPos", &CameraPos, sizeof(CameraPos));
        effect->SetValue("globalAmbient", &GlobalAmbient, sizeof(GlobalAmbient));
        effect->SetValue("numLights", &LightNumber, sizeof(LightNumber));


        D3DXHANDLE HandleLight;
        D3DXHANDLE HandleLightPos;
        D3DXHANDLE HandleLightAmbient;
        D3DXHANDLE HandleLightDiffuse;
        D3DXHANDLE HandleLightSpecular;
        D3DXHANDLE HandleLightRadius;

        int i = 0;

        for (auto iter = LightList.begin(); iter != LightList.end(); iter++)
        {
            HandleLight = effect->GetParameterElement("lights", i++);

            HandleLightPos = effect->GetParameterByName(HandleLight, "pos");
            HandleLightAmbient = effect->GetParameterByName(HandleLight, "ambient");
            HandleLightDiffuse = effect->GetParameterByName(HandleLight, "diffuse");
            HandleLightSpecular = effect->GetParameterByName(HandleLight, "specular");
            HandleLightRadius = effect->GetParameterByName(HandleLight, "radius");

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

            effect->SetValue(HandleLightPos, Pos, sizeof(Pos));
            effect->SetValue(HandleLightAmbient, LightAmbient, sizeof(LightAmbient));
            effect->SetValue(HandleLightDiffuse, LightDiffuse, sizeof(LightDiffuse));
            effect->SetValue(HandleLightSpecular, LightSpecular, sizeof(LightSpecular));
            effect->SetFloat(HandleLightRadius, (*iter)->GetRadius());
        }*/
    }

    void PhongShader::Render(int index, GameObject* parent)
    {
        if (effect == nullptr)
            effect = ResourceMgr->LoadEffect(address);

        SetMaterial(parent);
        SetEnvironment(parent);

        auto Mesh = (StaticMesh*)parent->GetComponent("StaticMesh");

        if (Mesh)
            effect->SetTexture("colorMapTexture", ResourceMgr->LoadTexture(Mesh->GetTextures()[index]));

        effect->CommitChanges();
    }
    
    void PhongShader::Render(int index, std::vector<LPDIRECT3DTEXTURE9> textures, GameObject* parent)
    {
        if (effect == nullptr)
            effect = ResourceMgr->LoadEffect(address);
        
        SetMaterial(parent);
        SetEnvironment(parent);
        
        auto AnimatedMesh = (SkinnedMesh*)parent->GetComponent("SkinnedMesh");

        if (AnimatedMesh != nullptr)
            effect->SetTexture("colorMapTexture", textures[index]);

        effect->CommitChanges();
    }
}


