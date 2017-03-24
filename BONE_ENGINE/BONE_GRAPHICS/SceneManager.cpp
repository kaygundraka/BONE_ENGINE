#include "Common.h"
#include "SceneManager.h"
#include "InputManager.h"

namespace BONE_GRAPHICS
{
	void SceneManager::InitializeMembers()
	{
		ThreadSync sync;

		closeThread = false;
		loadingBarValue = 0;
		loadScene = "";
		frame = 0;

        backColor.r = 247.0f / 255.0f;
        backColor.g = 204.0f / 255.0f;
        backColor.b = 130.0f / 255.0f;
	}

	void SceneManager::LoadingRender() 
	{
		ThreadSync sync;
	}
	
	void LoadThreadFunc(Scene* Framework)
	{
        Framework->LoadSceneData();
		Framework->LoadContents();
        Framework->Reference();
	}

	void SceneManager::AddScene(std::string name, Scene* scene)
	{
		ThreadSync sync;

        scene->SetName(name);
		sceneList.insert(std::pair<std::string, Scene*>(name, scene));
	}

	Scene* SceneManager::CurrentScene()
	{
        return sceneList[curScene];
	}

	Scene* SceneManager::GetLoadScene()
	{
		return sceneList[loadScene];
	}

	void SceneManager::SetLoadScene(std::string name)
	{
		ThreadSync sync;

		loadScene = name;
	}

    void SceneManager::SetLoadGUIScene(GUI_Scene* scene)
    {
        ThreadSync sync;

        this->loadGuiScene = scene;
    }

	int SceneManager::GetFrame()
	{
		return frame;
	}

	double SceneManager::GetTimeDelta()
	{
		return timeDelta;
	}

    void SceneManager::SetClearColor(D3DXCOLOR color)
    {
        backColor = color;
    }

    D3DXCOLOR SceneManager::GetClearColor()
    {
        return backColor;
    }
	
	bool SceneManager::StartScene(std::string name)
	{
		curScene = name;
		sceneList[name]->InitializeMembers();

		std::thread LoadingThread(LoadThreadFunc, sceneList[name]);

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

                if (RenderMgr->UseImGUI())
                {
                    ImGui_ImplDX9_NewFrame();

                    loadGuiScene->UpdateFrame();
                }

				if (sceneList[loadScene]->GetSceneFlag())
				{
					sceneList[loadScene]->Update();
					sceneList[loadScene]->LateUpdate();
					sceneList[loadScene]->Render();
					sceneList[loadScene]->LateRender();
				}

                if (RenderMgr->UseImGUI())
                {
                    RenderMgr->GetDevice()->SetRenderState(D3DRS_FOGENABLE, FALSE);
                    RenderMgr->GetDevice()->SetRenderState(D3DRS_ZENABLE, false);
                    RenderMgr->GetDevice()->SetRenderState(D3DRS_ALPHABLENDENABLE, false);
                    RenderMgr->GetDevice()->SetRenderState(D3DRS_SCISSORTESTENABLE, false);
                    DWORD temp;
                    RenderMgr->GetDevice()->GetRenderState(D3DRS_FILLMODE, &temp);
                    RenderMgr->GetDevice()->SetRenderState(D3DRS_FILLMODE, D3DFILL_SOLID);

                    ImGui::Render();

                    RenderMgr->GetDevice()->SetRenderState(D3DRS_FILLMODE, temp);
                    RenderMgr->GetDevice()->SetRenderState(D3DRS_ZENABLE, true);
                    RenderMgr->GetDevice()->SetRenderState(D3DRS_SCISSORTESTENABLE, true);
                    RenderMgr->GetDevice()->SetRenderState(D3DRS_FOGENABLE, sceneList[name]->OnFog());
                }

				RenderMgr->GetDevice()->EndScene();
				RenderMgr->GetDevice()->Present(0, 0, 0, 0);

				InputMgr->SetMouseWheelStatus(MOUSE_WHEEL_NONE);

				lastTime = currTime;

			} while (!sceneList[name]->EndLoading() || sceneList[loadScene]->GetSceneFlag());
		}

		LoadingThread.join();
        
		sceneList[name]->SortPriorityObject();

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
				if (sceneList[name]->GetSceneFlag())
				{
					double currTime = (double)timeGetTime();
					double deltaTime = (currTime - lastTime) * 0.001f;
					timeDelta = deltaTime;

                    if (RenderMgr->UseImGUI())
                    {
                        ImGui_ImplDX9_NewFrame();

                        guiScene->UpdateFrame();
                    }

					RenderMgr->GetDevice()->Clear(0, 0, D3DCLEAR_TARGET | D3DCLEAR_ZBUFFER, backColor, 1.0f, 0);
					RenderMgr->GetDevice()->BeginScene();

					if (sceneList[name]->GetSceneFlag())
					{
						sceneList[name]->Update();
						sceneList[name]->LateUpdate();
						sceneList[name]->Render();
						sceneList[name]->LateRender();
					}

                    if (RenderMgr->UseImGUI())
                    {
                        RenderMgr->GetDevice()->SetRenderState(D3DRS_FOGENABLE, FALSE);
                        RenderMgr->GetDevice()->SetRenderState(D3DRS_ZENABLE, false);
                        RenderMgr->GetDevice()->SetRenderState(D3DRS_ALPHABLENDENABLE, false);
                        RenderMgr->GetDevice()->SetRenderState(D3DRS_SCISSORTESTENABLE, false);
                        DWORD temp;
                        RenderMgr->GetDevice()->GetRenderState(D3DRS_FILLMODE, &temp);
                        RenderMgr->GetDevice()->SetRenderState(D3DRS_FILLMODE, D3DFILL_SOLID);

                        ImGui::Render();

                        RenderMgr->GetDevice()->SetRenderState(D3DRS_FILLMODE, temp);
                        RenderMgr->GetDevice()->SetRenderState(D3DRS_ZENABLE, true);
                        RenderMgr->GetDevice()->SetRenderState(D3DRS_SCISSORTESTENABLE, true);
                        RenderMgr->GetDevice()->SetRenderState(D3DRS_FOGENABLE, sceneList[name]->OnFog());
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

		if (sceneList[name]->GetSceneFlag() == false)
		{
			sceneList.erase(loadScene);
			loadScene = "";
			sceneList.erase(name);
			return true;
		}

		sceneList.erase(loadScene);
		loadScene = "";
		sceneList.erase(name);

		return false;
	}

    void SceneManager::SetGUIScene(GUI_Scene* scene)
    {
        this->guiScene = scene;
    }

	void SceneManager::EndScene(std::string name)
	{
		ThreadSync sync;

		sceneList[name]->SetSceneFlag(false);
	}

	void SceneManager::ReleaseMembers()
	{
	}
}