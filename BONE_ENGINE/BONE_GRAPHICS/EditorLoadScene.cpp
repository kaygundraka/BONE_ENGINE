#include "Common.h"
#include "EditorLoadScene.h"
#include "SceneManager.h"

namespace BONE_GRAPHICS
{
    EditorLoadScene::EditorLoadScene()
    {
        open = true;
        progress = 0;
    }

    void EditorLoadScene::UpdateFrame()
    {
        if (!ImGui::Begin("LoadingDialog", &open, ImVec2(0, 0), 0.3f, ImGuiWindowFlags_AlwaysAutoResize | ImGuiWindowFlags_NoTitleBar | ImGuiWindowFlags_NoResize | ImGuiWindowFlags_NoMove | ImGuiWindowFlags_NoSavedSettings))
        {
            ImGui::End();
            return;
        }

        auto Size = ImGui::GetWindowSize();
        ImGui::SetWindowPos(ImVec2((RenderMgr->GetWidth() - Size.x) / 2, (RenderMgr->GetHeight() - Size.y) / 2));

        ImGui::Text("Load Scene...");
        ImGui::ProgressBar(progress, ImVec2(0.0f, 0.0f));

        progress += SceneMgr->GetTimeDelta() / 2;

        if (progress >= 1 && SceneMgr->CurrentScene()->GetLoadPerTime() / 100 >= 1)
            SceneMgr->GetLoadScene()->SetSceneFlag(false);

        ImGui::End();
    }
}