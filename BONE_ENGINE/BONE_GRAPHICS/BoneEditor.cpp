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
#include "RuntimeCompiler.h"
#include "LogManager.h"
#include "GraphNode.h"
#include "SoundClip.h"
#include "ParticleSystem.h"

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
    ImGui::Begin(title, p_open, ImVec2(RenderMgr->GetWidth() - 450, 200), -1.0f, ImGuiWindowFlags_NoResize | ImGuiWindowFlags_NoMove | ImGuiWindowFlags_NoCollapse);
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
            Scene* LoadScene = new Scene();
            SceneMgr->AddScene("EditorLoadScene", LoadScene);
            SceneMgr->SetLoadScene("EditorLoadScene");

            EditorLoadScene* EditorLoadGUI = new EditorLoadScene();
            SceneMgr->SetLoadGUIScene(EditorLoadGUI);

            Scene* TestScene = new Scene();
            GUI_Scene* EmptyGUI = new GUI_Scene();
            SceneMgr->SetGUIScene(EmptyGUI);

            TestScene->SetName(playScene);

            SceneMgr->AddScene(playScene, TestScene);
            TestScene->OnLoadSceneData();

            flag = SceneMgr->StartScene(playScene);
            isTestPlay = false;
        }
        else
        {
            SceneInfoUI* EditorTtitleUI = new SceneInfoUI();
            SceneMgr->SetGUIScene(EditorTtitleUI);

            Scene* SceneInfo = new Scene();

            SceneMgr->AddScene("InfoScene", SceneInfo);
            flag = SceneMgr->StartScene("InfoScene");

            OpenSceneName = EditorTtitleUI->GetSceneName();
            IsNewScene = EditorTtitleUI->IsNewScene();
        }

        if (flag)
        {
            Scene* LoadScene = new Scene();
            SceneMgr->AddScene("EditorLoadScene", LoadScene);
            SceneMgr->SetLoadScene("EditorLoadScene");
            
            EditorLoadScene* EditorLoadGUI = new EditorLoadScene();
            SceneMgr->SetLoadGUIScene(EditorLoadGUI);
            
            Scene* ViewScene = new Scene();
            SceneMgr->SetGUIScene(this);
            this->SetScriptProc(this);

            GameObject* MainCamera = new GameObject();
            EditorCamera* EditorCameraScript = new EditorCamera(MainCamera, this, "EditorCameraScript");
            MainCamera->AddComponent(EditorCameraScript);
            ViewScene->AddObject(MainCamera, "EditorCamera");
 
            GameObject* Pivot = new GameObject();
            PosPivot* PosPivotScript = new PosPivot(this, Pivot, "PosPivotScript");
            Pivot->AddComponent(PosPivotScript);
            ViewScene->AddObject(Pivot, "PosPivot");
            
            ViewScene->SetAmbientColor(1.0f, 1.0f, 1.0f, 1.0f);
            ViewScene->SetName(OpenSceneName);

            SceneMgr->AddScene(OpenSceneName, ViewScene);

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
    CUR_SCENE->ClearSceneData();

    LogMgr->Info("%s - Save Scene...", CUR_SCENE->GetName().c_str());

    CUR_SCENE->SaveSceneData();

    auto DyamicObjectList = CUR_SCENE->GetObjectList();
    auto StaticObjectList = CUR_SCENE->GetStaticObjectList();
    
    for each(auto var in DyamicObjectList)
    {
        if (var->Tag() != "EditorObject")
        {
            LogMgr->Info("%s - Save Object", var->GetName().c_str());
 
            var->SavePrefab();
            var->SaveInMaps();
        }
    }

    for each(auto var in StaticObjectList)
    {
        if (var->Tag() != "EditorObject")
        {
            LogMgr->Info("%s - Save Object", var->GetName().c_str());

            var->SavePrefab();
            var->SaveInMaps();
        }
    }

    LogMgr->Info("%s - Save...", CUR_SCENE->GetName().c_str());
}

void BoneEditor::ShowFileMenu()
{
    if (ImGui::MenuItem("Save", "Ctrl+S"))
        SaveScene();
    
    if (ImGui::MenuItem("Quit", "Alt+F4"))
    {
        CUR_SCENE->SetSceneFlag(true);
        SceneMgr->EndScene(CUR_SCENE->GetName());
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

    if (ImGui::MenuItem("Setting Environment"))
    {
        if (showEnvironmentSetting)
            showEnvironmentSetting = false;
        else
            showEnvironmentSetting = true;
    }

    if (ImGui::MenuItem("Show Main Editor"))
    {
        if (showMainEditor)
            showMainEditor = false;
        else
            showMainEditor = true;
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
        auto Enable = CUR_SCENE->IsEnablePhysics();

        if (Enable)
            Enable = false;
        else
            Enable = true;
            
        CUR_SCENE->EnablePhysics(Enable);
    }

    if (ImGui::MenuItem("ShadowMap Build"))
    {
        CUR_SCENE->CreateShadowMap();
    }

    if (ImGui::MenuItem("Sort Object Render"))
    {
        CUR_SCENE->SortRenderOptimizeObject();
    }

    if (ImGui::MenuItem("Game Play"))
    {
        SaveScene();

        isTestPlay = true;

        CUR_SCENE->SetSceneFlag(true);
        SceneMgr->EndScene(CUR_SCENE->GetName());
    }
}

void BoneEditor::ShowGraphNodeInfo(GraphNode* node)
{
    if (ImGui::TreeNode(node->GetName().c_str()))
    {
        float pos[3] = {
            node->GetPosition().x,
            node->GetPosition().y,
            node->GetPosition().z
        };

        ImGui::DragFloat3("Position", pos, 1.0f);

        node->SetPosition(Vec3(pos[0], pos[1], pos[2]));
        
        static char Name[20];
        strcpy(Name, node->GetName().c_str());

        ImGui::InputText("Name", Name, 20);

        if (ImGui::Button("Change"))
            node->SetName(Name);

        if (ImGui::TreeNode("Connections"))
        {
            for each(auto var in node->GetConnections())
                ImGui::Text(var.c_str());

            ImGui::Separator();

            auto GraphNodes = CUR_SCENE->GetGraphNodes();
            
            const int Size = (*GraphNodes).size() - 1;
            char** ComboBoxItems = new char*[Size];

            static int CurItem = 0;

            int i = 0;
            for each(auto item in *GraphNodes)
            {
                if (item.second->GetName() == node->GetName())
                    continue;

                ComboBoxItems[i] = new char[64];
                strcpy(ComboBoxItems[i], item.second->GetName().c_str());
                
                i++;
            }

            ImGui::Combo("Nodes", &CurItem, ComboBoxItems, Size);

            if (ImGui::Button("Connect"))
                node->ConnectNode(ComboBoxItems[CurItem]);
            else if (ImGui::Button("Disconnect"))
                node->DisconnectNode(ComboBoxItems[CurItem]);

            for (int i = 0; i < Size; i++)
                delete ComboBoxItems[i];
            delete[] ComboBoxItems;

            ImGui::TreePop();
        }

        if (ImGui::Button("Remove"))
            CUR_SCENE->RemoveGraphNode(node);
        
        ImGui::TreePop();
    }
}

void BoneEditor::ShowHelpMenu()
{
    if (ImGui::MenuItem("Info")) {}
    ImGui::Text("Version 1.0");
}

void BoneEditor::ShowObjectInfo(std::string name)
{
    if (ImGui::CollapsingHeader("[GameObject Info]"))
    {
        auto object = CUR_SCENE->FindObjectByName(name);

        if (object == nullptr)
            return;

        static char buf[64] = "";
        
        ImGui::InputText("Name", buf, 64);
 
        if (ImGui::Button("Change"))
        {
            name = buf;
            object->SetName(name);
            strcpy(buf, "");
        }

        bool IsLocked = object->IsLockedEditor();
        ImGui::Checkbox("IsLocked", &IsLocked);

        object->LockEditor(IsLocked);
    }

    if (ImGui::CollapsingHeader("[Components]"))
    {
        GameObject* object = CUR_SCENE->FindObjectByName(name);

        if (object != nullptr)
        {
            auto components = object->GetComponents();

            for (auto item = components.begin(); item != components.end();)
            {
                if (ImGui::TreeNode(item->first.c_str()))
                {
                    if (item->first == "SpriteBillBoard")
                    {
                        float Width = ((SpriteBillBoard*)item->second)->GetSpriteInfo().width;
                        float Height = ((SpriteBillBoard*)item->second)->GetSpriteInfo().height;
                        int Cut = ((SpriteBillBoard*)item->second)->GetSpriteInfo().animationCut;
                        int Scene = ((SpriteBillBoard*)item->second)->GetSpriteInfo().animationScene;
                        float Speed = ((SpriteBillBoard*)item->second)->GetSpeed();
                        float Alpha = ((SpriteBillBoard*)item->second)->GetAlpha();
                        bool Full = ((SpriteBillBoard*)item->second)->IsFullAnimation();
                        bool isTargetCamera = ((SpriteBillBoard*)item->second)->IsTargetCamera();

                        ImGui::InputFloat("Width", &Width);
                        ImGui::InputFloat("Height", &Height);
                        ImGui::DragFloat("Speed", &Speed);
                        ImGui::InputInt("Cut", &Cut);
                        ImGui::InputInt("Scene", &Scene);
                        ImGui::InputFloat("Alpha", &Alpha);
                        ImGui::Checkbox("FullAnimation", &Full);
                        ImGui::Checkbox("IsTargetCamera", &isTargetCamera);

                        ((SpriteBillBoard*)item->second)->SetAnimation(Width, Height, Cut, Scene, Alpha);
                        ((SpriteBillBoard*)item->second)->IsFullAnimation(Full);
                        ((SpriteBillBoard*)item->second)->SetTargetCamera(isTargetCamera);
                        ((SpriteBillBoard*)item->second)->SetSpeed(Speed);
                    }
                    else if (item->first == "FireworkParticle")
                    {
                        static int NumOfParticle = ((FireworkParticle*)item->second)->GetNumOfParticles();
                        static int Size = ((FireworkParticle*)item->second)->GetSize();

                        ImGui::DragInt("NumOfParticle", &NumOfParticle);
                        ImGui::DragInt("Size", &Size);

                        if (ImGui::Button("Change"))
                        {
                            ((FireworkParticle*)item->second)->RemoveAllParticle();
                            ((FireworkParticle*)item->second)->Init(object, NumOfParticle, Size);
                            ((FireworkParticle*)item->second)->Reset();
                        }
                    }
                    else if (item->first == "Transform3D")
                    {
                        Vec3 oriPos = ((Transform3D*)item->second)->GetPosition();
                        Vec3 oriRot = ((Transform3D*)item->second)->GetRotateAngle();
                        Vec3 oriScale = ((Transform3D*)item->second)->GetScale();
                        
                        float pos[3] = { oriPos.x, oriPos.y, oriPos.z };
                        float rot[3] = { oriRot.x, oriRot.y, oriRot.z };
                        float scale[3] = { oriScale.x, oriScale.y, oriScale.z };
                        
                        ImGui::DragFloat3("Position", pos);
                        ImGui::DragFloat3("Rotation", rot);
                        ImGui::DragFloat3("Scale", scale);
                        
                        ((Transform3D*)item->second)->SetPosition(pos[0], pos[1], pos[2]);
                        ((Transform3D*)item->second)->SetRotate(rot[0], rot[1], rot[2]);
                        ((Transform3D*)item->second)->SetScale(scale[0], scale[1], scale[2]);
                    }
                    else if (item->first == "StaticMesh")
                    {
                        auto MeshName = ((StaticMesh*)(item->second))->GetFile();
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
                            item++;
                            object->RemoveComponent("StaticMesh");

                            StaticMesh* mesh = new StaticMesh();
                            mesh->SetFile(ComboBoxItems[CurItem]);
                            mesh->LoadContent();
                            object->AddComponent(mesh);
                            ImGui::TreePop();
                            continue;
                        }

                        ImGui::SameLine();

                        for (int i = 0; i < Size; i++)
                            delete ComboBoxItems[i];
                        delete[] ComboBoxItems;

                        if (ImGui::Button("Remove"))
                        {
                            item++;
                            object->RemoveComponent("StaticMesh");
                            ImGui::TreePop();
                            continue;
                        }
                    }
                    else if (item->first == "Material")
                    {
                        RGBA OriAmbient = ((Material*)(item->second))->GetAmbient();
                        RGBA OriDiffuse = ((Material*)(item->second))->GetDiffuse();
                        RGBA OriEmissive = ((Material*)(item->second))->GetEmissive();
                        RGBA OriSpecular = ((Material*)(item->second))->GetSpecular();
                        float OriShininess = ((Material*)(item->second))->GetShininess();

                        float Ambient[4] = { OriAmbient.r, OriAmbient.g, OriAmbient.b, OriAmbient.a };
                        float Diffuse[4] = { OriDiffuse.r, OriDiffuse.g, OriDiffuse.b, OriDiffuse.a };
                        float Emissive[4] = { OriEmissive.r, OriEmissive.g, OriEmissive.b, OriEmissive.a };
                        float Specular[4] = { OriSpecular.r, OriSpecular.g, OriSpecular.b, OriSpecular.a };
                        float Shininess = OriShininess;

                        ImGui::DragFloat3("Ambient", Ambient, 0.1f);
                        ImGui::DragFloat3("Diffuse", Diffuse, 0.1);
                        ImGui::DragFloat3("Emissive", Emissive, 0.1);
                        ImGui::DragFloat3("Specular", Specular, 0.1);
                        ImGui::DragFloat("Shininess", &Shininess, 0.1);

                        ((Material*)(item->second))->SetAmbient(Ambient[0], Ambient[1], Ambient[2], Ambient[3]);
                        ((Material*)(item->second))->SetDiffuse(Diffuse[0], Diffuse[1], Diffuse[2], Diffuse[3]);
                        ((Material*)(item->second))->SetEmissive(Emissive[0], Emissive[1], Emissive[2], Emissive[3]);
                        ((Material*)(item->second))->SetSpecular(Specular[0], Specular[1], Specular[2], Specular[3]);
                        ((Material*)(item->second))->SetShininess(Shininess);
                    }
                    else if (item->first == "SkinnedMesh")
                    {
                        auto AnimationSet = ((SkinnedMesh*)(item->second))->GetAnmimationSet();

                        const int AnimationSize = AnimationSet.size();
                        char** AnimationsCombo = new char*[AnimationSize];

                        static int CurItem = 0;

                        int i = 0;
                        for each(auto iter in AnimationSet)
                        {
                            AnimationsCombo[i] = new char[64];
                            strcpy(AnimationsCombo[i], iter.first.c_str());

                            if (((SkinnedMesh*)(item->second))->GetCurrentAnimation() == AnimationsCombo[i])
                                CurItem = i;

                            i++;
                        }

                        ImGui::Combo("Animations", &CurItem, AnimationsCombo, AnimationSize);
                        
                        if (ImGui::Button("Chanage"))
                            ((SkinnedMesh*)(item->second))->SetAnimation(AnimationsCombo[CurItem]);

                        auto MeshName = ((SkinnedMesh*)(item->second))->GetFile();
                        auto Meshes = ResourceMgr->ExistFiles(".\\Resource\\Mesh\\*");

                        const int MeshSize = Meshes.size();
                        char** MeshesCombo = new char*[MeshSize];

                        static int CurItem2 = 0;

                        i = 0;
                        for each(auto iter in Meshes)
                        {
                            MeshesCombo[i] = new char[64];
                            strcpy(MeshesCombo[i], Meshes[i].c_str());

                            if (MeshName == MeshesCombo[i])
                                CurItem2 = i;

                            i++;
                        }

                        ImGui::Combo("Meshes", &CurItem2, MeshesCombo, MeshSize);

                        if (ImGui::Button("Chanage"))
                        {
                            item++;
                            object->RemoveComponent("SkinnedMesh");

                            SkinnedMesh* mesh = new SkinnedMesh();
                            mesh->SetFile(MeshesCombo[CurItem2]);
                            mesh->LoadContent();
                            object->AddComponent(mesh);

                            for (int i = 0; i < AnimationSize; i++)
                                delete AnimationsCombo[i];
                            delete[] AnimationsCombo;

                            for (int i = 0; i < MeshSize; i++)
                                delete MeshesCombo[i];
                            delete[] MeshesCombo;

                            continue;
                        }

                        for (int i = 0; i < AnimationSize; i++)
                            delete AnimationsCombo[i];
                        delete[] AnimationsCombo;

                        for (int i = 0; i < MeshSize; i++)
                            delete MeshesCombo[i];
                        delete[] MeshesCombo;

                        ImGui::SameLine();

                        if (ImGui::Button("Remove"))
                        {
                            item++;
                            object->RemoveComponent("SkinnedMesh");
                            ImGui::TreePop();
                            continue;
                        }

                    }
                    else if (item->first == "Collision")
                    {
                        auto CollisionType = ((Collision*)(item->second))->GetCollisionType();
                        
                        switch (CollisionType)
                        {
                        case Collision::COLL_BOX:
                        {
                            ImGui::Text("Type : Box");
                            
                            float HalfExtens[] = {
                                ((Collision*)(item->second))->GetHalfExtens().x,
                                ((Collision*)(item->second))->GetHalfExtens().y,
                                ((Collision*)(item->second))->GetHalfExtens().z
                            };

                            ImGui::DragFloat3("HalfExtens", HalfExtens, 0.1f, 0.1f);

                            ((Collision*)(item->second))->SetHalfExtens(Vec3(HalfExtens[0], HalfExtens[1], HalfExtens[2]));
                        }
                        break;

                        case Collision::COLL_SPHERE:
                        {
                            ImGui::Text("Type : Sphere");
                            ImGui::Text("Radius : %f", ((Collision*)item->second)->GetRadius());
                        }
                        break;

                        case Collision::COLL_CAPSULE:
                        {
                            ImGui::Text("Type : Capsule");
                            ImGui::Text("Radius : %f", ((Collision*)item->second)->GetRadius());
                            ImGui::Text("Height : %f", ((Collision*)item->second)->GetHeight());
                        }
                        break;

                        case Collision::COLL_CONE:
                        {
                            ImGui::Text("Type : Cone");
                            ImGui::Text("Radius : %f", ((Collision*)item->second)->GetRadius());
                            ImGui::Text("Height : %f", ((Collision*)item->second)->GetHeight());
                        }
                        break;

                        case Collision::COLL_CYLINDER:
                        {
                            ImGui::Text("Type : Cylinder");
                            ImGui::Text("Radius : %f", ((Collision*)item->second)->GetRadius());
                            ImGui::Text("Height : %f", ((Collision*)item->second)->GetHeight());
                        }
                        break;
                        }

                        if (object->GetComponent("RigidBody") == nullptr)
                        {
                            if (ImGui::Button("Remove"))
                            {
                                item++;
                                object->RemoveComponent("Collision");
                                ImGui::TreePop();
                                continue;
                            }
                        }
                    }
                    else if (item->first == "RigidBody")
                    {
                        auto RigidBodyType = ((RigidBody*)item->second)->GetType();
                        auto Bounciness = ((RigidBody*)item->second)->GetBounciness();
                        auto FrictionCoefficient = ((RigidBody*)item->second)->GetFrictionCoefficient();
                        auto IsAllowedToSleep = ((RigidBody*)item->second)->GetIsAllowedToSleep();
                        auto Mass = ((RigidBody*)item->second)->GetMass();
                        float PosOnPivot[3] = {
                            ((RigidBody*)item->second)->GetPosOnPivot().x,
                            ((RigidBody*)item->second)->GetPosOnPivot().y,
                            ((RigidBody*)item->second)->GetPosOnPivot().z
                        };

                        bool LockRoataion[3] = {
                            ((RigidBody*)item->second)->IsLockedRotationX(),
                            ((RigidBody*)item->second)->IsLockedRotationY(),
                            ((RigidBody*)item->second)->IsLockedRotationZ()
                        };

                        char* Typs[3] = { "Static", "Kinemetic", "Dynamic"};
                        int CurItem = RigidBodyType;
                        ImGui::Combo("Animations", &CurItem, Typs, 3);
                        ImGui::DragFloat("Bounciness", &Bounciness, 0.01f, 0.0f, 1.0f);
                        ImGui::DragFloat("FrictionCoefficient", &FrictionCoefficient, 0.01f, 0.0f, 1.0f);
                        ImGui::DragFloat("Mass", &Mass, 0.1f);
                        ImGui::DragFloat3("PosOnPivot", PosOnPivot);
                        ImGui::Checkbox("IsAllowedToSleep", &IsAllowedToSleep);
                        ImGui::Checkbox("LockRotationX", &LockRoataion[0]);
                        ImGui::Checkbox("LockRotationY", &LockRoataion[1]);
                        ImGui::Checkbox("LockRotationZ", &LockRoataion[2]);

                        ((RigidBody*)item->second)->SetType((BodyType)CurItem);
                        ((RigidBody*)item->second)->SetBounciness(Bounciness);
                        ((RigidBody*)item->second)->SetFrictionCoefficient(FrictionCoefficient);
                        ((RigidBody*)item->second)->SetIsAllowedToSleep(IsAllowedToSleep);
                        ((RigidBody*)item->second)->SetMass(Mass);
                        ((RigidBody*)item->second)->SetPosOnPivot(Vec3(PosOnPivot[0], PosOnPivot[1], PosOnPivot[2]));
                        ((RigidBody*)item->second)->LockRotation(
                            LockRoataion[0],
                            LockRoataion[1],
                            LockRoataion[2]
                        );

                        if (ImGui::Button("Remove"))
                        {
                            item++;
                            object->RemoveComponent("RigidBody");
                            ImGui::TreePop();
                            continue;
                        }
                    }
                    else if (item->first == "SoundClip")
                    {
                        if (ImGui::TreeNode("Clips"))
                        {
                            for each (auto var in *((SoundClip*)item->second)->GetClips())
                            {
                                if (ImGui::TreeNode(var.first.c_str()))
                                {
                                    auto Loop = var.second.loop;
                                    auto MinDist = var.second.minDist;
                                    auto MaxDist = var.second.maxDist;
                                    auto StartPaused = var.second.startPaused;
                                    auto Volume = var.second.volume;

                                    ImGui::Checkbox("Loop", &Loop);
                                    ImGui::Checkbox("StartPaused", &StartPaused);
                                    ImGui::DragFloat("MinDist", &MinDist);
                                    ImGui::DragFloat("MaxDist", &MaxDist);
                                    ImGui::DragFloat("Volume", &Volume);

                                    ((SoundClip*)item->second)->ChangeInfo(var.first, Volume, Loop, StartPaused, MinDist, MaxDist);

                                    if (ImGui::Button("Play"))
                                        ((SoundClip*)item->second)->Play(var.first);

                                    if (ImGui::Button("Stop"))
                                        ((SoundClip*)item->second)->Stop(var.first);

                                    if (ImGui::Button("Remove"))
                                    {
                                        ((SoundClip*)item->second)->RemoveClip(var.first);
                                    
                                        ImGui::TreePop();
                                        break;
                                    }

                                    ImGui::TreePop();
                                }
                            }

                            ImGui::TreePop();
                        }
                        
                        if (ImGui::TreeNode("New Clips"))
                        {
                            auto Sounds = ResourceMgr->ExistFiles(".\\Resource\\Sound\\*");

                            const int Size = Sounds.size();
                            char** ComboBoxItems = new char*[Size - 1];

                            int i = 0;
                            for each(auto iter in Sounds)
                            {
                                ComboBoxItems[i] = new char[64];
                                strcpy(ComboBoxItems[i], iter.c_str());
                                i++;
                            }

                            static int CurItem = 0;
                            ImGui::Combo("Sounds", &CurItem, ComboBoxItems, Size);

                            static bool Loop;
                            static float MinDist;
                            static float MaxDist;
                            static bool StartPaused;
                            static float Volume;
                            
                            ImGui::Checkbox("Loop", &Loop);
                            ImGui::Checkbox("StartPaused", &StartPaused);
                            ImGui::DragFloat("MinDist", &MinDist);
                            ImGui::DragFloat("MaxDist", &MaxDist);
                            ImGui::DragFloat("Volume", &Volume);

                            if (ImGui::Button("Add New Clip"))
                                ((SoundClip*)item->second)->AddClip(ComboBoxItems[CurItem], Volume, Loop, StartPaused, MinDist, MaxDist);

                            for (int i = 0; i < Size; i++)
                                delete ComboBoxItems[i];
                            delete[] ComboBoxItems;

                            ImGui::TreePop();
                        }
                    }

                    ImGui::TreePop();
                }

                item++;
            }

            if (ImGui::SmallButton("Add Component"))
            {
                currentShowInfoObject = name;
                showAddComponent = true;
            }
        }
    }
}

void BoneEditor::ShowEditObjectTree(std::string treeName)
{
    auto parent = CUR_SCENE->FindObjectByName(treeName);

    if (parent == nullptr)
        return;

    ShowObjectInfo(treeName);

    if (ImGui::CollapsingHeader("[Childs]"))
    {
        auto childs = parent->GetChileds();

        for (auto iter = childs.begin(); iter != childs.end(); iter++)
        {
            if (ImGui::TreeNode((*iter)->GetName().c_str()))
            {
                ShowEditObjectTree((*iter)->GetName());
                
                if (ImGui::SmallButton("Remove Object"))
                {
                    (*iter)->DetachParent();
                    CUR_SCENE->Destroy((*iter));
                }

                ImGui::TreePop();
            }
        }

        if (ImGui::TreeNode("[Add New Child]"))
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
                    CUR_SCENE->AddObject(Child, ChildName);
                }
                else
                {
                    Child->SetPrfabName(Name);
                    Child->SetName(Name);
                    Child->SetPriority(1);
                    CUR_SCENE->AddObject(Child, Name);
                }

                parent->AttachChild(Child);
                childSize++;
            }

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
    {
        string F1String;

        if (currentObjectName != "")
            F1String = "[F1 - Change View/Select Mode] - Selected " + currentObjectName;
        else
            F1String = "[F1 - Change View/Select Mode]";

        ImGui::Text(F1String.c_str());
    }
    else
    {
        string F1String;

        if (currentObjectName != "")
            F1String = "[F1 - Change Editor Mode] - Selected " + currentObjectName;
        else
            F1String = "[F1 - Change Editor Mode]";

        ImGui::Text(F1String.c_str());
    }

    ImGui::End();

    if (!isEditMode)
        return;

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

        static bool ShowLights = false;

        ImGui::Checkbox("Show Lights", &ShowLights);

        auto PointLights = CUR_SCENE->GetPointLights();

        for each(auto var in PointLights)
            ((PointLight*)var)->SetIcon(ShowLights);
    

        static bool ShowNodeLists = false;
        ImGui::SameLine();
        ImGui::Checkbox("Show Graph Nodes", &ShowNodeLists);
        
        auto NodeList = CUR_SCENE->GetGraphNodes();

        for each(auto var in *NodeList)
            ((GraphNode*)var.second)->SetIcon(ShowNodeLists);


        auto DyamicObjectList = CUR_SCENE->GetObjectList();
        auto StaticObjectList = CUR_SCENE->GetStaticObjectList();

        static bool EnableMeshBox = false;
        ImGui::SameLine();
        ImGui::Checkbox("Show Collision", &EnableMeshBox);
        
        for each(auto var in DyamicObjectList)
        {
            auto coll = GET_COLLISION(var);
            if (coll != nullptr)
                coll->ShowShape(EnableMeshBox);
        }

        for each(auto var in StaticObjectList)
        {
            auto coll = GET_COLLISION(var);
            if (coll != nullptr)
                coll->ShowShape(EnableMeshBox);
        }

        ImGui::EndMainMenuBar();
    }
        
    if (showMainEditor)
    {
        if (!ImGui::Begin(" Scene GameObject List", &showMainEditor, ImGuiWindowFlags_AlwaysHorizontalScrollbar | ImGuiWindowFlags_ShowBorders | ImGuiWindowFlags_NoCollapse | ImGuiWindowFlags_NoMove | ImGuiWindowFlags_NoResize))
        {
            ImGui::End();
            return;
        }

        if (ImGui::IsRootWindowOrAnyChildFocused())
            isFocusedWindow = true;

        static int WindowHeight;
        
        WindowHeight = RenderMgr->GetHeight() - 18;

        ImGui::SetWindowSize(ImVec2(450, WindowHeight));
        ImVec2 Size = ImGui::GetWindowSize();
        ImGui::SetWindowPos(ImVec2(RenderMgr->GetWidth() - Size.x, 19));
        
        ImGui::Text("%.3f ms/frame (%.1f FPS)", 1000.0f / ImGui::GetIO().Framerate, ImGui::GetIO().Framerate);

        std::list<GameObject*> ObjectList = CUR_SCENE->GetObjectList();
        auto StaticObjectList = CUR_SCENE->GetStaticObjectList();
        ObjectList.sort();
        StaticObjectList.sort();
        ObjectList.merge(StaticObjectList);

        if (ImGui::CollapsingHeader("[Scene Entities]", ImGuiTreeNodeFlags_DefaultOpen))
        {
            static bool FindOption;
            ImGui::Checkbox("Enable Find Option", &FindOption);

            ImGui::Separator();

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

                std::string ObjectName = (*iter)->GetName();
                
                if (ObjectName == currentObjectName)
                    ObjectName += " - Select";

                if (ImGui::TreeNode(ObjectName.c_str()))
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

                        auto PipeLine = (int)(*iter)->GetPipeLine();

                        char* PipeLines[] = {
                            "DIRECT_DEFAULT", "DEFAULT_SHADER", "CUSTOM_SHADER"
                        };

                        ImGui::Combo("PipeLine", &PipeLine, PipeLines, 3);
                        
                        (*iter)->SetPipeLine((GameObject::PIPE_LINE)PipeLine);
                        (*iter)->SetActive(IsActive);
                        (*iter)->SetStatic(IsStatic);
                        (*iter)->SetPriority(Priority);

                        static char TagStr[64] = "";
                        ImGui::InputText("Tag", TagStr, 64);
                        if (ImGui::Button("Change Tag"))
                            (*iter)->SetTag(TagStr);

                        static char buf[64] = "";
                        ImGui::InputText("Name", buf, 64);
                        if (ImGui::Button("Change Name"))
                        {
                            auto object = CUR_SCENE->FindObjectByName((*iter)->GetName().c_str());
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

                    if (ImGui::TreeNode("Edit Object"))
                    {
                        ShowEditObjectTree((*iter)->GetName());

                        ImGui::TreePop();
                    }

                    if (ImGui::SmallButton("Remove Object"))
                        CUR_SCENE->Destroy((*iter));

                    if (ImGui::SmallButton("Focus On"))
                    {
                        auto Object = CUR_SCENE->FindObjectByName((*iter)->GetName().c_str());

                        if (Object != nullptr)
                        {
                            auto Position = ((Transform3D*)Object->transform3D)->GetPosition();

                            ((Camera*)(CUR_SCENE->GetCurrentCamera()->GetComponent("Camera")))->SetTargetPosition(Position);

                            ((Transform3D*)CUR_SCENE->GetCurrentCamera()->transform3D)->SetPosition(
                                Position + Vec3(20, 20, 20)
                            );
                        }
                    }

                    ImGui::TreePop();
                }
            }

            ImGui::Separator();

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
                        CUR_SCENE->AddObject(Object, ObjectName);
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

                        CUR_SCENE->AddObject(Object, ObjectName);
                    }
                }

                for (int i = 0; i < Size; i++)
                    delete ComboBoxItems[i];
                delete[] ComboBoxItems;

                ImGui::TreePop();
            }
        }

        if (ImGui::CollapsingHeader("[Graph Menu]", ImGuiTreeNodeFlags_DefaultOpen))
        {
            auto GraphNodes = CUR_SCENE->GetGraphNodes();
            ImGui::Text("[New Node]");
            static char Name[20] = "";
            ImGui::InputText("Name", Name, 20);

            if (ImGui::Button("Create"))
            {
                GraphNode* Node = new GraphNode();
                Node->Init();
                Node->Reference();
                Node->LoadContents();
                Node->SetName(Name);

                CUR_SCENE->AddGraphNode(Node);
            }

            ImGui::Separator();
            ImGui::Text("[GraphNodes]");

            for each(auto var in *GraphNodes)
            {
                ShowGraphNodeInfo(var.second);
            }
        }

        if (ImGui::CollapsingHeader("[Light Menu]", ImGuiTreeNodeFlags_DefaultOpen))
        {
            auto Lights = CUR_SCENE->GetPointLights();

            static int selected = 0;

            ImGui::BeginChild("Lights", ImVec2(150, 250), true);

            int i = 0;
            for each(auto var in Lights)
            {
                char label[128];
                sprintf(label, "Light %d", i);
                if (ImGui::Selectable(label, selected == i))
                    selected = i;

                bool Active = false;

                ImGui::SameLine();

                if (var->IsOn())
                    ImGui::Text("- Active");
                else
                    ImGui::Text("- Passive");

                i++;
            }

            ImGui::EndChild();
            ImGui::SameLine();

            ImGui::BeginGroup();

            ImGui::BeginChild("item view", ImVec2(250, 250), false); // Leave room for 1 line below us
            ImGui::Text("Light : %d", selected);
            ImGui::Separator();

            auto SelectedObject = Lights.at(selected);

            bool Active = SelectedObject->IsOn();

            ImGui::Checkbox("Active", &Active);
            SelectedObject->SetLight(Active);

            ImGui::Separator();
            ImGui::Text("Light Option", selected);
            float Ambient[4] = {
                SelectedObject->GetAmbient().r,
                SelectedObject->GetAmbient().g,
                SelectedObject->GetAmbient().b,
                SelectedObject->GetAmbient().a
            };
            float Diffuse[4] = {
                SelectedObject->GetDiffuse().r,
                SelectedObject->GetDiffuse().g,
                SelectedObject->GetDiffuse().b,
                SelectedObject->GetDiffuse().a
            };
            float Specular[4] = {
                SelectedObject->GetSpecular().r,
                SelectedObject->GetSpecular().g,
                SelectedObject->GetSpecular().b,
                SelectedObject->GetSpecular().a
            };
            float Position[3] = {
                SelectedObject->GetPosition().x,
                SelectedObject->GetPosition().y,
                SelectedObject->GetPosition().z
            };

            auto Radius = SelectedObject->GetRadius();

            ImGui::DragFloat4("Ambient", Ambient, 0.025f);
            ImGui::DragFloat4("Diffuse", Diffuse, 0.025f);
            ImGui::DragFloat4("Specular", Specular, 0.025f);
            ImGui::DragFloat3("Position", Position, 0.1f);
            ImGui::DragFloat("Radius", &Radius);

            ImGui::Separator();

            ImGui::Text("Shadow Option", selected);
            
            float AimPosition[3] = {
                SelectedObject->GetShadowAimPos().x,
                SelectedObject->GetShadowAimPos().y,
                SelectedObject->GetShadowAimPos().z
            };

            float LightViewSize[2] = {
                SelectedObject->GetShadowViewSize().x,
                SelectedObject->GetShadowViewSize().y
            };

            ImGui::DragFloat3("Target", AimPosition, 0.1f);
            ImGui::DragFloat2("ProjViewSize", LightViewSize, 0.1f);

            SelectedObject->SetShadowAimPos(Vec3(AimPosition[0], AimPosition[1], AimPosition[2]));
            SelectedObject->SetShadowViewSize(Vec2(LightViewSize[0], LightViewSize[1]));

            SelectedObject->SetAmbient(Ambient[0], Ambient[1], Ambient[2], Ambient[3]);
            SelectedObject->SetDiffuse(Diffuse[0], Diffuse[1], Diffuse[2], Diffuse[3]);
            SelectedObject->SetSpecular(Specular[0], Specular[1], Specular[2], Specular[3]);
            SelectedObject->SetPosition(Vec3(Position[0], Position[1], Position[2]));
            SelectedObject->SetRadius(Radius);
            
            ImGui::EndChild();

            ImGui::EndGroup();
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
                    CUR_SCENE->SetSkybox(ListBoxItems[CurItem], TypeName);

                for (int i = 0; i<Size; i++)
                    delete ListBoxItems[i];
                delete[] ListBoxItems;

                ImGui::TreePop();
            }

            if (ImGui::TreeNode("World Ambient"))
            {
                auto OriAmbient = CUR_SCENE->GetAmbientColor();

                static float Ambient[4] = { OriAmbient.r, OriAmbient.g, OriAmbient.b, OriAmbient.a };

                ImGui::InputFloat3("Ambient", Ambient);

                CUR_SCENE->SetAmbientColor(Ambient[0], Ambient[1], Ambient[2], Ambient[3]);

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
                auto CurScene = CUR_SCENE;
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

    if (showAddComponent)
    {
        std::string WindowName = " Add Component : " + currentShowInfoObject;
        ImGui::Begin(WindowName.c_str(), &showAddComponent, ImGuiWindowFlags_NoCollapse | ImGuiWindowFlags_ShowBorders | ImGuiWindowFlags_AlwaysAutoResize);

        if (ImGui::IsRootWindowOrAnyChildFocused())
            isFocusedWindow = true;

        const char* listbox_items[] = { "StaticMesh", "Collision", "RigidBody", "SkinnedMesh", "SoundClip", "Material", "Script", "BillBoard", "SpriteBillBoard", "ParticleSystem", "TrailRenderer" };
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
                char** ComboBoxItems = new char*[Size- 1];

                int i = 0;
                for each(auto item in Meshes)
                {
                    if (item == "Preview")
                        continue;
                    ComboBoxItems[i] = new char[64];
                    strcpy(ComboBoxItems[i], item.c_str());
                    i++;
                }

                static int CurItem = 0;

                ImGui::ListBox("Meshes", &CurItem, ComboBoxItems, Size - 1);

                std::string CurItemName = ComboBoxItems[CurItem];
                CurItemName.pop_back();
                CurItemName.pop_back();
                CurItemName += ".jpg";

                ImVec2 tex_screen_pos = ImGui::GetCursorScreenPos();
                float tex_w = (float)128;
                float tex_h = (float)128;

                ImGui::Image(ResourceMgr->LoadTexture(CurItemName), ImVec2(tex_w, tex_h), ImVec2(0, 0), ImVec2(1, 1), ImColor(255, 255, 255, 255), ImColor(255, 255, 255, 128));

                if (ImGui::IsItemHovered())
                {
                    ImGui::BeginTooltip();
                    float focus_sz = 32.0f;
                    float focus_x = ImGui::GetMousePos().x - tex_screen_pos.x - focus_sz * 0.5f; if (focus_x < 0.0f) focus_x = 0.0f; else if (focus_x > tex_w - focus_sz) focus_x = tex_w - focus_sz;
                    float focus_y = ImGui::GetMousePos().y - tex_screen_pos.y - focus_sz * 0.5f; if (focus_y < 0.0f) focus_y = 0.0f; else if (focus_y > tex_h - focus_sz) focus_y = tex_h - focus_sz;
                    ImGui::Text("Min: (%.2f, %.2f)", focus_x, focus_y);
                    ImGui::Text("Max: (%.2f, %.2f)", focus_x + focus_sz, focus_y + focus_sz);
                    ImVec2 uv0 = ImVec2((focus_x) / tex_w, (focus_y) / tex_h);
                    ImVec2 uv1 = ImVec2((focus_x + focus_sz) / tex_w, (focus_y + focus_sz) / tex_h);
                    ImGui::Image(ResourceMgr->LoadTexture(CurItemName), ImVec2(128, 128), uv0, uv1, ImColor(255, 255, 255, 255), ImColor(255, 255, 255, 128));
                    ImGui::EndTooltip();
                }

                if (ImGui::Button("Add Component"))
                {
                    std::string fullpath = "";
                    if (!ResourceMgr->ExistFile(ComboBoxItems[CurItem], &fullpath))
                        break;

                    StaticMesh* Mesh = new StaticMesh();
                    Mesh->SetFile(ComboBoxItems[CurItem]);
                    Mesh->LoadContent();
                    auto Object = CUR_SCENE->FindObjectByName(currentShowInfoObject);
                    Object->AddComponent(Mesh);

                    showAddComponent = false;
                }

                for (int j = 0; j < Size - 1; j++)
                    delete ComboBoxItems[j];
                delete[] ComboBoxItems;
            }
            break;

            // Collision
            case 1:
            {
                char* ComboBoxItems[] = { "Box", "Sphere", "Cone", "Cylinder", "Capsule" };

                static int CurItem = 0;
                ImGui::Combo("Collisions", &CurItem, ComboBoxItems, 5);

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
                    auto Object = CUR_SCENE->FindObjectByName(currentShowInfoObject);

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
                        Coll->CreateCylinder(Radius, Height);
                    else if (CurItem == 4)
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
                static bool LockRoataion[3] = { false };

                ImGui::Checkbox("Enable Gravity", &EnableGravity);
                ImGui::Checkbox("Allow To Sleep", &IsAllowedToSleep);
                ImGui::DragFloat("Bounciness", &Bounciness, 1.0f, 0.1f);
                ImGui::DragFloat("FrictionCoefficient", &FrictionCoefficient, 1.0f, 0.1f);
                ImGui::DragFloat("Mass", &Mass, 1.0f, 0.1f);
                ImGui::Checkbox("Lock RotationX", &LockRoataion[0]);
                ImGui::Checkbox("Lock RotationY", &LockRoataion[1]);
                ImGui::Checkbox("Lock RotationZ", &LockRoataion[2]);

                if (ImGui::Button("Add Component"))
                {
                    auto Object = CUR_SCENE->FindObjectByName(currentShowInfoObject);

                    RigidBody* rigidBody = new RigidBody();

                    if (rigidBody->SetInfo(Object, Mass))
                    {
                        rigidBody->EnableGravity(EnableGravity);
                        rigidBody->SetBounciness(Bounciness);
                        rigidBody->SetFrictionCoefficient(FrictionCoefficient);
                        rigidBody->SetIsAllowedToSleep(IsAllowedToSleep);
                        rigidBody->SetType(reactphysics3d::BodyType(CurItem));
                        rigidBody->LockRotation(LockRoataion[0], LockRoataion[1], LockRoataion[2]);
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
                auto Meshes = ResourceMgr->ExistFiles(".\\Resource\\SkinnedMesh\\*");

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

                    SkinnedMesh* Mesh = new SkinnedMesh();
                    Mesh->SetFile(ComboBoxItems[CurItem]);
                    Mesh->LoadContent();
                    auto Object = CUR_SCENE->FindObjectByName(currentShowInfoObject);
                    Object->AddComponent(Mesh);

                    showAddComponent = false;
                }

                for (int i = 0; i < Size; i++)
                    delete ComboBoxItems[i];
                delete[] ComboBoxItems;
            }
            break;

            // SoundClip
            case 4:
            {
                if (ImGui::Button("Add Component"))
                {
                    SoundClip* soundClip = new SoundClip();
                    auto Object = CUR_SCENE->FindObjectByName(currentShowInfoObject);
                    soundClip->AttachObject(Object);
                    Object->AddComponent(soundClip);

                    showAddComponent = false;
                }
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

                ImGui::DragFloat3("Ambient", Ambient, 0.1f);
                ImGui::DragFloat3("Diffuse", Diffuse, 0.1f);
                ImGui::DragFloat3("Emissive", Emissive, 0.1f);
                ImGui::DragFloat3("Specular", Specular, 0.1f);
                ImGui::DragFloat("Shininess", &Shininess, 0.1f);

                if (ImGui::Button("Add Component"))
                {
                    Material* Mat = new Material();

                    Mat->SetAmbient(Ambient[0], Ambient[1], Ambient[2], Ambient[3]);
                    Mat->SetDiffuse(Diffuse[0], Diffuse[1], Diffuse[2], Diffuse[3]);
                    Mat->SetEmissive(Emissive[0], Emissive[1], Emissive[2], Emissive[3]);
                    Mat->SetSpecular(Specular[0], Specular[1], Specular[2], Specular[3]);
                    Mat->SetShininess(Shininess);

                    Mat->LoadContent();

                    auto Object = CUR_SCENE->FindObjectByName(currentShowInfoObject);
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
                    auto Object = CUR_SCENE->FindObjectByName(currentShowInfoObject);

                    if (AddScript(Object, ComboBoxItems[CurItem]))
                        showAddComponent = false;
                    else
                        showAddComponent = true;
                }

                for (int i = 0; i < Size; i++)
                    delete ComboBoxItems[i];
                delete[] ComboBoxItems;
            }
            break;
            
            // BillBoard
            case 7:
            {

            }
            break;

            // SpriteBillBoard
            case 8:
            {
                auto Sprites = ResourceMgr->ExistFiles(".\\Resource\\Sprites\\*");

                const int Size = Sprites.size();
                char** ComboBoxItems = new char*[Size];

                int i = 0;
                for each(auto item in Sprites)
                {
                    ComboBoxItems[i] = new char[64];
                    strcpy(ComboBoxItems[i], item.c_str());
                    i++;
                }

                static int CurItem = 0;
                ImGui::Combo("Sprites", &CurItem, ComboBoxItems, Size);

                static float Width = 0;
                static float Height = 0;
                static int Cut = 0;
                static int Scene = 0;
                static float Speed = 1;
                static float Alpha = 0;
                static bool Full = 0;
                static bool isTargetCamera = true;

                ImGui::InputFloat("Width", &Width);
                ImGui::InputFloat("Height", &Height);
                ImGui::DragFloat("Speed", &Speed);
                ImGui::InputInt("Cut", &Cut);
                ImGui::InputInt("Scene", &Scene);
                ImGui::InputFloat("Alpha", &Alpha);
                ImGui::Checkbox("FullAnimation", &Full);
                ImGui::Checkbox("IsTargetCamera", &isTargetCamera);

                if (ImGui::Button("Add Component"))
                {
                    SpriteBillBoard* sb = new SpriteBillBoard();
                    sb->SetTexture(ComboBoxItems[CurItem]);
                    sb->SetAnimation(Width, Height, Cut, Scene, Alpha);
                    sb->IsFullAnimation(Full);
                    sb->SetTargetCamera(isTargetCamera);
                    sb->SetSpeed(Speed);
                    sb->LoadContent();

                    auto Object = CUR_SCENE->FindObjectByName(currentShowInfoObject);
                    Object->AddComponent(sb);

                    showAddComponent = false;
                }

                for (int i = 0; i < Size; i++)
                    delete ComboBoxItems[i];
                delete[] ComboBoxItems;
            }
            break;

            // ParticleSystem
            case 9:
            {
                auto Sprites = ResourceMgr->ExistFiles(".\\Resource\\Sprites\\*");

                const int SpriteSize = Sprites.size();
                char** ComboBoxItems = new char*[SpriteSize];

                int i = 0;
                for each(auto item in Sprites)
                {
                    ComboBoxItems[i] = new char[64];
                    strcpy(ComboBoxItems[i], item.c_str());
                    i++;
                }

                static int CurItem = 0;
                ImGui::Combo("Sprites", &CurItem, ComboBoxItems, SpriteSize);
                
                const char* Particle_items[] = { "Firework", "Snow" };
                static int CurParticle = 0;

                ImGui::ListBox("Type", &CurParticle, Particle_items, IM_ARRAYSIZE(Particle_items), 2);

                static int NumOfParticle = 10;
                static int Size = 10;

                ImGui::DragInt("NumOfParticle", &NumOfParticle);
                ImGui::DragInt("Size", &Size);

                if (ImGui::Button("Add Component"))
                {
                    auto Object = CUR_SCENE->FindObjectByName(currentShowInfoObject);
                    
                    if (CurParticle == 0)
                    {
                        FireworkParticle* Particle = new FireworkParticle();
                        Particle->Init(Object, NumOfParticle, Size);
                        Particle->SetTexture(ComboBoxItems[CurItem]);
                        Particle->LoadContent();

                        Object->AddComponent(Particle);
                    }
                    else if (CurParticle == 0)
                    {
                        //SnowParticle* Particle = new SnowParticle();
                        //Particle->Init(Object, boud);
                        //Particle->LoadContent();

                        //Object->AddComponent(Particle);
                    }

                    showAddComponent = false;
                }

                for (int i = 0; i < SpriteSize; i++)
                    delete ComboBoxItems[i];
                delete[] ComboBoxItems;
            }
            break;

            // TrailRenderer
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
    currentObjectName = name;
}