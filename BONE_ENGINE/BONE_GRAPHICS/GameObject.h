#pragma once
#include "Common.h"
#include "Component.h"

namespace BONE_GRAPHICS 
{
	class GameObject {
	private:
		std::string tag;
        std::string name;
        std::string prefab;
		std::vector<Component*> components;

		GameObject* parent;

		std::vector<GameObject*> childs;

		int priorty;
		
		bool isStatic;
		bool isActive;

	public:
        Component* transform3D;
        Component* transform2D;

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

		void DetachChildByTag(std::string tag);
		GameObject* FindChildByTag(std::string tag);

        std::vector<GameObject*> GetChileds();

		void AttachTag(std::string tag);
		void DetachTag();
		std::string Tag();
        void SetName(std::string name);
        std::string GetName();
		void SetTag(std::string tag);
        void SetPrfabName(std::string prefabName);
        std::string GetPrfabName();

        void SaveInMaps();
        void LoadPrefab();
        void SavePrefab();

		Component* GetComponent(std::string typeName);
        std::vector<Component*> GetComponents();
		bool AddComponent(Component* component);

		void LoadContents();

		virtual void Awake();
		virtual void Init();
		virtual void Reference();
		virtual void Update();
		virtual void LateUpdate();
		virtual void LateRender();
        
        virtual void OnCollisionEnter(GameObject* otherObject);
        virtual void OnCollisionStay(GameObject* otherObject);
        virtual void OnCollisionLeave(GameObject* otherObject);

		void Render();
	};
}
