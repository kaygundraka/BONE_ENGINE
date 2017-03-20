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
    }

    PhongShader::~PhongShader()
    {

    }
    
    void PhongShader::Render(int index, GameObject* parent)
    {
        auto PhongEffect = ResourceMgr->LoadEffect(address);
        
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
        
        PhongEffect->SetValue("material.ambient", MatAmbient, sizeof(MatAmbient));
        PhongEffect->SetValue("material.diffuse", MatDiffuse, sizeof(MatDiffuse));
        PhongEffect->SetValue("material.emissive", MatEmissive, sizeof(MatEmissive));
        PhongEffect->SetValue("material.specular", MatSpecular, sizeof(MatSpecular));
        PhongEffect->SetFloat("material.shininess", material->GetShininess());
        
        Matrix MatWorld = ((Transform3D*)parent->transform3D)->GetTransform();

        auto Mesh = (StaticMesh*)parent->GetComponent("StaticMesh");

        if (Mesh != nullptr)
        {
            auto CurScene = SceneMgr->CurrentScene();

            auto LightList = CurScene->GetPointLights();

            auto CurCameraObject = (GameObject*)SceneMgr->CurrentScene()->GetCurrentCamera();
            auto CurCameraComponent = (Camera*)CurCameraObject->GetComponent("Camera");
            auto CurCameraTransform = (Transform3D*)CurCameraObject->GetComponent("Transform3D");

            int LightNumber = LightList.size();

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
            
            PhongEffect->SetMatrix("worldMatrix", &MatWorld);
            PhongEffect->SetMatrix("worldInverseTransposeMatrix", &MatWorld);
            PhongEffect->SetMatrix("worldViewProjectionMatrix", &MatViewProj);

            PhongEffect->SetValue("cameraPos", &CameraPos, sizeof(CameraPos));
            PhongEffect->SetValue("globalAmbient", &GlobalAmbient, sizeof(GlobalAmbient));
            PhongEffect->SetValue("numLights", &LightNumber, sizeof(LightNumber));

            PhongEffect->SetTexture("colorMapTexture", ResourceMgr->LoadTexture(Mesh->GetTextures()[0]));
                        
            D3DXHANDLE HandleLight;
            D3DXHANDLE HandleLightPos;
            D3DXHANDLE HandleLightAmbient;
            D3DXHANDLE HandleLightDiffuse;
            D3DXHANDLE HandleLightSpecular;
            D3DXHANDLE HandleLightRadius;

            int i = 0;

            for (auto iter = LightList.begin(); iter != LightList.end(); iter++)
            {
                HandleLight = PhongEffect->GetParameterElement("lights", i++);

                HandleLightPos = PhongEffect->GetParameterByName(HandleLight, "pos");
                HandleLightAmbient = PhongEffect->GetParameterByName(HandleLight, "ambient");
                HandleLightDiffuse = PhongEffect->GetParameterByName(HandleLight, "diffuse");
                HandleLightSpecular = PhongEffect->GetParameterByName(HandleLight, "specular");
                HandleLightRadius = PhongEffect->GetParameterByName(HandleLight, "radius");

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

                PhongEffect->SetValue(HandleLightPos, Pos, sizeof(Pos));
                PhongEffect->SetValue(HandleLightAmbient, LightAmbient, sizeof(LightAmbient));
                PhongEffect->SetValue(HandleLightDiffuse, LightDiffuse, sizeof(LightDiffuse));
                PhongEffect->SetValue(HandleLightSpecular, LightSpecular, sizeof(LightSpecular));
                PhongEffect->SetFloat(HandleLightRadius, (*iter)->GetRadius());
            }

            PhongEffect->CommitChanges();
        }
    }
}


