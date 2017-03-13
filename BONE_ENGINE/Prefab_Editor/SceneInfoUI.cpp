#include <Common.h>
#include "SceneInfoUI.h"
#include <ResourceManager.h>
using namespace BONE_GRAPHICS;

SceneInfoUI::SceneInfoUI()
{
    endScene = false;
}

void SceneInfoUI::UpdateFrame()
{
    {
        if (!ImGui::Begin("Scene Loader", &open, ImGuiWindowFlags_MenuBar))
        {
            ImGui::End();
            return;
        }

        static char FileName[64] = "Scene Name";
        ImGui::InputText("New File Name", FileName, 64);

        if (ImGui::Button("New Scene"))
        {
            endScene = true;
            isNewScene = true;
            openSceneName = FileName;
        }

        auto Scenes = ResourceMgr->ExistFiles(".\\Engine\\Maps\\*");

        const int Size = Scenes.size();
        char** ComboBoxItems = new char*[Size];

        int i = 0;
        for each(auto item in Scenes)
        {
            ComboBoxItems[i] = new char[64];
            strcpy(ComboBoxItems[i], Scenes[i].c_str());
            i++;
        }

        static int CurItem = 0;
        ImGui::Combo("Scene List", &CurItem, ComboBoxItems, Size);

        if (ImGui::Button("Open Scene"))
        {
            isNewScene = false;
            endScene = true;

            for (int i = strlen(ComboBoxItems[CurItem]) - 1; i >= 0; i--)
            {
                if (ComboBoxItems[CurItem][i] != '.')
                    ComboBoxItems[CurItem][i] = '\0';
                else
                {
                    ComboBoxItems[CurItem][i] = '\0';
                    break;
                }
            }

            openSceneName = ComboBoxItems[CurItem];
        }

        ImGui::End();

        if (endScene)
        {
            SceneMgr->CurrentScene()->SetSceneFlag(true);
            SceneMgr->EndScene("InfoScene");
        }
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