#pragma once
#include "ISingleton.h"
#include "Common.h"
#include "Scene.h"

namespace BONE_GRAPHICS 
{
    class GUI_Scene {
    public:
        virtual void UpdateFrame() = 0;
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

        GUI_Scene* guiScene;

	public:
		void InitializeMembers();

		SceneManager() {}
		virtual ~SceneManager() {}

		virtual void ReleaseMembers();

	public:
		void LoadingRender();

		void AddScene(string name, Scene* scene);

		bool StartScene(string name);
		void SetLoadingScene(string name);
		Scene* GetLoadScene();
		void EndScene(string name);

        void SetGUIScene(GUI_Scene* scene);

		int GetFrame();
		double GetTimeDelta();

		Scene* CurrentScene();
	};
}