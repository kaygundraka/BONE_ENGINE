#include "Common.h"
#include "PointLight.h"
#include "Scene.h"
#include "Transform3D.h"
#include "SceneManager.h"
#include "BillBoard.h"

namespace BONE_GRAPHICS
{
    PointLight::PointLight()
    {
        radius = 50;
        status = false;
        IsInit = false;
        tr = nullptr;
        showIcon = false;
    }

    PointLight::~PointLight()
    {

    }

    void PointLight::Init()
    {
        SetTag("PointLight");
        SetPriority(100);

        BillBoard* billBoard = new BillBoard();
        Rect rect;
        rect.LeftTop = Vec2(0, 0);
        rect.RightBottom = Vec2(512, 512);
        billBoard->SetOriginRect(rect);
        billBoard->SetRectSize(20, 20);
        billBoard->SetTarget(SceneMgr->CurrentScene()->GetCurrentCamera());
        billBoard->SetTexture("PointLight.png", 512, 512);
        billBoard->SetRenderMode(BillBoard::RENDER_ALPHA);
        this->AddComponent(billBoard);

        this->lightViewSize.x = 1024;
        this->lightViewSize.y = 1024;
        this->SetAmbient(1.0f, 1.0f, 1.0f, 1.0f);
        this->SetDiffuse(1.0f, 1.0f, 1.0f, 1.0f);
        this->SetSpecular(0.5f, 0.5f, 0.5f, 0.5f);
        this->radius = 50;
        this->shadowAimPos = Vec3(0, 0, 0);

        this->SetLight(false);

        Transform3D* tr = new Transform3D();
        this->AddComponent(tr);
        this->tr = (Transform3D*)transform3D;

        ((BillBoard*)GetComponent("BillBoard"))->SetShow(false);
        this->SetPipeLine(GameObject::DIRECT_DEFAULT);

        IsInit = true;
    }

    void PointLight::SetShadowViewSize(Vec2 size)
    {
        lightViewSize = size;
    }
    
    Vec2 PointLight::GetShadowViewSize()
    {
        return lightViewSize;
    }

    void PointLight::Reference()
    {
    }

    void PointLight::SetIcon(bool show)
    {
        showIcon = show;

        ((BillBoard*)GetComponent("BillBoard"))->SetShow(show);
    }
    
    bool PointLight::ShowIcon()
    {
        return showIcon;
    }

    void PointLight::SaveInMaps()
    {
        bool isNewMap = false;
        std::string FullPath;

        if (!ResourceMgr->ExistFile(SceneMgr->CurrentScene()->GetName() + ".json", &FullPath))
            isNewMap = true;

        json j;

        std::ifstream file(FullPath);

        if (!isNewMap)
            file >> j;

        j["Light"][name]["Position"] = {
            ((Transform3D*)transform3D)->GetPosition().x,
            ((Transform3D*)transform3D)->GetPosition().y,
            ((Transform3D*)transform3D)->GetPosition().z
        };

        j["Light"][name]["Ambient"] = {
            ambient.r,
            ambient.g,
            ambient.b,
            ambient.a
        };

        j["Light"][name]["Diffuse"] = {
            diffuse.r,
            diffuse.g,
            diffuse.b,
            diffuse.a
        };

        j["Light"][name]["Specular"] = {
            specular.r,
            specular.g,
            specular.b,
            specular.a
        };

        j["Light"][name]["Radius"] = radius;

        j["Light"][name]["ShadowTarget"] = {
            shadowAimPos.x,
            shadowAimPos.y,
            shadowAimPos.z
        };

        j["Light"][name]["Status"] = status;

        if (!isNewMap)
            file.close();

        std::string fullPath = ".\\Engine\\Maps\\" + SceneMgr->CurrentScene()->GetName() + ".json";
        std::ofstream o(fullPath);
        o << std::setw(4) << j << std::endl;
        o.close();
    }

    void PointLight::SetLight(bool status) {
        this->status = status;
    }

    bool PointLight::IsOn()
    {
        return status;
    }

    void PointLight::SetAmbient(float r, float g, float b, float a)
    {
        ambient.r = r;
        ambient.g = g;
        ambient.b = b;
        ambient.a = a;
    }

    void PointLight::SetDiffuse(float r, float g, float b, float a)
    {
        diffuse.r = r;
        diffuse.g = g;
        diffuse.b = b;
        diffuse.a = a;
    }
    
    void PointLight::SetSpecular(float r, float g, float b, float a)
    {
        specular.r = r;
        specular.g = g;
        specular.b = b;
        specular.a = a;
    }
    
    void PointLight::SetRadius(float value)
    {
        radius = value;
    }

    void PointLight::SetAmbient(RGBA color)
    {
        ambient = color;
    }

    void PointLight::SetDiffuse(RGBA color)
    {
        diffuse = color;
    }
    
    void PointLight::SetSpecular(RGBA color)
    {
        specular = color;
    }

    void PointLight::SetPosition(Vec3 pos)
    {
        tr->SetPosition(pos);
    }

    RGBA PointLight::GetAmbient()
    {
        return ambient;
    }

    RGBA PointLight::GetDiffuse()
    {
        return diffuse;
    }
    
    RGBA PointLight::GetSpecular()
    {
        return specular;
    }

    Vec3 PointLight::GetPosition()
    {
        return tr->GetPosition();
    }
    
    float PointLight::GetRadius()
    {
        return radius;
    }

    void PointLight::SetShadowAimPos(Vec3 aim)
    {
        shadowAimPos = aim;
    }
    
    Vec3 PointLight::GetShadowAimPos()
    {
        return shadowAimPos;
    }
}