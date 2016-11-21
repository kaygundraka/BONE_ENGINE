#pragma once
#include <MultiThreadSync.h>
#include "ISingleton.h"
#include "Common.h"
#include <map>
#include "Scene.h"

namespace BONE_GRAPHICS 
{
	class SceneManager : public ISingleton <SceneManager>, public BONE_SYSTEM::CMultiThreadSync<SceneManager>
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

	public:
		void InitializeMembers();

		SceneManager() {}
		virtual ~SceneManager() {}

		virtual void ReleaseMembers();

	public:
		void LoadingRender();

		void AddScene(string _name, Scene* _scene);

		bool StartScene(string _name);
		void SetLoadingScene(string _name);
		Scene* GetLoadScene();
		void EndScene(string _name);

		int GetFrame();
		double GetTimeDelta();

		Scene* CurrentScene();
	};
}