#include "Common.h"
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
	void GameObject::SetStatic(bool _isStatic)
	{
		isStatic = _isStatic;
	}

	bool GameObject::GetStatic()
	{
		return isStatic;
	}

	void GameObject::Awake()
	{

	}

	void GameObject::SetActive(bool _isActive)
	{
		isActive = _isActive;

		if (_isActive)
			Awake();

		for (auto Iter = child.begin(); Iter != child.end(); Iter++)
		{
			(*Iter)->SetActive(_isActive);

			if (_isActive)
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

		parent = NULL;
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

	void GameObject::SetPriority(int _index)
	{
		priorty = _index;
	}

	int GameObject::GetPriority()
	{
		return priorty;
	}

	void GameObject::SetTag(string Tag)
	{
		tag = Tag;
	}

	Component* GameObject::GetComponent(string _typeName)
	{
		for (auto Iter = components.begin(); Iter != components.end(); Iter++)
		{
			if ((*Iter)->GetTypeName() == _typeName)
				return (*Iter);
		}

		return NULL;
	}

	void GameObject::LoadContents()
	{
		for (auto Iter = components.begin(); Iter != components.end(); Iter++)
		{
			(*Iter)->LoadContent();
		}
	}

	bool GameObject::AddComponent(Component* _component)
	{
		for (auto Iter = components.begin(); Iter != components.end(); Iter++)
		{
			if ((*Iter)->GetTypeName() == _component->GetTypeName())
				return false;
		}

		components.push_back(_component);

		return true;
	}

	void GameObject::Render(double _timeDelta)
	{
		IShader* Shader = ((IShader*)GetComponent("IShader"));

		if (Shader != NULL)
		{
			{
				UINT numPasses = 0;

				Shader->GetShader()->Begin(&numPasses, NULL);
				{
					for (UINT i = 0; i < numPasses; i++)
					{
						Shader->GetShader()->BeginPass(i);
						{
							if (GetComponent("StaticMesh") != NULL)
								((StaticMesh*)GetComponent("StaticMesh"))->Render(Shader, this);

							if (GetComponent("BillBoard") != NULL)
								((BillBoard*)GetComponent("BillBoard"))->Render(Shader, this);

							if (GetComponent("SpriteBillBoard") != NULL)
								((SpriteBillBoard*)GetComponent("SpriteBillBoard"))->Render(Shader, this);

							if (GetComponent("TrailRenderer") != NULL)
								((TrailRenderer*)GetComponent("TrailRenderer"))->Render(Shader);

							if (GetComponent("SkinnedMesh") != NULL)
								((SkinnedMesh*)GetComponent("SkinnedMesh"))->Render(Shader, this, _timeDelta);
						}
						Shader->GetShader()->End();
					}
					Shader->GetShader()->EndPass();
				}
			}
		}
		else
		{
			if (GetComponent("StaticMesh") != NULL)
			{
				((StaticMesh*)GetComponent("StaticMesh"))->Render(Shader, this);
			}

			if (GetComponent("BillBoard") != NULL)
			{
				((BillBoard*)GetComponent("BillBoard"))->Render(Shader, this);
			}

			if (GetComponent("TrailRenderer") != NULL)
			{
				((TrailRenderer*)GetComponent("TrailRenderer"))->Render(Shader);
			}

			if (GetComponent("SpriteBillBoard") != NULL)
				((SpriteBillBoard*)GetComponent("SpriteBillBoard"))->Render(Shader, this);
		}

		if (GetComponent("ScreenSprite") != NULL)
			((ScreenSprite*)GetComponent("ScreenSprite"))->Render(this);

		if (GetComponent("ScreenImage") != NULL)
			((ScreenImage*)GetComponent("ScreenImage"))->Render(this);

		if (GetComponent("ScreenButton") != NULL)
			((ScreenButton*)GetComponent("ScreenButton"))->Render(this);
	}

	void GameObject::AttachParent(GameObject* _parent)
	{
		parent = _parent;
		
		if (((Transform3D*)GetComponent("Transform3D")) != NULL)
		{
			((Transform3D*)GetComponent("Transform3D"))->AttachObject(_parent);
		}

		if (((Transform2D*)GetComponent("Transform2D")) != NULL)
		{
			((Transform2D*)GetComponent("Transform2D"))->AttachObject(_parent);
		}

		_parent->AttachChild(this);
	}

	void GameObject::DetachParent()
	{
		parent->DetachChild(this);
		parent = NULL;
	}

	GameObject* GameObject::GetParent()
	{
		return parent;
	}

	void GameObject::AttachChild(GameObject* _child)
	{
		if (_child != NULL)
		{
			if(_child->GetParent() == NULL)
				_child->AttachParent(this);

			bool Flag = false;

			for each (GameObject* var in child)
			{
				if (var == _child)
				{
					Flag = true;
					break;
				}
			}

			if(Flag == false)
				child.push_back(_child);
		}
	}

	void GameObject::DetachChild(GameObject* _child)
	{
		for (auto Iter = child.begin(); Iter != child.end(); Iter++)
		{
			if ((*Iter) == _child)
			{
				_child->DetachParent();
				break;
			}
		}
	}

	void GameObject::DetachChildByTag(string _tag)
	{
		for (auto Iter = child.begin(); Iter != child.end(); Iter++)
		{
			if ((*Iter)->Tag() == _tag)
			{
				(*Iter)->DetachParent();
				break;
			}
		}
	}
	
	GameObject* GameObject::FindChildByTag(string _tag)
	{
		for (auto Iter = child.begin(); Iter != child.end(); Iter++)
		{
			if ((*Iter)->Tag() == _tag)
			{
				return *Iter;
			}
		}
	}

	void GameObject::AttachTag(string _tag)
	{
		tag = _tag;
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
	
	void GameObject::Update(double _timeDelta)
	{
		// 비어있음
	}
	
	void GameObject::LateUpdate(double _timeDelta)
	{
		// 비어있음
	}

	void GameObject::LateRender()
	{
		// 비어있음
	}
}