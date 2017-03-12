#include "EditorUI.h"
#include <Common.h>
#include <SceneManager.h>
#include <Transform3D.h>
#include <Camera.h>
#include <Material.h>
#include <TrailRenderer.h>
#include <BillBoard.h>
#include <SpriteBillBoard.h>
#include <StaticMesh.h>
#include <SkinnedMesh.h>
#include <Collision.h>
#include <GameObject.h>
#include <InputManager.h>

#define IM_ARRAYSIZE(_ARR)  ((int)(sizeof(_ARR)/sizeof(*_ARR)))

EditorUI::EditorUI() {
    open = true;
    rootObjectName = "New Object";
    currentObjectName = rootObjectName;
    showAddComponent = false;
    showObjectInfo = false;
    childSize = 0;
    showPrefabHierarchical = false;
}

void EditorUI::ShowFileMenu()
{
    if (ImGui::MenuItem("New")) {}
    if (ImGui::MenuItem("Open", "Ctrl+O")) {}
    if (ImGui::MenuItem("Save", "Ctrl+S")) {}
    if (ImGui::MenuItem("Save As..")) {}
    if (ImGui::MenuItem("Quit", "Alt+F4")) {}
}

void EditorUI::ShowOpitionMenu()
{
    if (ImGui::BeginMenu("RenderMode"))
    {
        if (ImGui::MenuItem("POLYGON")) 
            RenderMgr->GetDevice()->SetRenderState(D3DRS_FILLMODE, D3DFILL_SOLID);
        
        if (ImGui::MenuItem("WIRE_FRAME")) 
            RenderMgr->GetDevice()->SetRenderState(D3DRS_FILLMODE, D3DFILL_WIREFRAME);
        
        ImGui::EndMenu();
    }

    if (ImGui::MenuItem("Show Collision")) {}
}

void EditorUI::ShowHelpMenu()
{
    if (ImGui::BeginMenu("RenderMode"))
    {
        if (ImGui::MenuItem("POLYGON")) {}
        if (ImGui::MenuItem("WIRE_FRAME")) {}

        ImGui::EndMenu();
    }

    if (ImGui::MenuItem("Show Collision")) {}
}

void EditorUI::ShowGameObjectTree(std::string treeName)
{
    if (ImGui::TreeNode(treeName.c_str()))
    {
        auto parent = SceneMgr->CurrentScene()->FindObjectByName(treeName);
        auto childs = parent->GetChileds();

        if (ImGui::SmallButton("Show Infos"))
        {
            currentObjectName = treeName;
            showObjectInfo = true;
        }

        if (treeName != rootObjectName)
        {
            if (ImGui::SmallButton("Remove Object"))
            {
                parent->DetachParent();
                SceneMgr->CurrentScene()->Destroy(parent);
            }
        }

        if (ImGui::SmallButton("Add New Child"))
        {
            GameObject* Child = new GameObject();
            std::string ChildName = "Child";

            char temp[10] = "";
            itoa(childSize, temp, 10);
            ChildName += temp;

            Child->SetName(ChildName);

            Transform3D* tr = new Transform3D();
            Child->AddComponent(tr);

            SceneMgr->CurrentScene()->AddObject(Child, ChildName);
            parent->AttachChild(Child);
            childSize++;
        }

        for (auto iter = childs.begin(); iter != childs.end(); iter++)
            ShowGameObjectTree((*iter)->GetName());

        ImGui::TreePop();
    }
}

void EditorUI::AllChildCheck(GameObject* parent)
{
    auto childs = parent->GetChileds();

    for (auto iter = childs.begin(); iter != childs.end(); iter++)
    {
        if (ImGui::TreeNode((*iter)->GetName().c_str()))
        {
            AllChildCheck((*iter));
            ImGui::TreePop();
        }
    }
}

void EditorUI::UpdateFrame()
{
    if (!ImGui::IsWindowFocused())
        InputMgr->SetFocusWindow(true);
    else
        InputMgr->SetFocusWindow(false);

    {
        if (!ImGui::Begin("Editor", &open, ImGuiWindowFlags_MenuBar))
        {
            ImGui::End();
            return;
        }

        std::list<GameObject*> ObjectList = SceneMgr->CurrentScene()->GetObjectList();
        auto StaticObjectList = SceneMgr->CurrentScene()->GetStaticObjectList();
        ObjectList.sort();
        StaticObjectList.sort();
        ObjectList.merge(StaticObjectList);

        if (ImGui::CollapsingHeader("[Scene Entities]", ImGuiTreeNodeFlags_DefaultOpen))
        {
            static bool FindOption;
            ImGui::Checkbox("Enable Find Option", &FindOption);
            
            std::list<std::string> FindObjectName;

            if (FindOption)
            {
                std::list<std::string> objectNames;

                for (auto iter = ObjectList.begin(); iter != ObjectList.end(); iter++)
                {
                    if ((*iter)->GetParent() != nullptr)
                        continue;

                    objectNames.push_back((*iter)->GetName());
                }

                static ImGuiTextFilter filter;
                filter.Draw();

                for (auto iter = objectNames.begin(); iter != objectNames.end(); iter++)
                    if (filter.PassFilter(iter->c_str()))
                        FindObjectName.push_back(iter->c_str());
            }

            for (auto iter = ObjectList.begin(); iter != ObjectList.end(); iter++)
            {
                if ((*iter)->GetParent() != nullptr)
                    continue;

                if (FindOption)
                {
                    bool Exist = false;

                    for (auto iter2 = FindObjectName.begin(); iter2 != FindObjectName.end(); iter2++)
                        if ((*iter2) == (*iter)->GetName())
                            Exist = true;

                    if (!Exist)
                        continue;
                }

                if (ImGui::TreeNode((*iter)->GetName().c_str()))
                {
                    if (ImGui::TreeNode("Information"))
                    {
                        auto IsActive = (*iter)->GetActive();
                        ImGui::Checkbox("IsActvie", &IsActive);
                        ImGui::SameLine();

                        auto IsStatic = (*iter)->GetStatic();
                        ImGui::Checkbox("IsStatic", &IsStatic);

                        auto Priority = (*iter)->GetPriority();
                        ImGui::InputInt("Priority", &Priority);

                        auto Tag = (*iter)->Tag();
                        char TagStr[64] = "";
                        strcpy(TagStr, Tag.c_str());
                        ImGui::InputText("Tag", TagStr, 64);

                        (*iter)->SetActive(IsActive);
                        (*iter)->SetStatic(IsStatic);
                        (*iter)->SetPriority(Priority);
                        (*iter)->SetTag(Tag);

                        ImGui::TreePop();
                    }

                    if (ImGui::TreeNode("Prefab"))
                    {
                        char PrefabName[64] = "";
                        ImGui::InputText("PrefabName", PrefabName, 64);
                        std::string temp;

                        if (ImGui::SmallButton("New Prfab"))
                        {

                        }
                        
                        ImGui::SameLine();

                        if (ImGui::SmallButton("Save Prfab"))
                        {

                        }
                        
                        if (ImGui::SmallButton("Edit Prfab"))
                        {
                            if (!ResourceMgr->ExistFile(PrefabName, &temp))
                            {
                                showPrefabHierarchical = true;
                                currentObjectName = (*iter)->GetName();
                            }
                        }
                    
                        ImGui::TreePop();
                    }
                    
                    if (ImGui::TreeNode("Childs"))
                    {
                        AllChildCheck((*iter));
                        ImGui::TreePop();
                    }

                    if (ImGui::SmallButton("Remove Object"))
                    {
                        SceneMgr->CurrentScene()->Destroy((*iter));
                    }

                    if (ImGui::SmallButton("Focus On"))
                    {
                    }

                    ImGui::TreePop();
                }
            }

            if (ImGui::Button("New GameObject"))
            {
            }
        }
        
        if (ImGui::CollapsingHeader("[Environment Setting]", ImGuiTreeNodeFlags_DefaultOpen))
        {
            if (ImGui::TreeNode("SkyBox"))
            {
                static char SkyBoxFolder[64] = "miramar";
                ImGui::InputText("Folder Name", SkyBoxFolder, 64);

                static char TypeName[64] = "tga";
                ImGui::InputText("ImageType", TypeName, 64);

                if (ImGui::Button("Chanage SkyBox"))
                    SceneMgr->CurrentScene()->SetSkybox(SkyBoxFolder, TypeName);

                ImGui::TreePop();
            }

            if (ImGui::TreeNode("World Ambient"))
            {
                auto OriAmbient = SceneMgr->CurrentScene()->GetAmbientColor();

                static float Ambient[4] = { OriAmbient.r, OriAmbient.g, OriAmbient.b, OriAmbient.a };
                
                ImGui::InputFloat3("Ambient", Ambient);

                SceneMgr->CurrentScene()->SetAmbientColor(Ambient[0], Ambient[1], Ambient[2], Ambient[3]);

                ImGui::TreePop();
            }

            if (ImGui::TreeNode("Terrain"))
            {
                ImGui::TreePop();
            }
        }

        if (ImGui::BeginMenuBar())
        {
            if (ImGui::BeginMenu("File"))
            {
                ShowFileMenu();
                ImGui::EndMenu();
            }

            if (ImGui::BeginMenu("Option"))
            {
                ShowOpitionMenu();
                ImGui::EndMenu();
            }

            if (ImGui::MenuItem("Help")) {}

            ImGui::EndMenuBar();
        }

        ImGui::End();
    }

    if (showObjectInfo)
    {
        std::string WindowName = "GameObject Info : " + currentObjectName;
        ImGui::SetNextWindowSize(ImVec2(200, 200), ImGuiSetCond_FirstUseEver);
        ImGui::Begin(WindowName.c_str(), &showObjectInfo);

        if (ImGui::CollapsingHeader("[GameObject Info]", ImGuiTreeNodeFlags_DefaultOpen))
        {
            static char buf[64] = "";
            strcpy(buf, currentObjectName.c_str());
            ImGui::InputText("Name", buf, 64);
            ImGui::SameLine();
            if (ImGui::Button("Change"))
            {
                auto object = SceneMgr->CurrentScene()->FindObjectByName(currentObjectName);
                if (currentObjectName == rootObjectName)
                    rootObjectName = buf;

                currentObjectName = buf;

                object->SetName(currentObjectName);
            }
        }

        if (ImGui::CollapsingHeader("[Components]"), ImGuiTreeNodeFlags_DefaultOpen)
        {
            if (ImGui::SmallButton("Add Component"))
            {
                showAddComponent = true;
            }

            auto object = SceneMgr->CurrentScene()->FindObjectByName(currentObjectName);
            auto childs = object->GetComponents();

            for (auto iter = childs.begin(); iter != childs.end(); iter++)
            {
                if (ImGui::TreeNode((*iter)->GetTypeName().c_str()))
                {
                    if ((*iter)->GetTypeName() == "Transform3D")
                    {
                        Vector3 oriPos = ((Transform3D*)(*iter))->GetPosition();
                        Vector3 oriRot = ((Transform3D*)(*iter))->GetRotateAngle();
                        Vector3 oriScale = ((Transform3D*)(*iter))->GetScale();
                        Vector3 oriForward = ((Transform3D*)(*iter))->GetForward();

                        static float pos[3] = { oriPos.x, oriPos.y, oriPos.z };
                        static float rot[3] = { oriRot.x, oriRot.y, oriRot.z };
                        static float scale[3] = { oriScale.x, oriScale.y, oriScale.z };
                        static float forward[3] = { oriForward.x, oriForward.y, oriForward.z };
                        
                        ImGui::InputFloat3("Position", pos);
                        ImGui::InputFloat3("Rotation", rot);
                        ImGui::InputFloat3("Scale", scale);
                        ImGui::InputFloat3("Forward", forward);
                        
                        ((Transform3D*)(*iter))->SetPosition(pos[0], pos[1], pos[2]);
                        ((Transform3D*)(*iter))->SetRotate(rot[0], rot[1], rot[2]);
                        ((Transform3D*)(*iter))->SetScale(scale[0], scale[1], scale[2]);
                        ((Transform3D*)(*iter))->SetForward(forward[0], forward[1], forward[2]);
                    }
                    else if ((*iter)->GetTypeName() == "StaticMesh")
                    {
                        static char buf[64] = "";

                        strcpy(buf, ((StaticMesh*)(*iter))->GetFileAddress().c_str());

                        ImGui::InputText("File Name", buf, 64);
                        
                        if (ImGui::Button("Chanage"))
                        {
                            
                        }

                        ImGui::SameLine();

                        if (ImGui::Button("Remove"))
                        {

                        }
                    }
                    else if ((*iter)->GetTypeName() == "Material")
                    {
                        RGBA OriAmbient = ((Material*)(*iter))->GetAmbient();
                        RGBA OriDiffuse = ((Material*)(*iter))->GetDiffuse();
                        RGBA OriEmissive = ((Material*)(*iter))->GetEmissive();
                        RGBA OriSpecular = ((Material*)(*iter))->GetSpecular();
                        float OriShininess = ((Material*)(*iter))->GetShininess();

                        static float Ambient[4] = { OriAmbient.r, OriAmbient.g, OriAmbient.b, OriAmbient.a };
                        static float Diffuse[4] = { OriDiffuse.r, OriDiffuse.g, OriDiffuse.b, OriDiffuse.a };
                        static float Emissive[4] = { OriEmissive.r, OriEmissive.g, OriEmissive.b, OriEmissive.a };
                        static float Specular[4] = { OriSpecular.r, OriSpecular.g, OriSpecular.b, OriSpecular.a };
                        static float Shininess = OriShininess;

                        ImGui::InputFloat3("Ambient", Ambient);
                        ImGui::InputFloat3("Diffuse", Diffuse);
                        ImGui::InputFloat3("Emissive", Emissive);
                        ImGui::InputFloat3("Specular", Specular);
                        ImGui::InputFloat("Shininess", &Shininess);

                        ((Material*)(*iter))->SetAmbient(Ambient[0], Ambient[1], Ambient[2], Ambient[3]);
                        ((Material*)(*iter))->SetDiffuse(Diffuse[0], Diffuse[1], Diffuse[2], Diffuse[3]);
                        ((Material*)(*iter))->SetEmissive(Emissive[0], Emissive[1], Emissive[2], Emissive[3]);
                        ((Material*)(*iter))->SetSpecular(Specular[0], Specular[1], Specular[2], Specular[3]);
                        ((Material*)(*iter))->SetShininess(Shininess);
                    }

                    ImGui::TreePop();
                }
                else if ((*iter)->GetTypeName() == "Transform3D")
                {
                }
            }
        }

        ImGui::End();
    }

    if (showPrefabHierarchical)
    {
        std::string WindowName = "Prefab Inspector : " + currentObjectName;
        ImGui::SetNextWindowSize(ImVec2(200, 200), ImGuiSetCond_FirstUseEver);
        ImGui::Begin(WindowName.c_str(), &showPrefabHierarchical);

        ShowGameObjectTree(currentObjectName);

        ImGui::End();
    }

    if (showAddComponent)
    {
        std::string WindowName = "Component Setup : " + currentObjectName;

        ImGui::SetNextWindowSize(ImVec2(200, 200), ImGuiSetCond_FirstUseEver);
        ImGui::Begin(WindowName.c_str(), &showAddComponent);

        const char* listbox_items[] = { "StaticMesh", "SkinnedMesh", "Camera", "Material", "TrailRenderer", "BillBoard", "SpriteBillBoard", "Collision" };
        static int listbox_item_current = 0;
        ImGui::ListBox("Component\nTypes\n", &listbox_item_current, listbox_items, IM_ARRAYSIZE(listbox_items), 4);

        switch (listbox_item_current) {
        case 0:
        {
            static char buf[64] = "";
            ImGui::InputText("FileName", buf, 64);
            ImGui::SameLine();
            
            if (ImGui::Button("Add Component"))
            {
                std::string fullpath = "";
                if (!ResourceMgr->ExistFile(buf, &fullpath))
                    break;

                StaticMesh* Mesh = new StaticMesh();
                Mesh->SetFileAddress(buf);
                Mesh->LoadContent();
                auto object = SceneMgr->CurrentScene()->FindObjectByName(currentObjectName);
                object->AddComponent(Mesh);

                showAddComponent = false;
            }
        }
        break;
        }

        ImGui::End();
    }
}