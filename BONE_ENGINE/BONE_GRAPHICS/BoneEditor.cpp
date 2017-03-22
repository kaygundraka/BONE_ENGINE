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
#include "GameObject.h"
#include "InputManager.h"
#include "Rp3dCollision.h"
#include "Rp3dRigidBody.h"
#include "SceneInfo.h"
#include "EditorCameraScript.h"
#include "EditorLoadScene.h"
#include "PosPivot.h"
#include "SkinnedMesh.h"
#include "RuntimeCompiler.h"
#include "LogManager.h"

#pragma warning (disable:4996)

using namespace BONE_GRAPHICS;

#define IM_ARRAYSIZE(_ARR)  ((int)(sizeof(_ARR)/sizeof(*_ARR)))

#pragma region !!---------------- LOG DIALOG ----------------!!

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
    ImGui::SetNextWindowPos(ImVec2(0, RenderMgr->GetHeight() - 200));
    ImGui::Begin(title, p_open, ImVec2(RenderMgr->GetWidth() - 300, 200), -1.0f, ImGuiWindowFlags_NoResize | ImGuiWindowFlags_NoMove | ImGuiWindowFlags_NoCollapse);
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

#pragma endregion

BoneEditor::BoneEditor() {
    showMainEditor = true;
    showAddComponent = false;
    showObjectInfo = false;
    showPrefabInspector = false;
    showLogWindow = true;
    showEnvironmentSetting = false;
    isTestPlay = false;
    isFocused = false;
 
    playScene = "";

    currentShowInfoObject = "";
    currentObjectName = "";
    childSize = 0;

    isEditMode = true;

    LogMgr->AddLogGui(&logDialog);
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
        if (isTestPlay)
        {
            auto_ptr<Scene> LoadScene(new Scene);
            SceneMgr->AddScene("EditorLoadScene", LoadScene.get());
            SceneMgr->SetLoadScene("EditorLoadScene");

            auto_ptr<EditorLoadScene> EditorLoadGUI(new EditorLoadScene);
            SceneMgr->SetLoadGUIScene(EditorLoadGUI.get());

            auto_ptr<Scene> TestScene(new Scene);
            auto_ptr<GUI_Scene> EmptyGUI(new GUI_Scene);
            SceneMgr->SetGUIScene(EmptyGUI.get());

            TestScene->SetName(playScene);

            SceneMgr->AddScene(playScene, TestScene.get());
            TestScene->OnLoadSceneData();

            flag = SceneMgr->StartScene(playScene);
            isTestPlay = false;
        }
        else
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
            auto_ptr<Scene> LoadScene(new Scene);
            SceneMgr->AddScene("EditorLoadScene", LoadScene.get());
            SceneMgr->SetLoadScene("EditorLoadScene");
            
            auto_ptr<EditorLoadScene> EditorLoadGUI(new EditorLoadScene);
            SceneMgr->SetLoadGUIScene(EditorLoadGUI.get());
            
            auto_ptr<Scene> ViewScene(new Scene);
            SceneMgr->SetGUIScene(this);
            this->SetScriptProc(this);

            GameObject* MainCamera = new GameObject();
            EditorCamera* EditorCameraScript = new EditorCamera(MainCamera, this, "EditorCameraScript");
            MainCamera->AddComponent(EditorCameraScript);
            ViewScene->AddObject(MainCamera, "EditorCamera");
 
            GameObject* testSkinnedMesh = new GameObject();
            testSkinnedMesh->SetDefaultPipeLine();
            Transform3D* tr = new Transform3D();
            tr->SetPosition(0, 0, 0);
            tr->SetScale(10, 10, 10);
            testSkinnedMesh->AddComponent(tr);
            SkinnedMesh* mesh = new SkinnedMesh();
            mesh->SetFile("Skeleton.X");
            //mesh->SetAnimation("Skeleton_1H_banging_shield");
            testSkinnedMesh->AddComponent(mesh);
            ViewScene->AddObject(testSkinnedMesh, "Skinning");

            GameObject* Pivot = new GameObject();
            PosPivot* PosPivotScript = new PosPivot(this, Pivot, "PosPivotScript");
            Pivot->AddComponent(PosPivotScript);
            ViewScene->AddObject(Pivot, "PosPivot");
            
            ViewScene->SetAmbientColor(1.0f, 1.0f, 1.0f, 1.0f);
            ViewScene->SetName(OpenSceneName);

            SceneMgr->AddScene(OpenSceneName, ViewScene.get());

            if (!IsNewScene)
                ViewScene->OnLoadSceneData();

            ViewScene->SetEditorScene();

            playScene = OpenSceneName;
            flag = SceneMgr->StartScene(OpenSceneName);
        }
    } while (flag);
}

bool BoneEditor::IsEditMode()
{
    return isEditMode;
}

void BoneEditor::AddScriptList(std::string scriptName)
{
    scriptList.push_back(scriptName);
}

std::list<std::string> BoneEditor::GetScriptList()
{
    return scriptList;
}

void BoneEditor::SaveScene()
{
    LogMgr->Info("Clean Old Scene Data");
    SceneMgr->CurrentScene()->ClearSceneData();

    LogMgr->Info("%s - Save Scene...", SceneMgr->CurrentScene()->GetName().c_str());

    SceneMgr->CurrentScene()->SaveSceneData();

    auto DyamicObjectList = SceneMgr->CurrentScene()->GetObjectList();
    auto StaticObjectList = SceneMgr->CurrentScene()->GetStaticObjectList();

    for each(auto var in DyamicObjectList)
    {
        if (var->Tag() != "EditorObject")
        {
            LogMgr->Info("%s - Save Object", var->GetName().c_str());
 
            if (var->Tag() != "PointLight")   
                var->SavePrefab();
            var->SaveInMaps();
        }
    }

    for each(auto var in StaticObjectList)
    {
        if (var->Tag() != "EditorObject")
        {
            LogMgr->Info("%s - Save Object", var->GetName().c_str());

            if (var->Tag() != "PointLight")
                var->SavePrefab();
            
            var->SaveInMaps();
        }
    }

    LogMgr->Info("%s - Save...", SceneMgr->CurrentScene()->GetName().c_str());
}

void BoneEditor::ShowFileMenu()
{
    if (ImGui::MenuItem("Save", "Ctrl+S"))
        SaveScene();
    
    if (ImGui::MenuItem("Quit", "Alt+F4"))
    {
        SceneMgr->CurrentScene()->SetSceneFlag(true);
        SceneMgr->EndScene(SceneMgr->CurrentScene()->GetName());
    }
}

void BoneEditor::ShowViewMenu()
{
    if (ImGui::BeginMenu("FillMode"))
    {
        if (ImGui::MenuItem("POLYGON"))
            RenderMgr->GetDevice()->SetRenderState(D3DRS_FILLMODE, D3DFILL_SOLID);

        if (ImGui::MenuItem("WIRE_FRAME"))
            RenderMgr->GetDevice()->SetRenderState(D3DRS_FILLMODE, D3DFILL_WIREFRAME);

        ImGui::EndMenu();
    }

    if (ImGui::MenuItem("Show Environment"))
    {
        if (showEnvironmentSetting)
            showEnvironmentSetting = false;
        else
            showEnvironmentSetting = true;
    }

    if (ImGui::MenuItem("Show Light Icon"))
    {
        static bool show = false;
        auto DyamicObjectList = SceneMgr->CurrentScene()->GetObjectList();

        if (show)
            show = false;
        else
            show = true;

        for each(auto var in DyamicObjectList)
        {
            if (var->Tag() == "PointLight")
                ((PointLight*)var)->SetIcon(show);
        }
    }

    if (ImGui::MenuItem("Show Main Editor"))
    {
        if (showMainEditor)
            showMainEditor = false;
        else
            showMainEditor = true;
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
}

void BoneEditor::ShowEditorMenu()
{
    if (ImGui::MenuItem("Compile Script"))
    {
        RuntimeMgr->Compile();
    }

    if (ImGui::MenuItem("Enable Physics"))
    {
        auto Enable = SceneMgr->CurrentScene()->IsEnablePhysics();

        if (Enable)
            Enable = false;
        else
            Enable = true;
            
        SceneMgr->CurrentScene()->EnablePhysics(Enable);
    }

    if (ImGui::MenuItem("Game Play"))
    {
        SaveScene();

        isTestPlay = true;

        SceneMgr->CurrentScene()->SetSceneFlag(true);
        SceneMgr->EndScene(SceneMgr->CurrentScene()->GetName());
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
        auto object = SceneMgr->CurrentScene()->FindObjectByName(name);

        if (object == nullptr)
            return;

        static char buf[64] = "";
        
        ImGui::InputText("Name", buf, 64);
 
        if (ImGui::Button("Change"))
        {
            name = buf;

            object->SetName(name);
        }

        bool IsLocked = object->IsLockedEditor();
        ImGui::Checkbox("IsLocked", &IsLocked);

        object->LockEditor(IsLocked);
    }

    if (ImGui::CollapsingHeader("[Components]"), ImGuiTreeNodeFlags_DefaultOpen)
    {
        GameObject* object = SceneMgr->CurrentScene()->FindObjectByName(name);

        if (object != nullptr)
        {
            std::vector<Component*> components = object->GetComponents();

            for (auto iter = components.begin(); iter != components.end(); iter++)
            {
                if (ImGui::TreeNode((*iter)->GetTypeName().c_str()))
                {
                    if ((*iter)->GetTypeName() == "Transform3D")
                    {
                        Vec3 oriPos = ((Transform3D*)object->transform3D)->GetPosition();
                        Vec3 oriRot = ((Transform3D*)object->transform3D)->GetRotateAngle();
                        Vec3 oriScale = ((Transform3D*)object->transform3D)->GetScale();
                        
                        float pos[3] = { oriPos.x, oriPos.y, oriPos.z };
                        float rot[3] = { oriRot.x, oriRot.y, oriRot.z };
                        float scale[3] = { oriScale.x, oriScale.y, oriScale.z };
                        
                        ImGui::InputFloat3("Position", pos);
                        ImGui::InputFloat3("Rotation", rot);
                        ImGui::InputFloat3("Scale", scale);
                        
                        ((Transform3D*)object->transform3D)->SetPosition(pos[0], pos[1], pos[2]);
                        ((Transform3D*)object->transform3D)->SetRotate(rot[0], rot[1], rot[2]);
                        ((Transform3D*)object->transform3D)->SetScale(scale[0], scale[1], scale[2]);
                    }
                    else if ((*iter)->GetTypeName() == "StaticMesh")
                    {
                        auto MeshName = ((StaticMesh*)(*iter))->GetFile();
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
                Child->EnableScript(false);

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
    if (InputMgr->KeyDown(VK_F1, true))
        if (isEditMode)
            isEditMode = false;
        else
            isEditMode = true;

    if (!ImGui::Begin("Mode", nullptr, ImVec2(0, 0), 0.3f, ImGuiWindowFlags_AlwaysAutoResize | ImGuiWindowFlags_NoTitleBar | ImGuiWindowFlags_NoResize | ImGuiWindowFlags_NoMove | ImGuiWindowFlags_NoSavedSettings))
    {
        ImGui::End();
        return;
    }

    ImGui::SetWindowPos(ImVec2(10, 30));

    if (isEditMode)
        ImGui::Text("[F1 - Change View/Select Mode]");
    else
        ImGui::Text("[F1 - Change Editor Mode]");

    ImGui::End();

    if (!isEditMode)
        return;

    RuntimeMgr->Compile();

    bool isFocusedWindow = false;

    if (ImGui::BeginMainMenuBar())
    {
        if (ImGui::BeginMenu("File"))
        {
            ShowFileMenu();
            ImGui::EndMenu();
        }

        if (ImGui::BeginMenu("View"))
        {
            ShowViewMenu();
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

        ImGui::EndMainMenuBar();
    }

    if (showMainEditor)
    {
        if (!ImGui::Begin(" Scene GameObject List", &showMainEditor, ImGuiWindowFlags_ShowBorders | ImGuiWindowFlags_NoCollapse | ImGuiWindowFlags_NoMove | ImGuiWindowFlags_NoResize))
        {
            ImGui::End();
            return;
        }

        if (ImGui::IsRootWindowOrAnyChildFocused())
            isFocusedWindow = true;

        static int WindowHeight;
        
        if (showPrefabInspector)
            WindowHeight = RenderMgr->GetHeight() / 2 - 17;
        else
            WindowHeight = RenderMgr->GetHeight() - 18;

        ImGui::SetWindowSize(ImVec2(300, WindowHeight));
        ImVec2 Size = ImGui::GetWindowSize();
        ImGui::SetWindowPos(ImVec2(RenderMgr->GetWidth() - Size.x, 19));

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
                if ((*iter)->GetParent() != nullptr || (*iter)->Tag() == "EditorObject")
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
                    if ((*iter)->Tag() == "PointLight")
                    {
                        if (ImGui::TreeNode("Transform3D"))
                        {
                            Vec3 oriPos = ((Transform3D*)(*iter)->transform3D)->GetPosition();

                            float pos[3] = { oriPos.x, oriPos.y, oriPos.z };

                            ImGui::InputFloat3("Position", pos);

                            ((Transform3D*)(*iter)->transform3D)->SetPosition(pos[0], pos[1], pos[2]);

                            ImGui::TreePop();
                        }
                        if (ImGui::TreeNode("Color"))
                        {
                            bool oriActive = ((PointLight*)(*iter))->GetActive();
                            RGBA oriAmbient = ((PointLight*)(*iter))->GetAmbient();
                            RGBA oriDiffuse = ((PointLight*)(*iter))->GetDiffuse();
                            RGBA oriSpecular = ((PointLight*)(*iter))->GetSpecular();
                            float oriRadius = ((PointLight*)(*iter))->GetRadius();

                            float Ambient[4] = { oriAmbient.r, oriAmbient.g, oriAmbient.b, oriAmbient.a };
                            float Diffuse[4] = { oriDiffuse.r, oriDiffuse.g, oriDiffuse.b, oriDiffuse.a };
                            float Specular[4] = { oriSpecular.r, oriSpecular.g, oriSpecular.b, oriSpecular.a };

                            ImGui::Checkbox("Active", &oriActive);
                            ImGui::DragFloat("Radius", &oriRadius, 1.0f, 0.1f);
                            ImGui::InputFloat3("Ambient", Ambient);
                            ImGui::InputFloat3("Diffuse", Diffuse);
                            ImGui::InputFloat3("Specular", Specular);

                            ((PointLight*)(*iter))->SetActive(oriActive);
                            ((PointLight*)(*iter))->SetAmbient(Ambient[0], Ambient[1], Ambient[2], Ambient[3]);
                            ((PointLight*)(*iter))->SetDiffuse(Diffuse[0], Diffuse[1], Diffuse[2], Diffuse[3]);
                            ((PointLight*)(*iter))->SetSpecular(Specular[0], Specular[1], Specular[2], Specular[3]);
                            ((PointLight*)(*iter))->SetRadius(oriRadius);

                            ImGui::TreePop();
                        }
                    }
                    else
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
                            showPrefabInspector = true;

                            currentObjectName = (*iter)->GetName();
                        }
                    }

                    if (ImGui::SmallButton("Remove Object"))
                    {
                        SceneMgr->CurrentScene()->Destroy((*iter));
                    }

                    if (ImGui::SmallButton("Focus On"))
                    {
                        auto Object = SceneMgr->CurrentScene()->FindObjectByName((*iter)->GetName().c_str());

                        if (Object != nullptr)
                        {
                            auto Position = ((Transform3D*)Object->transform3D)->GetPosition();

                            ((Camera*)(SceneMgr->CurrentScene()->GetCurrentCamera()->GetComponent("Camera")))->SetTargetPosition(Position);

                            ((Transform3D*)SceneMgr->CurrentScene()->GetCurrentCamera()->transform3D)->SetPosition(
                                Position + Vec3(100, 100, 100)
                            );
                        }
                    }

                    ImGui::TreePop();
                }
            }
        }

        if (ImGui::CollapsingHeader("[New GameObject]", ImGuiTreeNodeFlags_DefaultOpen))
        {
            if (ImGui::TreeNode("New Prefabs"))
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
                    Object->EnableScript(false);

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
        
            if (ImGui::TreeNode("New Light"))
            {
                char* ComboBoxItems[] = { "Point", "Direction", "Spot" };
                
                static int CurItem = 0;
                ImGui::Combo("Typs", &CurItem, ComboBoxItems, 3);
                
                if (ImGui::Button("Create"))
                {
                    PointLight* Light = new PointLight;

                    Light->SetAmbient(1.0f, 1.0f, 1.0f, 1.0f);
                    Light->SetDiffuse(1.0f, 1.0f, 1.0f, 1.0f);
                    Light->SetSpecular(0.5f, 0.5f, 0.5f, 0.5f);
                    Light->SetRadius(200);
                    Light->SetLight(true);
                    Light->SetPosition(Vec3(0, 0, 0));
                    Light->SetTag("EditorObject");

                    int num = SceneMgr->CurrentScene()->GetPointLights().size();
                    char temp[100] = "";
                    itoa(num, temp, 10);

                    std::string name = "PointLight_";
                    name += temp;

                    SceneMgr->CurrentScene()->AddPointLight(Light);
                    SceneMgr->CurrentScene()->AddObject(Light, name);
                }

                ImGui::TreePop();
            }
        }
        
        ImGui::End();
    }

    if (showEnvironmentSetting)
    {
        std::string WindowName = "Environment" + currentShowInfoObject;
        ImGui::Begin(WindowName.c_str(), &showEnvironmentSetting, ImGuiWindowFlags_AlwaysAutoResize);

        if (ImGui::IsRootWindowOrAnyChildFocused())
            isFocusedWindow = true;

        if (ImGui::CollapsingHeader("[Settings]", ImGuiTreeNodeFlags_DefaultOpen))
        {
            if (ImGui::TreeNode("SkyBox"))
            {
                auto SkyBoxs = ResourceMgr->ExistFiles(".\\Resource\\Skybox\\*");

                const int Size = SkyBoxs.size();
                char** ListBoxItems = new char*[Size];

                int i = 0;
                for each(auto item in SkyBoxs)
                {
                    ListBoxItems[i] = new char[64];
                    strcpy(ListBoxItems[i], SkyBoxs[i].c_str());
                    i++;
                }

                static int CurItem = 0;
                ImGui::ListBox("Folder List", &CurItem, ListBoxItems, Size, 4);

                static char TypeName[64] = "tga";
                ImGui::InputText("ImageType", TypeName, 64);

                if (ImGui::Button("Chanage SkyBox"))
                    SceneMgr->CurrentScene()->SetSkybox(ListBoxItems[CurItem], TypeName);

                for (int i = 0; i<Size; i++)
                    delete[] ListBoxItems[i];
                delete[] ListBoxItems;

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
                auto ClearColor = SceneMgr->GetClearColor();
                static ImVec4 clear_col(ClearColor * 255.0f, ClearColor * 255.0f, ClearColor * 255.0f, 1);
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

        ImGui::End();
    }

    if (showPrefabInspector)
    {
        std::string WindowName = " Prefab Inspector : " + currentObjectName;

        ImGui::Begin(WindowName.c_str(), &showPrefabInspector,
            ImGuiWindowFlags_NoMove | ImGuiWindowFlags_NoCollapse | ImGuiWindowFlags_ShowBorders | ImGuiWindowFlags_NoResize);  //ImGuiWindowFlags_AlwaysAutoResize

        if (ImGui::IsRootWindowOrAnyChildFocused())
            isFocusedWindow = true;

        ImGui::SetWindowSize(ImVec2(300, RenderMgr->GetHeight() / 2));
        ImVec2 Size = ImGui::GetWindowSize();
        ImGui::SetWindowPos(ImVec2(RenderMgr->GetWidth() - Size.x, RenderMgr->GetHeight() / 2));
        
        ShowGameObjectTree(currentObjectName);

        ImGui::End();
    }

    if (showAddComponent)
    {
        std::string WindowName = " Add Component : " + currentShowInfoObject;
        ImGui::Begin(WindowName.c_str(), &showAddComponent, ImGuiWindowFlags_NoCollapse | ImGuiWindowFlags_ShowBorders | ImGuiWindowFlags_AlwaysAutoResize);

        if (ImGui::IsRootWindowOrAnyChildFocused())
            isFocusedWindow = true;

        const char* listbox_items[] = { "StaticMesh", "Collision", "RigidBody", "SkinnedMesh", "Sound", "Material", "Script", "Camera", "TrailRenderer", "BillBoard", "SpriteBillBoard" };
        static int listbox_item_current = 0;

        if (ImGui::CollapsingHeader("[Select Component]"), ImGuiTreeNodeFlags_DefaultOpen)
        {
            ImGui::ListBox("Component\nTypes\n", &listbox_item_current, listbox_items, IM_ARRAYSIZE(listbox_items), 4);
        }

        if (ImGui::CollapsingHeader("[Detail]"), ImGuiTreeNodeFlags_DefaultOpen)
        {
            switch (listbox_item_current) {
            // StaticMesh
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
                    Mesh->SetFile(ComboBoxItems[CurItem]);
                    Mesh->LoadContent();
                    auto Object = SceneMgr->CurrentScene()->FindObjectByName(currentShowInfoObject);
                    Object->AddComponent(Mesh);

                    showAddComponent = false;
                }
            }
            break;

            // Collision
            case 1:
            {
                char* ComboBoxItems[] = { "Box", "Sphere", "Cone", "Cylinder", "Capsule" };

                static int CurItem = 0;
                ImGui::Combo("Collisions", &CurItem, ComboBoxItems, 3);

                static bool AutoCompute = true;
                static float Radius = 1.0f;
                static float Height = 1.0f;
                static float BoxVector[] = { 1.0f, 1.0f, 1.0f };

                if (CurItem == 0 || CurItem == 1)
                    ImGui::Checkbox("Auto Compute", &AutoCompute);

                if (CurItem == 0)
                    ImGui::DragFloat3("BoxVector", BoxVector, 1.0f, 0.0f);

                if (CurItem != 0)
                    ImGui::DragFloat("Radius", &Radius, 1.0f, 0.1f);

                if (CurItem == 2 || CurItem == 3 || CurItem == 4)
                    ImGui::DragFloat("Height", &Height, 1.0f, 0.1f);

                if (ImGui::Button("Add Component"))
                {
                    auto Object = SceneMgr->CurrentScene()->FindObjectByName(currentShowInfoObject);

                    Collision* Coll = new Collision(Object);

                    bool Init = true;

                    if (CurItem == 0)
                    {
                        if (AutoCompute)
                        {
                            auto Mesh = (StaticMesh*)Object->GetComponent("StaticMesh");

                            if (Mesh != nullptr)
                                Coll->ComputeBoundingBox(ResourceMgr->LoadMesh((Mesh)->GetFile())->mesh);
                            else
                                Init = false;
                        }
                        else
                            Coll->CreateBox(reactphysics3d::Vector3(BoxVector[0], BoxVector[1], BoxVector[2]));
                    }
                    else if (CurItem == 1)
                    {
                        if (AutoCompute)
                        {
                            auto Mesh = (StaticMesh*)Object->GetComponent("StaticMesh");

                            if (Mesh != nullptr)
                                Coll->ComputeBoundingSphere(ResourceMgr->LoadMesh((Mesh)->GetFile())->mesh);
                            else
                                Init = false;
                        }
                        else
                            Coll->CreateSphere(Radius);
                    }
                    else if (CurItem == 2)
                        Coll->CreateCone(Radius, Height);
                    else if (CurItem == 3)
                        Coll->CreateCone(Radius, Height);
                    else if (CurItem == 4)
                        Coll->CreateCylinder(Radius, Height);
                    else if (CurItem == 5)
                        Coll->CreateCapsule(Radius, Height);

                    if (Init)
                    {
                        Object->AddComponent(Coll);

                        showAddComponent = false;
                    }
                }
            }
            break;

            // RigidBody
            case 2:
            {
                char* ComboBoxItems[] = { "Static", "Kemetic",  "Dynamic" };

                static int CurItem = 0;
                ImGui::Combo("Types", &CurItem, ComboBoxItems, 3);

                static bool EnableGravity = false;
                static bool IsAllowedToSleep = false;
                static float Bounciness = 0.4f;
                static float FrictionCoefficient = 0.2f;
                static float Mass = 1.0f;

                ImGui::Checkbox("Enable Gravity", &EnableGravity);
                ImGui::Checkbox("Allow To Sleep", &IsAllowedToSleep);
                ImGui::DragFloat("Bounciness", &Bounciness, 1.0f, 0.1f);
                ImGui::DragFloat("FrictionCoefficient", &FrictionCoefficient, 1.0f, 0.1f);
                ImGui::DragFloat("Mass", &Mass, 1.0f, 0.1f);

                if (ImGui::Button("Add Component"))
                {
                    auto Object = SceneMgr->CurrentScene()->FindObjectByName(currentShowInfoObject);

                    RigidBody* rigidBody = new RigidBody();

                    if (rigidBody->SetInfo(Object, Mass))
                    {
                        rigidBody->EnableGravity(EnableGravity);
                        rigidBody->SetBounciness(Bounciness);
                        rigidBody->SetFrictionCoefficient(FrictionCoefficient);
                        rigidBody->SetIsAllowedToSleep(IsAllowedToSleep);
                        rigidBody->SetType(reactphysics3d::BodyType(CurItem));

                        Object->AddComponent(rigidBody);

                        showAddComponent = false;
                    }
                    else
                        delete rigidBody;
                }
            }
            break;

            // SkinnedMesh
            case 3:
            {
            }
            break;

            // Sound
            case 4:
            {
            }
            break;

            // Material
            case 5:
            {
                static float Ambient[4] = { 1.0f, 1.0f, 1.0f, 1.0f };
                static float Diffuse[4] = { 1.0f, 1.0f, 1.0f, 1.0f };
                static float Emissive[4] = { 1.0f, 1.0f, 1.0f, 1.0f };
                static float Specular[4] = { 1.0f, 1.0f, 1.0f, 1.0f };
                static float Shininess = 1.0f;

                ImGui::InputFloat3("Ambient", Ambient);
                ImGui::InputFloat3("Diffuse", Diffuse);
                ImGui::InputFloat3("Emissive", Emissive);
                ImGui::InputFloat3("Specular", Specular);
                ImGui::InputFloat("Shininess", &Shininess);

                if (ImGui::Button("Add Component"))
                {
                    Material* Mat = new Material();

                    Mat->SetAmbient(Ambient[0], Ambient[1], Ambient[2], Ambient[3]);
                    Mat->SetDiffuse(Diffuse[0], Diffuse[1], Diffuse[2], Diffuse[3]);
                    Mat->SetEmissive(Emissive[0], Emissive[1], Emissive[2], Emissive[3]);
                    Mat->SetSpecular(Specular[0], Specular[1], Specular[2], Specular[3]);
                    Mat->SetShininess(Shininess);

                    Mat->LoadContent();

                    auto Object = SceneMgr->CurrentScene()->FindObjectByName(currentShowInfoObject);
                    Object->AddComponent(Mat);

                    showAddComponent = false;
                }
            }
            break;

            // Script
            case 6:
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

            // Camera
            case 7:
            {

            }
            break;

            // TrailRenderer
            case 8:
            {

            }
            break;

            // BillBoard
            case 9:
            {

            }
            break;

            // SpriteBillBoard
            case 10:
            {

            }
            break;

            }
        }
        ImGui::End();
    }

    if (showLogWindow)
        logDialog.Render(" Log Dialog", &showLogWindow);
    
    if (isFocusedWindow)
        InputMgr->SetFocusWindow(false);
    else
        InputMgr->SetFocusWindow(true);
}

void BoneEditor::SelectObject(std::string name)
{
    showPrefabInspector = true;

    currentObjectName = name;
}