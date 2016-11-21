#include "Common.h"
#include "SceneManager.h"
#include "InputManager.h"
#include <thread>

namespace BONE_GRAPHICS
{
	//SceneManager* SceneManager::pInst = NULL;

	void SceneManager::InitializeMembers()
	{
		CThreadSync sync;

		closeThread = false;
		loadingBarValue = 0;
		loadScene = "";
		frame = 0;
	}

	void SceneManager::LoadingRender() 
	{
		CThreadSync sync;
	}
	
	void LoadThreadFunc(Scene* Framework)
	{
		Framework->LoadContents();
	}

	void SceneManager::AddScene(string _name, Scene* _scene)
	{
		CThreadSync sync;

		sceneList.insert(std::pair<string, Scene*>(_name, _scene));
	}

	Scene* SceneManager::CurrentScene()
	{
		CThreadSync sync;

		return sceneList[curScene];
	}

	Scene* SceneManager::GetLoadScene()
	{
		CThreadSync sync;

		return sceneList[loadScene];
	}

	void SceneManager::SetLoadingScene(string _name)
	{
		CThreadSync sync;

		loadScene = _name;
	}

	int SceneManager::GetFrame()
	{
		CThreadSync sync;

		return frame;
	}

	double SceneManager::GetTimeDelta()
	{
		CThreadSync sync;

		return frame;
	}
	
	bool SceneManager::StartScene(string _name)
	{
		curScene = _name;
		sceneList[_name]->InitializeMembers();

		std::thread LoadingThread(LoadThreadFunc, sceneList[_name]);

		double lastTime = (double)timeGetTime();

		if (sceneList[loadScene] != NULL)
		{
			sceneList[loadScene]->InitializeMembers();
			sceneList[loadScene]->Reference();
			sceneList[loadScene]->LoadContents();

			do
			{
				double currTime = (double)timeGetTime();
				double deltaTime = (currTime - lastTime) * 0.001f;
				timeDelta = deltaTime;

				RenderMgr->GetDevice()->Clear(0, 0, D3DCLEAR_TARGET | D3DCLEAR_ZBUFFER, backColor, 1.0f, 0);
				RenderMgr->GetDevice()->BeginScene();

				if (sceneList[loadScene]->GetSceneFlag())
				{
					sceneList[loadScene]->Update(deltaTime);
					sceneList[loadScene]->LateUpdate(deltaTime);
					sceneList[loadScene]->Render(deltaTime);
					sceneList[loadScene]->LateRender();
				}

				RenderMgr->GetDevice()->EndScene();
				RenderMgr->GetDevice()->Present(0, 0, 0, 0);

				InputMgr->SetMouseWheelStatus(MOUSE_WHEEL_NONE);

				lastTime = currTime;

			} while (!sceneList[_name]->EndLoading() || sceneList[loadScene]->GetSceneFlag());

		}

		LoadingThread.join();

		sceneList[_name]->Reference();

		sceneList[_name]->SortPriorityObject();

		lastTime = (double)timeGetTime();

		MSG msg;
		::ZeroMemory(&msg, sizeof(MSG));

		double OneSecond = 0;
		int Frame = 0;

		while (msg.message != WM_QUIT)
		{
			if (::PeekMessage(&msg, 0, 0, 0, PM_REMOVE))
			{
				::TranslateMessage(&msg);
				::DispatchMessage(&msg);
			}
			else
			{
				if (sceneList[_name]->GetSceneFlag())
				{
					double currTime = (double)timeGetTime();
					double deltaTime = (currTime - lastTime) * 0.001f;
					timeDelta = deltaTime;

					RenderMgr->GetDevice()->Clear(0, 0, D3DCLEAR_TARGET | D3DCLEAR_ZBUFFER, backColor, 1.0f, 0);
					RenderMgr->GetDevice()->BeginScene();

					if (sceneList[_name]->GetSceneFlag())
					{
						sceneList[_name]->Update(deltaTime);
						sceneList[_name]->LateUpdate(deltaTime);
						sceneList[_name]->Render(deltaTime);
						sceneList[_name]->LateRender();
					}

					RenderMgr->GetDevice()->EndScene();
					RenderMgr->GetDevice()->Present(0, 0, 0, 0);

					InputMgr->SetMouseWheelStatus(MOUSE_WHEEL_NONE);

					lastTime = currTime;

					OneSecond += deltaTime;
					Frame++;

					if (OneSecond >= 1.0f)
					{
						frame = Frame;
						Frame = 0;
						OneSecond = 0;
					}
				}
				else
					break;
			}
		}

		closeThread = true;

		if (sceneList[_name]->GetSceneFlag() == false)
		{
			sceneList.erase(loadScene);
			loadScene = "";
			sceneList.erase(_name);
			return true;
		}

		sceneList.erase(loadScene);
		loadScene = "";
		sceneList.erase(_name);

		return false;
	}

	void SceneManager::EndScene(string _name)
	{
		CThreadSync sync;

		sceneList[_name]->SetSceneFlag(false);
	}

	void SceneManager::ReleaseMembers()
	{
		CThreadSync sync;
	}
}