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
#include "Script.h"
#include "RuntimeCompiler.h"
#include "Rp3dCollision.h"
#include "Rp3dRigidBody.h"

namespace BONE_GRAPHICS
{
    GameObject::GameObject()
    {
        isActive = true;
        isStatic = false;
        pipeLine = PIPE_LINE::DEFAULT_SHADER;
        isEditorLock = false;
        name = "";
        enableScript = true;

        transform3D = nullptr;

        parent = nullptr;
        std::vector<GameObject*> Child;
    }

    GameObject::~GameObject()
    {
        for (auto Iter = childs.begin(); Iter != childs.end();)
        {
            GameObject* Temp = (*Iter);
            Iter = childs.erase(Iter);

            if (SceneMgr->CurrentScene() != nullptr)
                SceneMgr->CurrentScene()->Destroy(Temp);
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
        //static PhongShader phongShader;

        //if (pipeLine == PIPE_LINE::CUSTOM_SHADER)
        //    Shader = &phongShader;

        if (Shader != nullptr)
        {
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

                        if (GetComponent("SkinnedMesh") != nullptr)
                            ((SkinnedMesh*)GetComponent("SkinnedMesh"))->Render(Shader, this);

                        if (GetComponent("TrailRenderer") != nullptr)
                            ((TrailRenderer*)GetComponent("TrailRenderer"))->Render(Shader);
                    }
                    Shader->GetShader()->End();
                }

                Shader->GetShader()->EndPass();
            }
        }
        else
        {
            if (GetComponent("SkinnedMesh") != nullptr)
                ((SkinnedMesh*)GetComponent("SkinnedMesh"))->Render((IShader*)nullptr, this);

            if (GetComponent("StaticMesh") != nullptr)
                ((StaticMesh*)GetComponent("StaticMesh"))->Render(nullptr, this);

            if (GetComponent("BillBoard") != nullptr)
                ((BillBoard*)GetComponent("BillBoard"))->Render(nullptr, this);

            if (GetComponent("SpriteBillBoard") != nullptr)
                ((SpriteBillBoard*)GetComponent("SpriteBillBoard"))->Render(nullptr, this);

            if (GetComponent("TrailRenderer") != nullptr)
                ((TrailRenderer*)GetComponent("TrailRenderer"))->Render(nullptr);
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
        bool isNewMap = false;
        std::string FullPath;
        
        if (!ResourceMgr->ExistFile(SceneMgr->CurrentScene()->GetName() + ".json", &FullPath))
            isNewMap = true;

        int TreeDepth = 0;

        GameObject* pt = parent;

        while (pt != nullptr)
        {
            TreeDepth++;

            pt = pt->GetParent();
        }

        json j;

        if (!isNewMap)
        {
            std::ifstream file(FullPath);
            file >> j;

            if (parent == nullptr)
                j["GameObject"][name]["IsChild"] = false;
            else
                j["GameObject"][name]["IsChild"] = true;

            j["GameObject"][name]["PrefabName"] = prefab;
            j["GameObject"][name]["TreeDepth"] = TreeDepth;

            j["GameObject"][name]["Position"] = {
                ((Transform3D*)(GetComponent("Transform3D")))->GetPosition().x,
                ((Transform3D*)(GetComponent("Transform3D")))->GetPosition().y,
                ((Transform3D*)(GetComponent("Transform3D")))->GetPosition().z
            };

            j["GameObject"][name]["Rotation"] = {
                ((Transform3D*)(GetComponent("Transform3D")))->GetRotateAngle().x,
                ((Transform3D*)(GetComponent("Transform3D")))->GetRotateAngle().y,
                ((Transform3D*)(GetComponent("Transform3D")))->GetRotateAngle().z
            };

            j["GameObject"][name]["Scale"] = {
                ((Transform3D*)(GetComponent("Transform3D")))->GetScale().x,
                ((Transform3D*)(GetComponent("Transform3D")))->GetScale().y,
                ((Transform3D*)(GetComponent("Transform3D")))->GetScale().z
            };

            file.close();
        }
        else
        {
            if (parent == nullptr)
                j["GameObject"][name]["IsChild"] = false;
            else
                j["GameObject"][name]["IsChild"] = true;

            j["GameObject"][name]["PrefabName"] = prefab;
            j["GameObject"][name]["TreeDepth"] = TreeDepth;

            j["GameObject"][name]["Position"] = {
                ((Transform3D*)(GetComponent("Transform3D")))->GetPosition().x,
                ((Transform3D*)(GetComponent("Transform3D")))->GetPosition().y,
                ((Transform3D*)(GetComponent("Transform3D")))->GetPosition().z
            };

            j["GameObject"][name]["Rotation"] = {
                ((Transform3D*)(GetComponent("Transform3D")))->GetRotateAngle().x,
                ((Transform3D*)(GetComponent("Transform3D")))->GetRotateAngle().y,
                ((Transform3D*)(GetComponent("Transform3D")))->GetRotateAngle().z
            };

            j["GameObject"][name]["Scale"] = {
                ((Transform3D*)(GetComponent("Transform3D")))->GetScale().x,
                ((Transform3D*)(GetComponent("Transform3D")))->GetScale().y,
                ((Transform3D*)(GetComponent("Transform3D")))->GetScale().z
            };
        }

        std::string fullPath = ".\\Engine\\Maps\\" + SceneMgr->CurrentScene()->GetName() + ".json";
        std::ofstream o(fullPath);
        o << std::setw(4) << j << std::endl;
        o.close();
    }

    void GameObject::SavePrefab()
    {
        json j;

        j["Tag"] = tag;
        j["Priority"] = priorty;
        j["IsActive"] = isActive;
        j["IsStatic"] = isStatic;
        j["IsEditorLock"] = isEditorLock;
        
        for (auto iter = childs.begin(); iter != childs.end(); iter++)
            j["ChildObjects"].push_back((*iter)->GetPrfabName());

        for each(auto item in components)
            if (item->IsScript())
                j["8.Script"].push_back(item->GetTypeName());
         
        for (int i = 0; i < components.size(); i++)
        {
            std::string TypeName = components[i]->GetTypeName();

            if (TypeName == "Transform3D")
            {
                auto Position = ((Transform3D*)(components[i]))->GetPosition();
                auto Rotation = ((Transform3D*)(components[i]))->GetRotateAngle();
                auto Scale = ((Transform3D*)(components[i]))->GetScale();

                j["1.Transform3D"]["Position"] = { Position.x, Position.y, Position.z };
                j["1.Transform3D"]["Rotation"] = { Rotation.x, Rotation.y, Rotation.z };
                j["1.Transform3D"]["Scale"] = { Scale.x, Scale.y, Scale.z };
            }
            else if (TypeName == "Camera")
            {
                j["2.Camera"]["ID"] = ((Camera*)(components[i]))->GetID();
                j["2.Camera"]["Type"] = ((Camera*)(components[i]))->GetProjectionType();
                j["2.Camera"]["Width"] = ((Camera*)(components[i]))->GetScreenWidth();
                j["2.Camera"]["Height"] = ((Camera*)(components[i]))->GetScreenHeight();
                j["2.Camera"]["farDist"] = ((Camera*)(components[i]))->GetFarDistance();
                j["2.Camera"]["nearDist"] = ((Camera*)(components[i]))->GetNearDistance();
                j["2.Camera"]["fov"] = ((Camera*)(components[i]))->GetFov();
                j["2.Camera"]["UpVector"] = {
                    ((Camera*)(components[i]))->GetCameraUp().x,
                    ((Camera*)(components[i]))->GetCameraUp().y,
                    ((Camera*)(components[i]))->GetCameraUp().z
                };
            }
            else if (TypeName == "Collision")
            {
                auto Type = ((Collision*)(components[i]))->GetCollisionType();
                j["3.Collision"]["Type"] = (int)Type;

                if (Type == Collision::COLL_BOX)
                {
                    j["3.Collision"]["HalfExtens"] = {
                        ((Collision*)(components[i]))->GetHalfExtens().x,
                        ((Collision*)(components[i]))->GetHalfExtens().y,
                        ((Collision*)(components[i]))->GetHalfExtens().z
                    };
                }
                else if (Type == Collision::COLL_SPHERE)
                    j["3.Collision"]["Radius"] = ((Collision*)(components[i]))->GetRadius();
                else
                {
                    j["3.Collision"]["Radius"] = ((Collision*)(components[i]))->GetRadius();
                    j["3.Collision"]["Height"] = ((Collision*)(components[i]))->GetHeight();
                }
            }
            else if (TypeName == "RigidBody")
            {
                j["4.RigidBody"]["Type"] = (int)(((RigidBody*)(components[i]))->GetType());
                j["4.RigidBody"]["Mass"] = ((RigidBody*)(components[i]))->GetMass();
                j["4.RigidBody"]["EnableGravity"] = ((RigidBody*)(components[i]))->IsEnableGravity();
                j["4.RigidBody"]["Bounciness"] = ((RigidBody*)(components[i]))->GetBounciness();
                j["4.RigidBody"]["FricitionCoefficient"] = ((RigidBody*)(components[i]))->GetFrictionCoefficient();
                j["4.RigidBody"]["IsAllowedToSleep"] = ((RigidBody*)(components[i]))->GetIsAllowedToSleep();
            }
            else if (TypeName == "Material")
            {
                auto Ambient = ((Material*)(components[i]))->GetAmbient();
                auto Diffuse = ((Material*)(components[i]))->GetDiffuse();
                auto Emissive = ((Material*)(components[i]))->GetEmissive();
                auto Specular = ((Material*)(components[i]))->GetSpecular();
                auto Shininess = ((Material*)(components[i]))->GetShininess();

                j["5.Material"]["Ambient"] = { Ambient.r, Ambient.g, Ambient.b, Ambient.a };
                j["5.Material"]["Diffuse"] = { Diffuse.r, Diffuse.g, Diffuse.b, Diffuse.a };
                j["5.Material"]["Emissive"] = { Emissive.r, Emissive.g, Emissive.b, Emissive.a };
                j["5.Material"]["Specular"] = { Specular.r, Specular.g, Specular.b, Specular.a };
                j["5.Material"]["Shininess"] = Shininess;
            }
            else if (TypeName == "StaticMesh")
            {
                j["6.StaticMesh"]["FileName"] = ((StaticMesh*)(components[i]))->GetFile();
            }
            else if (TypeName == "SkinnedMesh")
            {
                j["7.SkinnedMesh"]["FileName"] = ((SkinnedMesh*)(components[i]))->GetFile();
            }
        }

        std::string fullPath = ".\\Engine\\Prefabs\\" + prefab + ".json";
        std::ofstream o(fullPath, ios::trunc);
        o << std::setw(4) << j << std::endl;
        o.close();
    }

    void GameObject::LoadPrefab()
    {
        std::string FullPath;

        SceneMgr->SetGameObject(this);

        if (!ResourceMgr->ExistFile(prefab + ".json", &FullPath))
            return;

        std::ifstream file(FullPath);
        json j;
        file >> j;

        for (json::iterator it = j.begin(); it != j.end(); ++it) 
        {
            std::string TypeName = it.key();

            if (TypeName == "Tag")
            {
                this->SetTag(j["Tag"].get<std::string>());

                if (Tag() != "EditorObject" && SceneMgr->CurrentScene()->IsEditorScene())
                    EnableScript(false);
            }
            else if (TypeName == "IsEditorLock")
            {
                this->isEditorLock = j["IsEditorLock"].get<bool>();
            }
            else if (TypeName == "Priority")
            {
                this->SetPriority(j["Priority"].get<int>());
            }
            else if (TypeName == "IsActive")
            {
                this->SetActive(j["IsActive"].get<bool>());
            }
            else if (TypeName == "IsStatic")
            {
                this->SetStatic(j["IsStatic"].get<bool>());
            }
            else if (TypeName == "ChildObjects")
            {
                auto Childs = j["ChildObjects"].get<std::vector<std::string>>();

                for (auto iter = Childs.begin(); iter != Childs.end(); iter++)
                {
                    auto ChildObject = new GameObject();
                    
                    ChildObject->SetPrfabName((*iter));
                    ChildObject->LoadPrefab();
                    ChildObject->AttachParent(this);
                    SceneMgr->CurrentScene()->AddObject(ChildObject, (*iter));
                }
            }
            else if (TypeName == "ParentObject")
            {
                auto Parent = SceneMgr->CurrentScene()->FindObjectByName(j["ParentObject"].get<std::string>());
            
                this->AttachParent(Parent);
            }
            else if (TypeName == "1.Transform3D")
            {
                auto Position = j["1.Transform3D"]["Position"].get<std::vector<double>>();
                auto Rotation = j["1.Transform3D"]["Rotation"].get<std::vector<double>>();
                auto Scale = j["1.Transform3D"]["Scale"].get<std::vector<double>>();

                Transform3D* tr = (Transform3D*)GetComponent("Transform3D");

                if (tr == nullptr)
                {
                    tr = new Transform3D();
                    AddComponent(tr);
                
                    tr->SetPosition(Position[0], Position[1], Position[2]);
                    tr->SetRotate(Rotation[0], Rotation[1], Rotation[2]);
                    tr->SetScale(Scale[0], Scale[1], Scale[2]);
                }
            }
            else if (TypeName == "2.Camera")
            {
                auto UpVector = j["2.Camera"]["UpVector"].get<std::vector<double>>();
                
                Camera* camera = (Camera*)GetComponent("Camera");

                if (camera == nullptr)
                {
                    camera = new Camera(
                        j["2.Camera"]["ID"],
                        j["2.Camera"]["Type"],
                        Vec3(UpVector[0], UpVector[1], UpVector[2]),
                        j["2.Camera"]["Width"],
                        j["2.Camera"]["Height"],
                        j["2.Camera"]["farDist"],
                        j["2.Camera"]["nearDist"],
                        j["2.Camera"]["fov"]
                    );

                    AddComponent(camera);
                }
            }
            else if (TypeName == "3.Collision")
            {
                Collision* collision = (Collision*)GetComponent("Collision");

                if (collision == nullptr)
                {
                    collision = new Collision(this);

                    Collision::COLLISION_TYPE Type = (Collision::COLLISION_TYPE)j["3.Collision"]["Type"].get<int>();

                    if (Type == Collision::COLL_BOX)
                    {
                        auto HalfExtens = j["3.Collision"]["HalfExtens"].get<std::vector<double>>();

                        collision->CreateBox(Vector3((float)HalfExtens[0], (float)HalfExtens[1], (float)HalfExtens[2]));
                    }
                    else if (Type == Collision::COLL_CAPSULE)
                    {
                        auto Radius = j["3.Collision"]["Radius"].get<double>();
                        auto Height = j["3.Collision"]["Height"].get<double>();

                        collision->CreateCapsule(Radius, Height);
                    }
                    else if (Type == Collision::COLL_CONE)
                    {
                        auto Radius = j["3.Collision"]["Radius"].get<double>();
                        auto Height = j["3.Collision"]["Height"].get<double>();

                        collision->CreateCone(Radius, Height);
                    }
                    else if (Type == Collision::COLL_CYLINDER)
                    {
                        auto Radius = j["3.Collision"]["Radius"].get<double>();
                        auto Height = j["3.Collision"]["Height"].get<double>();

                        collision->CreateCylinder(Radius, Height);
                    }
                    else if (Type == Collision::COLL_SPHERE)
                    {
                        auto Radius = j["3.Collision"]["Radius"].get<double>();
                        
                        collision->CreateSphere(Radius);
                    }

                    AddComponent(collision);
                }
            }
            else if (TypeName == "4.RigidBody")
            {
                RigidBody* rigidBody = (RigidBody*)GetComponent("RigidBody");

                if (rigidBody == nullptr)
                {
                    rigidBody = new RigidBody();

                    reactphysics3d::BodyType Type = (reactphysics3d::BodyType)j["4.RigidBody"]["Type"].get<int>();
                    auto Mass = j["4.RigidBody"]["Mass"].get<double>();
                    auto EnableGravity = j["4.RigidBody"]["EnableGravity"].get<bool>();
                    auto Bounciness = j["4.RigidBody"]["Bounciness"].get<double>();
                    auto FricitionCoefficient = j["4.RigidBody"]["FricitionCoefficient"].get<double>();
                    auto IsAllowedToSleep = j["4.RigidBody"]["IsAllowedToSleep"].get<bool>();

                    if (rigidBody->SetInfo(this, Mass))
                    {
                        rigidBody->EnableGravity(EnableGravity);
                        rigidBody->SetBounciness(Bounciness);
                        rigidBody->SetFrictionCoefficient(FricitionCoefficient);
                        rigidBody->SetIsAllowedToSleep(IsAllowedToSleep);
                        rigidBody->SetType(Type);

                        AddComponent(rigidBody);
                    }
                }
            }
            else if (TypeName == "5.Material")
            {
                auto Ambient = j["5.Material"]["Ambient"].get<std::vector<double>>();
                auto Diffuse = j["5.Material"]["Diffuse"].get<std::vector<double>>();
                auto Emissive = j["5.Material"]["Emissive"].get<std::vector<double>>();
                auto Specular = j["5.Material"]["Specular"].get<std::vector<double>>();

                Material* material = (Material*)GetComponent("Material");

                if (material == nullptr)
                {
                    material = new Material();

                    material->SetAmbient(Ambient[0], Ambient[1], Ambient[2], Ambient[3]);
                    material->SetDiffuse(Diffuse[0], Diffuse[1], Diffuse[2], Diffuse[3]);
                    material->SetEmissive(Emissive[0], Emissive[1], Emissive[2], Emissive[3]);
                    material->SetSpecular(Specular[0], Specular[1], Specular[2], Specular[3]);
                    material->SetShininess(j["5.Material"]["Shininess"]);
                    
                    AddComponent(material);
                }
            }
            else if (TypeName == "6.StaticMesh")
            {
                StaticMesh* staticMesh = (StaticMesh*)GetComponent("StaticMesh");

                if (staticMesh == nullptr)
                {
                    staticMesh = new StaticMesh();

                    staticMesh->SetFile(j["6.StaticMesh"]["FileName"]);

                    AddComponent(staticMesh);
                }
            }
            else if (TypeName == "7.SkinnedMesh")
            {
                SkinnedMesh* skinnedMesh = (SkinnedMesh*)GetComponent("SkinnedMesh");

                if (skinnedMesh == nullptr)
                {
                    skinnedMesh = new SkinnedMesh();
                    skinnedMesh->SetFile(j["7.SkinnedMesh"]["FileName"]);
                    AddComponent(skinnedMesh);
                }
            }
            else if (TypeName == "8.Script")
            {
                auto Scripts = j["8.Script"].get<std::vector<std::string>>();

                for each(auto item in Scripts)
                    SceneMgr->AddScript(this, item);
            }
        }

        file.close();
    }

    void GameObject::EnableScript(bool enable)
    {
        enableScript = enable;
    }

    bool GameObject::IsEnableScript()
    {
        return enableScript;
    }

    void GameObject::LockEditor(bool lock)
    {
        isEditorLock = lock;
    }
    
    bool GameObject::IsLockedEditor()
    {
        return isEditorLock;
    }

    void GameObject::AddScript(std::string name)
    {
        scripts.push_back(name);
    }

    std::list<std::string> GameObject::GetScript()
    {
        return scripts;
    }

    void GameObject::SetPrfabName(std::string prefabName)
    {
        this->prefab = prefabName;
    }
    
    std::string GameObject::GetPrfabName()
    {
        return prefab;
    }

    void GameObject::Init()
    {
        if (!enableScript)
            return;

        for (int i = 0; i < components.size(); i++)
            if ((components[i])->IsScript())
                ((Script*)(components[i]))->Init();
        
        InitFunc(this);
    }

    void GameObject::Awake()
    {
        if (!enableScript)
            return;

        for (auto var = components.begin(); var != components.end(); var++)
            if ((*var)->IsScript())
                ((Script*)*var)->Awake();
    
        AwakeFunc(this);
    }

	void GameObject::Reference()
	{
        if (!enableScript)
            return;

        for (auto var = components.begin(); var != components.end(); var++)
            if ((*var)->IsScript())
                ((Script*)*var)->Reference();
    
        ReferenceFunc(this);
    }
	
	void GameObject::Update()
	{
        auto rigidBody = this->GetComponent("RigidBody");
        if (rigidBody != nullptr)
            ((RigidBody*)rigidBody)->UpdateTransform();

        auto skinnedMesh = this->GetComponent("SkinnedMesh");
        if (skinnedMesh != nullptr)
            ((SkinnedMesh*)skinnedMesh)->UpdateAnimation();

        if (!enableScript)
            return;

        for (auto var = components.begin(); var != components.end(); var++)
        {
            if ((*var)->IsScript())
                ((Script*)*var)->Update();
            else if ((*var)->GetTypeName() == "RigidBody")
                ((RigidBody*)(*var))->UpdateTransform();
        }

        UpdateFunc(this);
    }
	
	void GameObject::LateUpdate()
	{
        if (!enableScript)
            return;

        for (auto var = components.begin(); var != components.end(); var++)
            if ((*var)->IsScript())
                ((Script*)*var)->LateUpdate();
    
        LateUpdateFunc(this);
    }

	void GameObject::LateRender()
	{
        if (!enableScript)
            return;

        for (auto var = components.begin(); var != components.end(); var++)
            if ((*var)->IsScript())
                ((Script*)*var)->LateRender();
    
        LateRenderFunc(this);
    }

    void GameObject::CollisionEvent(GameObject* otherObject)
    {
        if (!enableScript)
            return;

        for (auto var = components.begin(); var != components.end(); var++)
            if ((*var)->IsScript())
                ((Script*)*var)->CollisionEvent(otherObject);
    
        CollisionEventFunc(this, otherObject);
    }

    void GameObject::SetPipeLine(PIPE_LINE pipeLine)
    {
        this->pipeLine = pipeLine;
    }
    
    GameObject::PIPE_LINE GameObject::GetPipeLine()
    {
        return pipeLine;
    }
}