#pragma once
#include "Common.h"
#include "Component.h"
#include <vector>

namespace BONE_GRAPHICS 
{
	class GameObject {
	private:
		string tag;
		std::vector<Component*> components;

		GameObject* parent;
		std::vector<GameObject*> childs;

		int priorty;
		
		bool isStatic;
		bool isActive;

	public:
		GameObject();
		virtual ~GameObject();

		void SetStatic(bool isStatic);
		bool GetStatic();

		void SetActive(bool isActive);
		bool GetActive();

		void SetPriority(int index);
		int GetPriority();

		void AttachParent(GameObject* parent);
		void DetachParent();
		GameObject* GetParent();
		
		void AttachChild(GameObject* child);
		void DetachChild(GameObject* child);

		void DetachChildByTag(string tag);
		GameObject* FindChildByTag(string tag);

		void AttachTag(string tag);
		void DetachTag();
		string Tag();
		void SetTag(string tag);

		Component* GetComponent(string typeName);
		bool AddComponent(Component* component);

		void LoadContents();

		virtual void Awake();
		virtual void Init();
		virtual void Reference();
		virtual void Update();
		virtual void LateUpdate();
		virtual void LateRender();

		void Render();
	};
}
