#include "Common.h"
#include "SceneInfo.h"
#include "ResourceManager.h"

#pragma warning(disable:4996)

namespace BONE_GRAPHICS
{
    SceneInfoUI::SceneInfoUI()
    {
        endScene = false;

        openPopup = false;

        D3DXCOLOR color;
        color.r = 45 / 255.f;
        color.g = 45 / 255.f;
        color.b = 45 / 255.f;
        color.a = 255 / 255.f;
        SceneMgr->SetClearColor(color);
    }

    void SceneInfoUI::UpdateFrame()
    {
        if (!ImGui::Begin("Scene Loader", &open, ImGuiWindowFlags_NoMove | ImGuiWindowFlags_ShowBorders | ImGuiWindowFlags_AlwaysAutoResize))
        {
            ImGui::End();
            return;
        }

        ImVec2 Position(-ImGui::GetWindowSize().x / 2, -ImGui::GetWindowSize().y / 2);
        Position.x += RenderMgr->GetWidth() / 2;
        Position.y += RenderMgr->GetHeight() / 2;

        ImGui::SetWindowPos(Position);

        static char FileName[64] = "Scene";
        ImGui::InputText("File Name", FileName, 64);

        if (ImGui::Button("New Scene"))
        {
            std::string Path;
            std::string Name = FileName;
            Name += ".json";

            if (!ResourceMgr->ExistFile(Name, &Path))
            {
                endScene = true;
                isNewScene = true;
                openSceneName = FileName;
            }
            else
                ImGui::OpenPopup("Error?");
        }

        if (ImGui::BeginPopupModal("Error?", nullptr, ImGuiWindowFlags_AlwaysAutoResize))
        {
            ImGui::Text("A file with duplicate names exists.\nThis operation cannot be undone!\n\n");
            ImGui::Separator();

            if (ImGui::Button("OK", ImVec2(120, 0))) { ImGui::CloseCurrentPopup(); }
            ImGui::EndPopup();
        }

        auto Scenes = ResourceMgr->ExistFiles(".\\Engine\\Maps\\*");

        const int Size = Scenes.size();
        char** ListBoxItems = new char*[Size];

        int i = 0;
        for each(auto item in Scenes)
        {
            ListBoxItems[i] = new char[64];
            strcpy(ListBoxItems[i], Scenes[i].c_str());
            i++;
        }

        static int CurItem = 0;
        ImGui::ListBox("Scene List", &CurItem, ListBoxItems, Size, 4);

        if (ImGui::Button("Open Scene"))
        {
            isNewScene = false;
            endScene = true;

            for (int i = strlen(ListBoxItems[CurItem]) - 1; i >= 0; i--)
            {
                if (ListBoxItems[CurItem][i] != '.')
                    ListBoxItems[CurItem][i] = '\0';
                else
                {
                    ListBoxItems[CurItem][i] = '\0';
                    break;
                }
            }

            openSceneName = ListBoxItems[CurItem];
        }

        for (int i =0; i<Size; i++)
            delete[] ListBoxItems[i];
        delete[] ListBoxItems;

        ImGui::End();

        if (endScene)
        {
            SceneMgr->CurrentScene()->SetSceneFlag(true);
            SceneMgr->EndScene("InfoScene");
        }
    }

    bool SceneInfoUI::IsNewScene()
    {
        return isNewScene;
    }

    std::string SceneInfoUI::GetSceneName()
    {
        return openSceneName;
    }
}