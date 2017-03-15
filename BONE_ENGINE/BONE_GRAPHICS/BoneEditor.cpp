#include "Common.h"
#include "BoneEditor.h"
#include "SceneManager.h"
#include "Transform3D.h"
#include "Camera.h"
#include "Material.h"
#include "TrailRenderer.h"
#include "BillBoard.h"
#include "SpriteBillBoard.h"
#include "StaticMesh.h"
#include "SkinnedMesh.h"
#include "Collision.h"
#include "GameObject.h"
#include "InputManager.h"
#include "Collision.h"
#include "SceneInfo.h"
#include "EditorCameraScript.h"

#pragma warning (disable:4996)

using namespace BONE_GRAPHICS;

#define IM_ARRAYSIZE(_ARR)  ((int)(sizeof(_ARR)/sizeof(*_ARR)))

void LogDialog::Clear() { Buf.clear(); LineOffsets.clear(); }

void LogDialog::AddLog(const char* fmt, ...) IM_PRINTFARGS(2)
{
    int old_size = Buf.size();
    va_list args;
    va_start(args, fmt);
    Buf.appendv(fmt, args);
    va_end(args);
    for (int new_size = Buf.size(); old_size < new_size; old_size++)
        if (Buf[old_size] == '\n')
            LineOffsets.push_back(old_size);
    ScrollToBottom = true;
}

void LogDialog::Render(const char* title, bool* p_open)
{
    ImGui::SetNextWindowSize(ImVec2(500, 400), ImGuiSetCond_FirstUseEver);
    ImGui::Begin(title, p_open);
    if (ImGui::Button("Clear")) Clear();
    ImGui::SameLine();
    bool copy = ImGui::Button("Copy");
    ImGui::SameLine();
    Filter.Draw("Filter", -100.0f);
    ImGui::Separator();
    ImGui::BeginChild("scrolling", ImVec2(0, 0), false, ImGuiWindowFlags_HorizontalScrollbar);
    if (copy) ImGui::LogToClipboard();

    if (Filter.IsActive())
    {
        const char* buf_begin = Buf.begin();
        const char* line = buf_begin;
        for (int line_no = 0; line != NULL; line_no++)
        {
            const char* line_end = (line_no < LineOffsets.Size) ? buf_begin + LineOffsets[line_no] : NULL;
            if (Filter.PassFilter(line, line_end))
                ImGui::TextUnformatted(line, line_end);
            line = line_end && line_end[1] ? line_end + 1 : NULL;
        }
    }
    else
    {
        ImGui::TextUnformatted(Buf.begin());
    }

    if (ScrollToBottom)
        ImGui::SetScrollHere(1.0f);
    ScrollToBottom = false;
    ImGui::EndChild();
    ImGui::End();
}

BoneEditor::BoneEditor() {
    open = true;
    currentShowInfoObject = "";
    currentObjectName = "";
    showAddComponent = false;
    showObjectInfo = false;
    childSize = 0;
    showPrefabHierarchical = false;
    showLogWindow = false;
}

void BoneEditor::Run()
{
    bool flag = false;

    RenderMgr->GetDevice()->SetRenderState(D3DRS_LIGHTING, FALSE);
    InputMgr->SetFocusWindow(true);

    std::string OpenSceneName = "";
    bool IsNewScene = false;

    do
    {
        {
            auto_ptr<SceneInfoUI> EditorTtitleUI(new SceneInfoUI);
            SceneMgr->SetGUIScene(EditorTtitleUI.get());

            auto_ptr<Scene> SceneInfo(new Scene);

            SceneMgr->AddScene("InfoScene", SceneInfo.get());
            flag = SceneMgr->StartScene("InfoScene");

            OpenSceneName = EditorTtitleUI->GetSceneName();
            IsNewScene = EditorTtitleUI->IsNewScene();
        }

        if (flag)
        {
            SceneMgr->SetGUIScene(this);

            this->SetScriptProc(this);

            auto_ptr<Scene> ViewScene(new Scene);

            auto_ptr<GameObject> MainCamera(new GameObject);
            EditorCamera* EditorCameraScript = new EditorCamera(MainCamera.get(), "EditorCameraScript");
            MainCamera->AddComponent(EditorCameraScript);
            ViewScene->AddObject(MainCamera.get(), "EditorCamera");

            ViewScene->SetAmbientColor(1.0f, 1.0f, 1.0f, 1.0f);
            ViewScene->SetName(OpenSceneName);

            SceneMgr->AddScene(OpenSceneName, ViewScene.get());

            if (!IsNewScene)
                ViewScene->OnLoadSceneData();

            flag = SceneMgr->StartScene(OpenSceneName);
        }
    } while (flag);
}

void BoneEditor::AddScriptList(std::string scriptName)
{
    scriptList.push_back(scriptName);
}

std::list<std::string> BoneEditor::GetScriptList()
{
    return scriptList;
}

void BoneEditor::ShowFileMenu()
{
    if (ImGui::MenuItem("Save", "Ctrl+S"))
    {
        SceneMgr->CurrentScene()->ClearSceneData();

        auto DyamicObjectList = SceneMgr->CurrentScene()->GetObjectList();
        auto StaticObjectList = SceneMgr->CurrentScene()->GetStaticObjectList();

        for each(auto var in DyamicObjectList)
        {
            if (var->GetName() != "EditorCamera")
            {
                var->SavePrefab();
                var->SaveInMaps();
            }
        }

        for each(auto var in StaticObjectList)
        {
            if (var->GetName() != "EditorCamera")
            {
                var->SavePrefab();
                var->SaveInMaps();
            }
        }
    }

    if (ImGui::MenuItem("Quit", "Alt+F4"))
    {
        SceneMgr->CurrentScene()->SetSceneFlag(true);
        SceneMgr->EndScene(SceneMgr->CurrentScene()->GetName());
    }
}

void BoneEditor::ShowEditorMenu()
{
    if (ImGui::BeginMenu("FillMode"))
    {
        if (ImGui::MenuItem("POLYGON"))
            RenderMgr->GetDevice()->SetRenderState(D3DRS_FILLMODE, D3DFILL_SOLID);

        if (ImGui::MenuItem("WIRE_FRAME"))
            RenderMgr->GetDevice()->SetRenderState(D3DRS_FILLMODE, D3DFILL_WIREFRAME);

        ImGui::EndMenu();
    }

    if (ImGui::MenuItem("Show Collision"))
    {
        auto DyamicObjectList = SceneMgr->CurrentScene()->GetObjectList();
        auto StaticObjectList = SceneMgr->CurrentScene()->GetStaticObjectList();

        static bool EnableMeshBox = false;

        if (!EnableMeshBox)
            EnableMeshBox = true;
        else
            EnableMeshBox = false;

        for each(auto var in DyamicObjectList)
        {
            auto sm = ((StaticMesh*)var->GetComponent("StaticMesh"));
            if (sm != nullptr)
                sm->ShowMeshBox(EnableMeshBox);
        }

        for each(auto var in StaticObjectList)
        {
            auto sm = ((StaticMesh*)var->GetComponent("StaticMesh"));
            if (sm != nullptr)
                sm->ShowMeshBox(EnableMeshBox);
        }
    }

    if (ImGui::MenuItem("Show Log"))
    {
        if (showLogWindow)
            showLogWindow = false;
        else
            showLogWindow = true;
    }

    if (ImGui::MenuItem("Play"))
    {
    }
}

void BoneEditor::ShowHelpMenu()
{
    if (ImGui::MenuItem("Info")) {}
    ImGui::Text("Version 1.0");
}

void BoneEditor::ShowObjectInfo(std::string name)
{
    if (ImGui::CollapsingHeader("[GameObject Info]", ImGuiTreeNodeFlags_DefaultOpen))
    {
        static char buf[64] = "";
        ImGui::InputText("Name", buf, 64);
        ImGui::SameLine();

        if (ImGui::Button("Change"))
        {
            auto object = SceneMgr->CurrentScene()->FindObjectByName(name);
            name = buf;

            object->SetName(name);
        }
    }

    if (ImGui::CollapsingHeader("[Components]"), ImGuiTreeNodeFlags_DefaultOpen)
    {
        GameObject* object = SceneMgr->CurrentScene()->FindObjectByName(name);
        std::vector<Component*> components = object->GetComponents();

        for (auto iter = components.begin(); iter != components.end(); iter++)
        {
            if (ImGui::TreeNode((*iter)->GetTypeName().c_str()))
            {
                if ((*iter)->GetTypeName() == "Transform3D")
                {
                    Vector3 oriPos = ((Transform3D*)object->transform3D)->GetPosition();
                    Vector3 oriRot = ((Transform3D*)object->transform3D)->GetRotateAngle();
                    Vector3 oriScale = ((Transform3D*)object->transform3D)->GetScale();
                    Vector3 oriForward = ((Transform3D*)object->transform3D)->GetForward();

                    float pos[3] = { oriPos.x, oriPos.y, oriPos.z };
                    float rot[3] = { oriRot.x, oriRot.y, oriRot.z };
                    float scale[3] = { oriScale.x, oriScale.y, oriScale.z };
                    float forward[3] = { oriForward.x, oriForward.y, oriForward.z };

                    ImGui::InputFloat3("Position", pos);
                    ImGui::InputFloat3("Rotation", rot);
                    ImGui::InputFloat3("Scale", scale);
                    ImGui::InputFloat3("Forward", forward);

                    ((Transform3D*)object->transform3D)->SetPosition(pos[0], pos[1], pos[2]);
                    ((Transform3D*)object->transform3D)->SetRotate(rot[0], rot[1], rot[2]);
                    ((Transform3D*)object->transform3D)->SetScale(scale[0], scale[1], scale[2]);
                    ((Transform3D*)object->transform3D)->SetForward(forward[0], forward[1], forward[2]);
                }
                else if ((*iter)->GetTypeName() == "StaticMesh")
                {
                    auto MeshName = ((StaticMesh*)(*iter))->GetFileAddress();
                    auto Meshes = ResourceMgr->ExistFiles(".\\Resource\\Mesh\\*");

                    const int Size = Meshes.size();
                    char** ComboBoxItems = new char*[Size];

                    int CurItem = 0;

                    int i = 0;
                    for each(auto item in Meshes)
                    {
                        ComboBoxItems[i] = new char[64];
                        strcpy(ComboBoxItems[i], Meshes[i].c_str());

                        if (MeshName == ComboBoxItems[i])
                            CurItem = i;

                        i++;
                    }

                    ImGui::Combo("Meshes", &CurItem, ComboBoxItems, Size);

                    if (ImGui::Button("Chanage"))
                    {

                    }

                    ImGui::SameLine();

                    if (ImGui::Button("Remove"))
                    {

                    }

                    for (int i = 0; i < Size; i++)
                        delete ComboBoxItems[i];
                    delete[] ComboBoxItems;
                }
                else if ((*iter)->GetTypeName() == "Material")
                {
                    RGBA OriAmbient = ((Material*)(*iter))->GetAmbient();
                    RGBA OriDiffuse = ((Material*)(*iter))->GetDiffuse();
                    RGBA OriEmissive = ((Material*)(*iter))->GetEmissive();
                    RGBA OriSpecular = ((Material*)(*iter))->GetSpecular();
                    float OriShininess = ((Material*)(*iter))->GetShininess();

                    float Ambient[4] = { OriAmbient.r, OriAmbient.g, OriAmbient.b, OriAmbient.a };
                    float Diffuse[4] = { OriDiffuse.r, OriDiffuse.g, OriDiffuse.b, OriDiffuse.a };
                    float Emissive[4] = { OriEmissive.r, OriEmissive.g, OriEmissive.b, OriEmissive.a };
                    float Specular[4] = { OriSpecular.r, OriSpecular.g, OriSpecular.b, OriSpecular.a };
                    float Shininess = OriShininess;

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
        }

        if (ImGui::SmallButton("Add Component"))
        {
            currentShowInfoObject = name;
            showAddComponent = true;
        }
    }
}

void BoneEditor::ShowGameObjectTree(std::string treeName)
{
    if (ImGui::TreeNode(treeName.c_str()))
    {
        auto parent = SceneMgr->CurrentScene()->FindObjectByName(treeName);

        if (ImGui::TreeNode("Show Infos"))
        {
            ShowObjectInfo(treeName);
            ImGui::TreePop();
        }

        if (treeName != currentObjectName)
        {
            if (ImGui::SmallButton("Remove Object"))
            {
                parent->DetachParent();
                SceneMgr->CurrentScene()->Destroy(parent);
            }
        }

        if (ImGui::TreeNode("Add New Child"))
        {
            static char Name[64] = "";
            ImGui::InputText("Name", Name, 64);

            if (ImGui::SmallButton("Create"))
            {
                GameObject* Child = new GameObject();

                Transform3D* tr = new Transform3D();
                Child->AddComponent(tr);

                if (!strcmp("Name", ""))
                {
                    std::string ChildName = "Child";

                    char temp[10] = "";
                    itoa(childSize, temp, 10);
                    ChildName += temp;

                    Child->SetPrfabName(ChildName);
                    Child->SetName(ChildName);
                    Child->SetPriority(1);
                    SceneMgr->CurrentScene()->AddObject(Child, ChildName);
                }
                else
                {
                    Child->SetPrfabName(Name);
                    Child->SetName(Name);
                    Child->SetPriority(1);
                    SceneMgr->CurrentScene()->AddObject(Child, Name);
                }

                parent->AttachChild(Child);
                childSize++;
            }

            ImGui::TreePop();
        }

        auto childs = parent->GetChileds();

        for (auto iter = childs.begin(); iter != childs.end(); iter++)
            ShowGameObjectTree((*iter)->GetName());

        ImGui::TreePop();
    }
}

void BoneEditor::AllChildCheck(GameObject* parent)
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

void BoneEditor::UpdateFrame()
{
    bool isFocusedWindow = false;

    {
        if (!ImGui::Begin("Editor", &open, ImGuiWindowFlags_NoMove | ImGuiWindowFlags_MenuBar | ImGuiWindowFlags_ShowBorders | ImGuiWindowFlags_AlwaysAutoResize))
        {
            ImGui::End();
            return;
        }

        ImVec2 Position(0, 0);

        ImGui::SetWindowPos(Position);

        if (ImGui::IsRootWindowOrAnyChildFocused())
            isFocusedWindow = true;

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
                if ((*iter)->GetParent() != nullptr || (*iter)->GetName() == "EditorCamera")
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

                        (*iter)->SetActive(IsActive);
                        (*iter)->SetStatic(IsStatic);
                        (*iter)->SetPriority(Priority);

                        auto Tag = (*iter)->Tag();
                        char TagStr[64] = "";
                        strcpy(TagStr, Tag.c_str());
                        ImGui::InputText("Tag", TagStr, 64);
                        if (ImGui::Button("Change Tag"))
                            (*iter)->SetTag(Tag);

                        static char buf[64] = "";
                        ImGui::InputText("Name", buf, 64);
                        if (ImGui::Button("Change Name"))
                        {
                            auto object = SceneMgr->CurrentScene()->FindObjectByName((*iter)->GetName().c_str());
                            object->SetName(buf);
                        }

                        ImGui::TreePop();
                    }

                    if (ImGui::TreeNode("Prefab"))
                    {
                        auto Prefabs = ResourceMgr->ExistFiles(".\\Engine\\Prefabs\\*");

                        const int Size = Prefabs.size();
                        char** ComboBoxItems = new char*[Size];

                        int i = 0;
                        for each(auto item in Prefabs)
                        {
                            ComboBoxItems[i] = new char[64];
                            strcpy(ComboBoxItems[i], Prefabs[i].c_str());
                            i++;
                        }

                        static int CurItem = 0;

                        for (int i = 0; i < Size; i++)
                        {
                            if ((*iter)->GetPrfabName() + ".json" == Prefabs[i])
                            {
                                CurItem = i;
                                break;
                            }
                        }

                        ImGui::Combo("Prefabs", &CurItem, ComboBoxItems, Size);

                        static char PrefabName[64] = "";
                        ImGui::InputText("Name", PrefabName, 64);

                        if (ImGui::SmallButton("New File"))
                        {
                            (*iter)->SetPrfabName(PrefabName);
                            (*iter)->SavePrefab();
                        }

                        ImGui::SameLine();

                        if (ImGui::SmallButton("Change File"))
                        {
                            for (int i = Prefabs[CurItem].size(); i >= 0; i--)
                            {
                                if (ComboBoxItems[CurItem][i] != '.')
                                    ComboBoxItems[CurItem][i] = '\0';
                                else
                                {
                                    ComboBoxItems[CurItem][i] = '\0';
                                    break;
                                }
                            }
                            (*iter)->SetPrfabName(ComboBoxItems[CurItem]);
                        }

                        for (int i = 0; i < Size; i++)
                            delete ComboBoxItems[i];
                        delete[] ComboBoxItems;

                        ImGui::TreePop();
                    }

                    if ((*iter)->GetChileds().size() != 0)
                    {
                        if (ImGui::TreeNode("Childs"))
                        {
                            AllChildCheck((*iter));
                            ImGui::TreePop();
                        }
                    }

                    if (ImGui::SmallButton("Edit Object"))
                    {
                        showPrefabHierarchical = true;

                        currentObjectName = (*iter)->GetName();
                    }

                    if (ImGui::SmallButton("Remove Object"))
                    {
                        SceneMgr->CurrentScene()->Destroy((*iter));
                    }

                    if (ImGui::SmallButton("Focus On"))
                    {
                        auto FocusObject = SceneMgr->CurrentScene()->FindObjectByName((*iter)->GetName().c_str());

                        if (FocusObject != nullptr)
                        {
                            auto Position = ((Transform3D*)FocusObject->transform3D)->GetPosition();
                            ((Camera*)(SceneMgr->CurrentScene()->GetCurrentCamera()->GetComponent("Camera")))->SetTargetPosition(Position);
                            ((Transform3D*)SceneMgr->CurrentScene()->GetCurrentCamera()->transform3D)->Translate(Position + Vector3(100, 100, 100));
                        }
                    }

                    ImGui::TreePop();
                }
            }

            if (ImGui::TreeNode("New GameObject"))
            {
                auto Prefabs = ResourceMgr->ExistFiles(".\\Engine\\Prefabs\\*");

                const int Size = Prefabs.size();
                char** ComboBoxItems = new char*[Size];

                int i = 0;
                for each(auto item in Prefabs)
                {
                    ComboBoxItems[i] = new char[64];
                    strcpy(ComboBoxItems[i], Prefabs[i].c_str());
                    i++;
                }

                static int CurItem = 0;
                ImGui::Combo("Prefabs", &CurItem, ComboBoxItems, Size);

                static char PrefabName[64] = "";
                static bool NewPrefabs = false;
                if (NewPrefabs)
                    ImGui::InputText("Name", PrefabName, 64);

                ImGui::Checkbox("New Prefab", &NewPrefabs);
                ImGui::SameLine();

                if (ImGui::Button("Create"))
                {
                    GameObject* Object = new GameObject;

                    std::string ObjectName = "GameObject";
                    static int ObjectNum = 0;

                    char temp[10] = "";
                    itoa(ObjectNum, temp, 10);
                    ObjectName += temp;
                    ObjectNum++;

                    if (NewPrefabs)
                    {
                        Object->SetName(ObjectName);
                        Object->SetPriority(1);
                        Object->SetTag("");

                        Transform3D* tr = new Transform3D();
                        Object->AddComponent(tr);
                        Object->SetPrfabName(PrefabName);
                        Object->SavePrefab();
                        SceneMgr->CurrentScene()->AddObject(Object, ObjectName);
                    }
                    else if (Size != 0)
                    {
                        for (int i = Prefabs[CurItem].size(); i >= 0; i--)
                        {
                            if (ComboBoxItems[CurItem][i] != '.')
                                ComboBoxItems[CurItem][i] = '\0';
                            else
                            {
                                ComboBoxItems[CurItem][i] = '\0';
                                break;
                            }
                        }
                        Object->SetPrfabName(ComboBoxItems[CurItem]);
                        Object->LoadPrefab();

                        SceneMgr->CurrentScene()->AddObject(Object, ObjectName);
                    }
                }

                for (int i = 0; i < Size; i++)
                    delete ComboBoxItems[i];
                delete[] ComboBoxItems;

                ImGui::TreePop();
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

            if (ImGui::TreeNode("Clear Color"))
            {
                static ImVec4 clear_col(247.0f / 255.0f, 204.0f / 255.0f, 130.0f / 255.0f, 1);
                ImGui::ColorEdit3("clear color", (float*)&clear_col);

                D3DXCOLOR color;
                color.r = clear_col.x;
                color.g = clear_col.y;
                color.b = clear_col.z;
                color.a = 1.0f;
                SceneMgr->SetClearColor(color);

                ImGui::TreePop();
            }

            if (ImGui::TreeNode("Fog"))
            {
                auto CurScene = SceneMgr->CurrentScene();
                bool EnableFog = CurScene->OnFog();
                ImGui::Checkbox("On Fog", &EnableFog);

                static ImVec4 FogColor(
                    CurScene->GetFogColor().r * 255.0f,
                    CurScene->GetFogColor().g * 255.0f,
                    CurScene->GetFogColor().b * 255.0f,
                    1
                );

                D3DXCOLOR color;
                color.r = FogColor.x;
                color.g = FogColor.y;
                color.b = FogColor.z;
                color.a = 1.0f;

                float Start = CurScene->GetFogStart();
                float End = CurScene->GetFogEnd();
                float Density = CurScene->GetFogDensity();

                char* ComboBoxItems[] = {
                    "D3DFOG_NONE",
                    "D3DFOG_EXP",
                    "D3DFOG_EXP2",
                    "D3DFOG_LINEAR"
                };

                static int CurItem = CurScene->GetFogMode();

                if (EnableFog)
                {
                    ImGui::ColorEdit3("Color", (float*)&FogColor);
                    ImGui::DragFloat("Start", &Start, 0.2f, 0.0f, 1000.0f);
                    ImGui::DragFloat("End", &End, 0.2f, Start, 1000.0f);
                    ImGui::DragFloat("Density", &Density, 0.2f, 0.0f, 1000.0f);
                    ImGui::Combo("Prefabs", &CurItem, ComboBoxItems, 4);
                }

                CurScene->SetFogStatus(EnableFog, color, Start, End, Density, CurItem);

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

            if (ImGui::BeginMenu("Editor"))
            {
                ShowEditorMenu();
                ImGui::EndMenu();
            }

            if (ImGui::BeginMenu("Help"))
            {
                ShowHelpMenu();
                ImGui::EndMenu();
            }

            ImGui::EndMenuBar();
        }

        ImGui::End();
    }

    if (showPrefabHierarchical)
    {
        std::string WindowName = "Prefab Inspector : " + currentObjectName;

        ImGui::Begin(WindowName.c_str(), &showPrefabHierarchical, ImGuiWindowFlags_ShowBorders | ImGuiWindowFlags_AlwaysAutoResize);

        if (ImGui::IsRootWindowOrAnyChildFocused())
            isFocusedWindow = true;

        ShowGameObjectTree(currentObjectName);

        ImGui::End();
    }

    if (showAddComponent)
    {
        std::string WindowName = "Add Component : " + currentShowInfoObject;
        ImGui::Begin(WindowName.c_str(), &showAddComponent, ImGuiWindowFlags_AlwaysAutoResize);

        if (ImGui::IsRootWindowOrAnyChildFocused())
            isFocusedWindow = true;

        const char* listbox_items[] = { "StaticMesh", "Collision", "Script", "SkinnedMesh", "Camera", "Material", "TrailRenderer", "BillBoard", "SpriteBillBoard" };
        static int listbox_item_current = 0;
        ImGui::ListBox("Component\nTypes\n", &listbox_item_current, listbox_items, IM_ARRAYSIZE(listbox_items), 4);

        switch (listbox_item_current) {
        case 0:
        {
            auto Meshes = ResourceMgr->ExistFiles(".\\Resource\\Mesh\\*");

            const int Size = Meshes.size();
            char** ComboBoxItems = new char*[Size];

            int i = 0;
            for each(auto item in Meshes)
            {
                ComboBoxItems[i] = new char[64];
                strcpy(ComboBoxItems[i], item.c_str());
                i++;
            }

            static int CurItem = 0;
            ImGui::Combo("Meshes", &CurItem, ComboBoxItems, Size);

            if (ImGui::Button("Add Component"))
            {
                std::string fullpath = "";
                if (!ResourceMgr->ExistFile(ComboBoxItems[CurItem], &fullpath))
                    break;

                StaticMesh* Mesh = new StaticMesh();
                Mesh->SetFileAddress(ComboBoxItems[CurItem]);
                Mesh->LoadContent();
                auto Object = SceneMgr->CurrentScene()->FindObjectByName(currentShowInfoObject);
                Object->AddComponent(Mesh);

                showAddComponent = false;
            }
        }
        break;

        case 1:
        {
            char* ComboBoxItems[] = { "AABB", "SPHERE", "OBB" };

            static int CurItem = 0;
            ImGui::Combo("Meshes", &CurItem, ComboBoxItems, 3);

            if (ImGui::Button("Add Component"))
            {
                auto Object = SceneMgr->CurrentScene()->FindObjectByName(currentShowInfoObject);

                Collision* Coll = new Collision(Object);
                Coll->ComputeBoundingBox(ResourceMgr->LoadMesh(((StaticMesh*)Object->GetComponent("StaticMesh"))->GetFileAddress())->mesh);
                Coll->LoadContent();

                Object->AddComponent(Coll);

                showAddComponent = false;
            }
        }
        break;

        case 2:
        {
            const int Size = scriptList.size();
            char** ComboBoxItems = new char*[Size];

            int i = 0;
            for each(auto item in scriptList)
            {
                ComboBoxItems[i] = new char[64];
                strcpy(ComboBoxItems[i], item.c_str());
                i++;
            }

            static int CurItem = 0;
            ImGui::Combo("Scripts", &CurItem, ComboBoxItems, Size);

            if (ImGui::Button("Add Component"))
            {
                auto Object = SceneMgr->CurrentScene()->FindObjectByName(currentShowInfoObject);

                if (AddScript(Object, ComboBoxItems[CurItem]))
                    showAddComponent = false;
                else
                    showAddComponent = true;
            }
        }
        break;
        }

        ImGui::End();
    }

    if (showLogWindow)
        logDialog.Render("Log", &showLogWindow);
    
    if (isFocusedWindow)
        InputMgr->SetFocusWindow(false);
    else
        InputMgr->SetFocusWindow(true);
}