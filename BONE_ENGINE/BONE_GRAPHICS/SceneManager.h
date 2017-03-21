#pragma once
#include "ISingleton.h"
#include "Common.h"
#include "Scene.h"
#include "SceneManager.h"

namespace BONE_GRAPHICS 
{
    class GUI_Scene {
    public:
        virtual void UpdateFrame() {};
    };

	class SceneManager : public ISingleton <SceneManager>, public BONE_SYSTEM::MultiThreadSync<SceneManager>
	{
	private:
		float loadingBarValue;
		bool closeThread;
		D3DXCOLOR backColor;

		string curScene;
		std::map<string, Scene*> sceneList;
		string loadScene;

		double timeDelta;
		int frame;

        D3DXCOLOR color;

        GUI_Scene* guiScene;  
        GUI_Scene* loadGuiScene;

	public:
		void InitializeMembers();

		SceneManager() {}
		virtual ~SceneManager() {}

		virtual void ReleaseMembers();

	public:
		void LoadingRender();

        void (*SetGameObject)(GameObject* obj);

		void AddScene(string name, Scene* scene);

		bool StartScene(string name);
		void SetLoadScene(string name);
        void SetLoadGUIScene(GUI_Scene* scene);
		Scene* GetLoadScene();
		void EndScene(string name);

        void SetClearColor(D3DXCOLOR color);
        D3DXCOLOR GetClearColor();

        void SetGUIScene(GUI_Scene* scene);

		int GetFrame();
		double GetTimeDelta();

        bool(*AddScript)(GameObject* object, std::string scripts);

		Scene* CurrentScene();
	};
}