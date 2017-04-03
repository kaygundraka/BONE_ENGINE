#include "Common.h"
#include "SceneManager.h"
#include "InputManager.h"

namespace BONE_GRAPHICS
{
    void LoadThreadFunc(Scene* Framework)
    {
        Framework->LoadSceneData();
        Framework->LoadContents();
        Framework->Reference();
    }

    void UpdateThreadFunc(Scene* Framework)
    {
        double lastTime = (double)timeGetTime();
        
        while (SceneMgr->IsPhysicsUpdate() == false)
        {
            double currTime = (double)timeGetTime();
            double deltaTime = (currTime - lastTime) * 0.001f;
            
            Framework->PhysicsUpdate(deltaTime);
            
            lastTime = currTime;
        }
    }
    
	void SceneManager::InitializeMembers()
	{
		ThreadSync sync;

        loadingBarValue = 0;
		loadScene = "";
		frame = 0;
        isPhysicsUpdate = false;
        uiUpdate = false;

        backColor.r = 45.0f / 255.0f;
        backColor.g = 45.0f / 255.0f;
        backColor.b = 45.0f / 255.0f;
	}

	void SceneManager::LoadingRender() 
	{
		ThreadSync sync;
	}
		
	void SceneManager::AddScene(std::string name, Scene* scene)
	{
		ThreadSync sync;

        scene->SetName(name);
		sceneList.insert(std::pair<std::string, Scene*>(name, scene));
	}

	Scene* SceneManager::CurrentScene()
	{
        if (sceneList.find(curScene) == sceneList.end())
            return nullptr;

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

    void SceneManager::SetFrame(int frame)
    {
        this->frame = frame;
    }

	int SceneManager::GetFrame()
	{
		return frame;
	}

	double SceneManager::GetTimeDelta()
	{
		return timeDelta;
	}

    void SceneManager::SetTimeDelta(double timeDelta)
    {
        this->timeDelta = timeDelta;
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
        isPhysicsUpdate = false;
        
		sceneList[name]->SortPriorityObject();

		MSG msg;
		::ZeroMemory(&msg, sizeof(MSG));

        physicsUpdateThread = new thread(UpdateThreadFunc, sceneList[name]);
        
        double OneSecond = 0;
        int Frame = 0;

        lastTime = (double)timeGetTime();
        
		while (msg.message != WM_QUIT)
		{
			if (::PeekMessage(&msg, 0, 0, 0, PM_REMOVE))
			{
				::TranslateMessage(&msg);
				::DispatchMessage(&msg);
                continue;
            }

            if (sceneList[name]->GetSceneFlag())
            {
                double currTime = (double)timeGetTime();
                double deltaTime = (currTime - lastTime) * 0.001f;
                timeDelta = deltaTime;

                if (RenderMgr->UseImGUI())
                {
                    ImGui_ImplDX9_NewFrame();
                    SceneMgr->GetGUIScene()->UpdateFrame();
                }
             
                RenderMgr->GetDevice()->Clear(0, 0, D3DCLEAR_TARGET | D3DCLEAR_ZBUFFER, backColor, 1.0f, 0);

                RenderMgr->GetDevice()->BeginScene();

                sceneList[name]->Update();
                sceneList[name]->LateUpdate();
                sceneList[name]->Render();
                sceneList[name]->LateRender();

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
        
        if (physicsUpdateThread != nullptr)
        {
            isPhysicsUpdate = true;
            physicsUpdateThread->join();

            delete physicsUpdateThread;
            physicsUpdateThread = nullptr;
        }

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

    GUI_Scene* SceneManager::GetGUIScene()
    {
        return guiScene;
    }

	void SceneManager::EndScene(std::string name)
	{
		ThreadSync sync;

		sceneList[name]->SetSceneFlag(false);
	}

    bool SceneManager::IsPhysicsUpdate()
    {
        return isPhysicsUpdate;
    }
    
	void SceneManager::ReleaseMembers()
	{
        isPhysicsUpdate = true;
        
        if (physicsUpdateThread != nullptr)
        {
            physicsUpdateThread->join();
            delete physicsUpdateThread;
            physicsUpdateThread = nullptr;
        }
	}
}