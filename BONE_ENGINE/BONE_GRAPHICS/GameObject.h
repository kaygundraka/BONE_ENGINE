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
		std::vector<GameObject*> child;

		int priorty;
		
		bool isStatic;
		bool isActive;

	public:
		GameObject();
		virtual ~GameObject();

		void SetStatic(bool _isStatic);
		bool GetStatic();

		void SetActive(bool _isActive);
		bool GetActive();

		void SetPriority(int _index);
		int GetPriority();

		void AttachParent(GameObject* _parent);
		void DetachParent();
		GameObject* GetParent();
		
		void AttachChild(GameObject* _child);
		void DetachChild(GameObject* _child);

		void DetachChildByTag(string _tag);
		GameObject* FindChildByTag(string _tag);

		void AttachTag(string _tag);
		void DetachTag();
		string Tag();
		void SetTag(string Tag);

		Component* GetComponent(string _typeName);
		bool AddComponent(Component* _component);

		void LoadContents();

		virtual void Awake();
		virtual void Init();
		virtual void Reference();
		virtual void Update(double _timeDelta);
		virtual void LateUpdate(double _timeDelta);
		virtual void LateRender();

		void Render(double _timeDelta);
	};
}
