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

namespace BONE_GRAPHICS
{
	void GameObject::SetStatic(bool _sStatic)
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

		for (auto Iter = child.begin(); Iter != child.end(); Iter++)
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

	GameObject::GameObject()
	{
		isActive = true;
		isStatic = false;

		parent = nullptr;
		std::vector<GameObject*> Child;
	}

	GameObject::~GameObject()
	{
		for (auto Iter = child.begin(); Iter != child.end();)
		{
			GameObject* Temp = (*Iter);
			Iter = child.erase(Iter);
			delete Temp;
		}

		for (auto Iter = components.begin(); Iter != components.end();)
		{
			Component* Temp = (*Iter);
			Iter = components.erase(Iter);
			delete Temp;
		}
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

	void GameObject::SetTag(string tag)
	{
		this->tag = tag;
	}

	Component* GameObject::GetComponent(string typeName)
	{
		for (auto Iter = components.begin(); Iter != components.end(); Iter++)
			if ((*Iter)->GetTypeName() == typeName)
				return (*Iter);
		
		return nullptr;
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
		
		components.push_back(component);

		return true;
	}

	void GameObject::Render()
	{
		IShader* Shader = ((IShader*)GetComponent("IShader"));

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

                        if (GetComponent("TrailRenderer") != nullptr)
                            ((TrailRenderer*)GetComponent("TrailRenderer"))->Render(Shader);

                        if (GetComponent("SkinnedMesh") != nullptr)
                            ((SkinnedMesh*)GetComponent("SkinnedMesh"))->Render(Shader, this, SceneMgr->GetTimeDelta());
                    }
                    Shader->GetShader()->End();
                }

                Shader->GetShader()->EndPass();
            }
		}
		else
		{
			if (GetComponent("StaticMesh") != nullptr)
				((StaticMesh*)GetComponent("StaticMesh"))->Render(Shader, this);

			if (GetComponent("BillBoard") != nullptr)
				((BillBoard*)GetComponent("BillBoard"))->Render(Shader, this);

			if (GetComponent("TrailRenderer") != nullptr)
				((TrailRenderer*)GetComponent("TrailRenderer"))->Render(Shader);

			if (GetComponent("SpriteBillBoard") != nullptr)
				((SpriteBillBoard*)GetComponent("SpriteBillBoard"))->Render(Shader, this);
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
				child->DetachParent();
				break;
			}
		}
	}

	void GameObject::DetachChildByTag(string tag)
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
	
	GameObject* GameObject::FindChildByTag(string tag)
	{
		for (auto Iter = childs.begin(); Iter != childs.end(); Iter++)
			if ((*Iter)->Tag() == tag)
				return *Iter;
	}

	void GameObject::AttachTag(string tag)
	{
		this->tag = tag;
	}
	
	void GameObject::DetachTag()
	{
		tag = "";
	}

	string GameObject::Tag()
	{
		return tag;
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
}