#include <Common.h>
#include "SceneInfoUI.h"
#include <ResourceManager.h>
using namespace BONE_GRAPHICS;

SceneInfoUI::SceneInfoUI()
{

}

void SceneInfoUI::UpdateFrame()
{
    {
        if (!ImGui::Begin("Scene Loader", &open, ImGuiWindowFlags_MenuBar))
        {
            ImGui::End();
            return;
        }

        if (ImGui::CollapsingHeader("[New Scene]", ImGuiTreeNodeFlags_DefaultOpen))
        {

        }

        if (ImGui::CollapsingHeader("[Open Scene]", ImGuiTreeNodeFlags_DefaultOpen))
        {
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
            ImGui::Combo("Scenes", &CurItem, ComboBoxItems, Size);

            if (ImGui::Button("Load Scene"))
            {

            }
        }

        ImGui::End();
    }
}