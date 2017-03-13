#include "Common.h"
#include "SceneManager.h"
#include "GameObject.h"
#include "Transform3D.h"
#include "Transform2D.h"
#include "TrailRenderer.h"
#include "StaticMesh.h"
#include "SkinnedMesh.h"
#include "ScreenSprite.h"
#include "IShader.h"
#include "SpriteBillBoard.h"
#include "BillBoard.h"
#include "ScreenImage.h"
#include "ScreenButton.h"
#include "PhongShader.h"
#include "Material.h"
#include "PointLight.h"

namespace BONE_GRAPHICS
{
    GameObject::GameObject()
    {
        isActive = true;
        isStatic = false;
        name = "";

        parent = nullptr;
        std::vector<GameObject*> Child;
    }

    GameObject::~GameObject()
    {
        for (auto Iter = childs.begin(); Iter != childs.end();)
        {
            GameObject* Temp = (*Iter);
            Iter = childs.erase(Iter);
            delete Temp;
        }

        for (auto Iter = components.begin(); Iter != components.end();)
        {
            Component* Temp = (*Iter);
            Iter = components.erase(Iter);
            delete Temp;
        }
    }

	void GameObject::SetStatic(bool isStatic)
	{
		this->isStatic = isStatic;
	}

	bool GameObject::GetStatic()
	{
		return isStatic;
	}

	void GameObject::Awake()
	{

	}

	void GameObject::SetActive(bool isActive)
	{
		this->isActive = isActive;

		if (isActive)
			Awake();

		for (auto Iter = childs.begin(); Iter != childs.end(); Iter++)
		{
			(*Iter)->SetActive(isActive);

			if (isActive)
				(*Iter)->Awake();
		}
	}

	bool GameObject::GetActive()
	{
		return isActive;
	}

	void GameObject::Init()
	{
	}

	void GameObject::SetPriority(int index)
	{
		priorty = index;
	}

	int GameObject::GetPriority()
	{
		return priorty;
	}

	void GameObject::SetTag(std::string tag)
	{
		this->tag = tag;
	}

    void GameObject::SetName(std::string name)
    {
        this->name = name;
    }
    
    std::string GameObject::GetName()
    {
        return name;
    }

	Component* GameObject::GetComponent(std::string typeName)
	{
		for (auto Iter = components.begin(); Iter != components.end(); Iter++)
			if ((*Iter)->GetTypeName() == typeName)
				return (*Iter);
		
		return nullptr;
	}

    std::vector<Component*> GameObject::GetComponents()
    {
        return components;
    }

	void GameObject::LoadContents()
	{
		for (auto Iter = components.begin(); Iter != components.end(); Iter++)
			(*Iter)->LoadContent();
	}

	bool GameObject::AddComponent(Component* component)
	{
		for (auto Iter = components.begin(); Iter != components.end(); Iter++)
			if ((*Iter)->GetTypeName() == component->GetTypeName())
				return false;
		
        if (component->GetTypeName() == "Transform3D")
            transform3D = (Transform3D*)component;
        else if (component->GetTypeName() == "Transform2D")
            transform2D = (Transform2D*)component;

		components.push_back(component);

		return true;
	}

	void GameObject::Render()
	{
		IShader* Shader = ((IShader*)GetComponent("IShader"));
        static PhongShader phongShader;

        if (Shader == nullptr)
            Shader = &phongShader;

        UINT numPasses = 0;

        Shader->GetShader()->Begin(&numPasses, 0);
        {
            for (UINT i = 0; i < numPasses; i++)
            {
                Shader->GetShader()->BeginPass(i);
                {
                    if (GetComponent("StaticMesh") != nullptr)
                        ((StaticMesh*)GetComponent("StaticMesh"))->Render(Shader, this);

                    if (GetComponent("BillBoard") != nullptr)
                        ((BillBoard*)GetComponent("BillBoard"))->Render(Shader, this);

                    if (GetComponent("SpriteBillBoard") != nullptr)
                        ((SpriteBillBoard*)GetComponent("SpriteBillBoard"))->Render(Shader, this);

                    if (GetComponent("TrailRenderer") != nullptr)
                        ((TrailRenderer*)GetComponent("TrailRenderer"))->Render(Shader);

                    if (GetComponent("SkinnedMesh") != nullptr)
                        ((SkinnedMesh*)GetComponent("SkinnedMesh"))->Render(Shader, this);
                }
                Shader->GetShader()->End();
            }

            Shader->GetShader()->EndPass();
        }

        if (GetComponent("ScreenSprite") != nullptr)
			((ScreenSprite*)GetComponent("ScreenSprite"))->Render(this);

		if (GetComponent("ScreenImage") != nullptr)
			((ScreenImage*)GetComponent("ScreenImage"))->Render(this);

		if (GetComponent("ScreenButton") != nullptr)
			((ScreenButton*)GetComponent("ScreenButton"))->Render(this);
	}

	void GameObject::AttachParent(GameObject* parent)
	{
		this->parent = parent;
		
		if (((Transform3D*)GetComponent("Transform3D")) != nullptr)
			((Transform3D*)GetComponent("Transform3D"))->AttachObject(this->parent);

		if (((Transform2D*)GetComponent("Transform2D")) != nullptr)
			((Transform2D*)GetComponent("Transform2D"))->AttachObject(this->parent);

        this->parent->AttachChild(this);
	}

	void GameObject::DetachParent()
	{
        if (parent != nullptr)
    		parent->DetachChild(this);

		parent = nullptr;
	}

	GameObject* GameObject::GetParent()
	{
		return parent;
	}

	void GameObject::AttachChild(GameObject* child)
	{
		if (child != nullptr)
		{
			if(child->GetParent() == nullptr)
                child->AttachParent(this);

			bool Flag = false;

			for each (GameObject* var in childs)
			{
				if (var == child)
				{
					Flag = true;
					break;
				}
			}

			if(Flag == false)
				childs.push_back(child);
		}
	}

	void GameObject::DetachChild(GameObject* child)
	{
		for (auto Iter = childs.begin(); Iter != childs.end(); Iter++)
		{
			if ((*Iter) == child)
			{
                childs.erase(Iter);
				child->DetachParent();
				break;
			}
		}
	}

    std::vector<GameObject*> GameObject::GetChileds()
    {
        return this->childs;
    }

	void GameObject::DetachChildByTag(std::string tag)
	{
		for (auto Iter = childs.begin(); Iter != childs.end(); Iter++)
		{
			if ((*Iter)->Tag() == tag)
			{
				(*Iter)->DetachParent();
				break;
			}
		}
	}
	
	GameObject* GameObject::FindChildByTag(std::string tag)
	{
		for (auto Iter = childs.begin(); Iter != childs.end(); Iter++)
			if ((*Iter)->Tag() == tag)
				return *Iter;

        return nullptr;
	}

	void GameObject::AttachTag(std::string tag)
	{
		this->tag = tag;
	}
	
	void GameObject::DetachTag()
	{
		tag = "";
	}

	std::string GameObject::Tag()
	{
		return tag;
	}

    void GameObject::SaveInMaps()
    {
        std::string FullPath;
        
        if (!ResourceMgr->ExistFile(SceneMgr->CurrentScene()->GetName() + ".json", &FullPath))
            return;

        std::ifstream file(FullPath);
        json j;
        file >> j;

        j[name]["PrefabName"] = prefab;

        j[name]["Position"] = { 
            ((Transform3D*)(GetComponent("Transform3D")))->GetPosition().x,
            ((Transform3D*)(GetComponent("Transform3D")))->GetPosition().y,
            ((Transform3D*)(GetComponent("Transform3D")))->GetPosition().z
        };

        j[name]["Rotation"] = {
            ((Transform3D*)(GetComponent("Transform3D")))->GetRotateAngle().x,
            ((Transform3D*)(GetComponent("Transform3D")))->GetRotateAngle().y,
            ((Transform3D*)(GetComponent("Transform3D")))->GetRotateAngle().z
        };

        j[name]["Scale"] = {
            ((Transform3D*)(GetComponent("Transform3D")))->GetScale().x,
            ((Transform3D*)(GetComponent("Transform3D")))->GetScale().y,
            ((Transform3D*)(GetComponent("Transform3D")))->GetScale().z
        };

        file.close();
    }

    void GameObject::SavePrefab()
    {
        json j;

        j["Tag"] = tag;
        j["Priority"] = priorty;
        j["IsActive"] = isActive;
        j["IsStatic"] = isStatic;

        for (int i = 0; i < components.size(); i++)
        {
            std::string TypeName = components[i]->GetTypeName();

            if (TypeName == "Transform3D")
            {
                auto Position = ((Transform3D*)(components[i]))->GetPosition();
                auto Rotation = ((Transform3D*)(components[i]))->GetRotateAngle();
                auto Scale = ((Transform3D*)(components[i]))->GetScale();
                auto ForwardVector = ((Transform3D*)(components[i]))->GetForward();

                j["Transform3D"]["Position"] = { Position.x, Position.y, Position.z };
                j["Transform3D"]["Rotation"] = { Rotation.x, Rotation.y, Rotation.z };
                j["Transform3D"]["Scale"] = { Scale.x, Scale.y, Scale.z };
                j["Transform3D"]["ForwardVector"] = { ForwardVector.x, ForwardVector.y, ForwardVector.z };
            }
            else if (TypeName == "Camera")
            {
                j["Camera"]["ID"] = ((Camera*)(components[i]))->GetID();
                j["Camera"]["Type"] = ((Camera*)(components[i]))->GetProjectionType();
                j["Camera"]["Width"] = ((Camera*)(components[i]))->GetScreenWidth();
                j["Camera"]["Height"] = ((Camera*)(components[i]))->GetScreenHeight();
                j["Camera"]["farDist"] = ((Camera*)(components[i]))->GetFarDistance();
                j["Camera"]["nearDist"] = ((Camera*)(components[i]))->GetNearDistance();
                j["Camera"]["fov"] = ((Camera*)(components[i]))->GetFov();
                j["Camera"]["UpVector"] = {
                    ((Camera*)(components[i]))->GetCameraUp().x,
                    ((Camera*)(components[i]))->GetCameraUp().y,
                    ((Camera*)(components[i]))->GetCameraUp().z
                };
            }
            else if (TypeName == "Material")
            {
                auto Ambient = ((Material*)(components[i]))->GetAmbient();
                auto Diffuse = ((Material*)(components[i]))->GetDiffuse();
                auto Emissive = ((Material*)(components[i]))->GetEmissive();
                auto Specular = ((Material*)(components[i]))->GetSpecular();
                auto Shininess = ((Material*)(components[i]))->GetShininess();

                j["Material"]["Ambient"] = { Ambient.r, Ambient.g, Ambient.b, Ambient.a };
                j["Material"]["Diffuse"] = { Diffuse.r, Diffuse.g, Diffuse.b, Diffuse.a };
                j["Material"]["Emissive"] = { Emissive.r, Emissive.g, Emissive.b, Emissive.a };
                j["Material"]["Specular"] = { Specular.r, Specular.g, Specular.b, Specular.a };
                j["Material"]["Shininess"] = Shininess;
            }
            else if (TypeName == "StaticMesh")
            {
                j["StaticMesh"]["FileName"] = ((StaticMesh*)(components[i]))->GetFileAddress();
            }
            else if (TypeName == "SkinnedMesh")
            {
                j["SkinnedMesh"]["FileName"] = ((SkinnedMesh*)(components[i]))->GetFileAddress();

                auto animationSet = ((SkinnedMesh*)(components[i]))->GetAnmimationSet();

                for each(auto var in animationSet)
                {
                    j["SkinnedMesh"]["AnimationSet"][var.first]["Speed"] = var.second.AnimationSpeed;
                    j["SkinnedMesh"]["AnimationSet"][var.first]["Index"] = var.second.Vertex_Index;
                }
            }
        }

        std::string fullPath = ".\\Engine\\Prefabs\\" + prefab + ".json";
        std::ofstream o(fullPath);
        o << std::setw(4) << j << std::endl;
        o.close();
    }

    void GameObject::LoadPrefab()
    {
        std::string FullPath;

        if (!ResourceMgr->ExistFile(prefab + ".json", &FullPath))
            return;

        std::ifstream file(FullPath);
        json j;
        file >> j;

        for (json::iterator it = j.begin(); it != j.end(); ++it) 
        {
            std::string TypeName = it.key();

            if (TypeName == "Transform3D")
            {
                auto Position = j["Transform3D"]["Position"].get<std::vector<double>>();
                auto Rotation = j["Transform3D"]["Rotation"].get<std::vector<double>>();
                auto Scale = j["Transform3D"]["Scale"].get<std::vector<double>>();
                auto ForwardVector = j["Transform3D"]["ForwardVector"].get<std::vector<double>>();

                Transform3D* tr = (Transform3D*)GetComponent("Transform3D");

                if (tr == nullptr)
                {
                    tr = new Transform3D();
                    AddComponent(tr);
                
                    tr->SetPosition(Position[0], Position[1], Position[2]);
                    tr->SetRotate(Rotation[0], Rotation[1], Rotation[2]);
                    tr->SetScale(Scale[0], Scale[1], Scale[2]);
                    tr->SetForward(ForwardVector[0], ForwardVector[1], ForwardVector[2]);
                }
            }
            else if (TypeName == "Camera")
            {
                auto UpVector = j["Camera"]["UpVector"].get<std::vector<double>>();
                
                Camera* camera = (Camera*)GetComponent("Camera");

                if (camera == nullptr)
                {
                    camera = new Camera(
                        j["Camera"]["ID"],
                        j["Camera"]["Type"],
                        Vector3(UpVector[0], UpVector[1], UpVector[2]),
                        j["Camera"]["Width"],
                        j["Camera"]["Height"],
                        j["Camera"]["farDist"],
                        j["Camera"]["nearDist"],
                        j["Camera"]["fov"]
                    );

                    AddComponent(camera);
                }
            }
            else if (TypeName == "Material")
            {
                auto Ambient = j["Material"]["Ambient"].get<std::vector<double>>();
                auto Diffuse = j["Material"]["Diffuse"].get<std::vector<double>>();
                auto Emissive = j["Material"]["Emissive"].get<std::vector<double>>();
                auto Specular = j["Material"]["Specular"].get<std::vector<double>>();

                Material* material = (Material*)GetComponent("Material");

                if (material == nullptr)
                {
                    material = new Material();

                    material->SetAmbient(Ambient[0], Ambient[1], Ambient[2], Ambient[3]);
                    material->SetDiffuse(Diffuse[0], Diffuse[1], Diffuse[2], Diffuse[3]);
                    material->SetEmissive(Emissive[0], Emissive[1], Emissive[2], Emissive[3]);
                    material->SetSpecular(Specular[0], Specular[1], Specular[2], Specular[3]);
                    material->SetShininess(j["Material"]["Shininess"]);
                    
                    AddComponent(material);
                }
            }
            else if (TypeName == "StaticMesh")
            {
                StaticMesh* staticMesh = (StaticMesh*)GetComponent("StaticMesh");

                if (staticMesh == nullptr)
                {
                    staticMesh = new StaticMesh();

                    staticMesh->SetFileAddress(j["StaticMesh"]["FileName"]);

                    AddComponent(staticMesh);
                }
            }
            else if (TypeName == "SkinnedMesh")
            {
                auto Attributes = j["SkinnedMesh"].get<std::vector<std::string>>();

                for (auto iter = Attributes.begin(); iter != Attributes.end(); iter++)
                {
                    if (*iter == "FileName")
                    {
                        SkinnedMesh* skinnedMesh = (SkinnedMesh*)GetComponent("SkinnedMesh");

                        if (skinnedMesh == nullptr)
                        {
                            skinnedMesh = new SkinnedMesh();

                            skinnedMesh->SetFileAddress(j["SkinnedMesh"]["FileName"]);

                            AddComponent(skinnedMesh);
                        }
                    }
                    else if (*iter == "AnimationSet")
                    {
                        /*for 
                        auto animationSet = ((SkinnedMesh*)(components[i]))->GetAnmimationSet();

                        for each(auto var in animationSet)
                        {
                            j["SkinnedMesh"]["AnimationSet"][var.first]["Speed"] = var.second.AnimationSpeed;
                            j["SkinnedMesh"]["AnimationSet"][var.first]["Index"] = var.second.Vertex_Index;
                        }*/
                    }
                }
            }
        }

        file.close();
    }

    void GameObject::SetPrfabName(std::string prefabName)
    {
        this->prefab = prefabName;
    }
    
    std::string GameObject::GetPrfabName()
    {
        return prefab;
    }

	void GameObject::Reference()
	{
		// 비어있음
	}
	
	void GameObject::Update()
	{
		// 비어있음
	}
	
	void GameObject::LateUpdate()
	{
		// 비어있음
	}

	void GameObject::LateRender()
	{
		// 비어있음
	}

    void GameObject::OnCollisionEnter(GameObject* otherObject)
    {

    }

    void GameObject::OnCollisionStay(GameObject* otherObject)
    {

    }
    
    void GameObject::OnCollisionLeave(GameObject* otherObject)
    {

    }
}