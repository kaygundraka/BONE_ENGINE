#include "Common.h"
#include "Scene.h"
#include "LogManager.h"
#include "RenderManager.h"
#include "SceneManager.h"
#include "InputManager.h"
#include "LogManager.h"
#include "ResourceManager.h"
#include "Transform3D.h"
#include "etuImage.h"

namespace BONE_GRAPHICS
{
    Scene::Scene()
    {
        IsFrameworkFlag = false;
        cameraIndex = 0;
        CompleateLoading = false;
        onLoadScene = false;

        globalAmbient.r = 1.0f;
        globalAmbient.g = 1.0f;
        globalAmbient.b = 1.0f;
        globalAmbient.a = 1.0f;

        enableFog = false;
        fogStart = 50.0f;
        fogEnd = 100.0f;
        fogDensity = 1.0f;
        fogMode = D3DFOG_LINEAR;

        fogColor = COLOR::YELLOW;
    }

    Scene::~Scene()
    {
        for (auto Iter = objectList.begin(); Iter != objectList.end();)
        {
            GameObject* Temp = *Iter;

            if (Temp == NULL)
            {
                Iter = objectList.erase(Iter);
                delete Temp;
            }
            else
                Iter++;
        }
    }

    bool Scene::SetLoading(std::string imageAddress, int width, int height)
    {
        RECT rect = { 0, 0, width, height };
        return image_LoadingBackground.SetInformaition("image_LoadingBackground", imageAddress, D3DXVECTOR3(0, 0, 0), RenderMgr->GetWidth(), RenderMgr->GetHeight(), &rect, NULL);
    }

    bool Scene::InitializeMembers()
    {
        for (auto Iter = objectList.begin(); Iter != objectList.end(); Iter++)
            (*Iter)->Init();

        IsFrameworkFlag = true;

        return IsFrameworkFlag;
    }

    bool Scene::LoadContents()
    {
        float Max = objectList.size();
        float Cur = 0;
        loadPerTime = 0;
        
        for (auto Iter = objectList.begin(); Iter != objectList.end(); Iter++)
        {
            Cur++;
            (*Iter)->LoadContents();

            loadPerTime = Cur / Max;
        }

        IsFrameworkFlag = true;
        CompleateLoading = true;

        return IsFrameworkFlag;
    }

    float Scene::GetLoadPerTime()
    {
        return loadPerTime * 100;
    }

    void Scene::Render()
    {
        for (auto Iter = staticObjectList.begin(); Iter != staticObjectList.end(); Iter++)
            if ((*Iter)->GetActive())
                (*Iter)->Render();

        for (auto Iter = objectList.begin(); Iter != objectList.end(); Iter++)
            if ((*Iter)->GetActive())
                (*Iter)->Render();
    }

    void Scene::LateRender()
    {
        for (auto Iter = staticObjectList.begin(); Iter != staticObjectList.end(); Iter++)
            if ((*Iter)->GetActive())
                (*Iter)->LateRender();

        for (auto Iter = objectList.begin(); Iter != objectList.end(); Iter++)
            if ((*Iter)->GetActive())
                (*Iter)->LateRender();
    }

    void Scene::SetSkybox(std::string dirName, std::string fileType)
    {
        char Path[MAX_PATH];
        strcpy_s(Path, dirName.c_str());
        skybox.SetSkybox(Path, fileType);
    }

    GameObject* Scene::GetCurrentCamera()
    {
        for (auto Iter = objectList.begin(); Iter != objectList.end(); Iter++)
            if (((Camera*)(*Iter)->GetComponent("Camera")) != NULL)
                if (((Camera*)(*Iter)->GetComponent("Camera"))->GetID() == SceneMgr->CurrentScene()->GetCameraIndex())
                    return (*Iter);
    }

    void Scene::Reference()
    {
        for (auto Iter = objectList.begin(); Iter != objectList.end(); Iter++)
            (*Iter)->Reference();
    }

    void Scene::Update()
    {
        skybox.Render(GetCurrentCamera());

        for (auto Iter = objectList.begin(); Iter != objectList.end(); Iter++)
            if ((*Iter)->GetActive())
                (*Iter)->Update();
    }

    void Scene::LateUpdate()
    {
        for (auto Iter = objectList.begin(); Iter != objectList.end(); Iter++)
            if ((*Iter)->GetActive())
                (*Iter)->LateUpdate();
    }

    void Scene::AddObject(GameObject* object, std::string name)
    {
        object->SetName(name);

        if (object->GetStatic())
            staticObjectList.push_back(object);
        else
            objectList.push_back(object);

        if (CompleateLoading)
        {
            object->Init();
            object->Reference();
            object->LoadContents();
        }
    }

    class ObjSortClass {
    public:
        bool operator() (GameObject* left, GameObject* right) const {
            return left->GetPriority() < right->GetPriority();
        }
    };

    void Scene::SortPriorityObject()
    {
        objectList.sort(ObjSortClass());
        staticObjectList.sort(ObjSortClass());
    }

    void Scene::AddObjects(GameObject** objects, int size)
    {
        for (int i = 0; i < size; i++)
        {
            if (objects[i]->GetStatic())
                staticObjectList.push_back(objects[i]);
            else
                objectList.push_back(objects[i]);

            if (CompleateLoading)
            {
                objects[i]->Init();
                objects[i]->Reference();
                objects[i]->LoadContents();
            }
        }
    }

    bool Scene::EndLoading()
    {
        return CompleateLoading;
    }

    GameObject* Scene::FindObjectByTag(std::string tag)
    {
        for (auto Iter = objectList.begin(); Iter != objectList.end(); Iter++)
            if (tag == (*Iter)->Tag())
                return (*Iter);

        for (auto Iter = staticObjectList.begin(); Iter != staticObjectList.end(); Iter++)
            if (tag == (*Iter)->Tag())
                return (*Iter);

        return nullptr;
    }

    GameObject* Scene::FindObjectByName(std::string name)
    {
        for (auto Iter = objectList.begin(); Iter != objectList.end(); Iter++)
            if (name == (*Iter)->GetName())
                return (*Iter);

        for (auto Iter = staticObjectList.begin(); Iter != staticObjectList.end(); Iter++)
            if (name == (*Iter)->GetName())
                return (*Iter);

        return nullptr;
    }

    std::tuple<GameObject**, int> Scene::FindObjectsByTag(std::string tag)
    {
        std::tuple<GameObject**, int> Result;

        GameObject** ObjArray = new GameObject*[objectList.size() + staticObjectList.size()];
        int Size = 0;

        for (int i = 0; i < objectList.size() + staticObjectList.size(); i++)
            ObjArray[i] = NULL;

        for (auto Iter = objectList.begin(); Iter != objectList.end(); Iter++)
            if (tag == (*Iter)->Tag())
                ObjArray[Size++] = (*Iter);

        for (auto Iter = staticObjectList.begin(); Iter != staticObjectList.end(); Iter++)
            if (tag == (*Iter)->Tag())
                ObjArray[Size++] = (*Iter);

        std::get<0>(Result) = ObjArray;
        std::get<1>(Result) = Size;

        return Result;
    }

    bool Scene::GetSceneFlag()
    {
        return IsFrameworkFlag;
    }

    void Scene::Destroy(GameObject* gameObject)
    {
        for (auto Iter = objectList.begin(); Iter != objectList.end(); Iter++)
        {
            if (gameObject == (*Iter))
            {
                objectList.remove(gameObject);
                break;
            }
        }

        for (auto Iter = staticObjectList.begin(); Iter != staticObjectList.end(); Iter++)
        {
            if (gameObject == (*Iter))
            {
                objectList.remove(gameObject);
                break;
            }
        }

        delete gameObject;
    }

    void Scene::SetCamera(int ID)
    {
    }

    void Scene::SetSceneFlag(bool _flag)
    {
        IsFrameworkFlag = _flag;
    }

    void Scene::SetCameraIndex(int _index)
    {
        cameraIndex = _index;
    }

    int Scene::GetCameraIndex()
    {
        return cameraIndex;
    }

    void Scene::SetName(std::string name)
    {
        this->name = name;
    }

    std::string Scene::GetName()
    {
        return name;
    }

    void Scene::SaveSceneData()
    {
    }

    void Scene::ClearSceneData()
    {
        std::string FullPath;

        if (!ResourceMgr->ExistFile(name + ".json", &FullPath))
            return;

        _unlink(FullPath.c_str());
    }

    void Scene::LoadSceneData()
    {
        if (!onLoadScene)
            return;

        std::string FullPath;

        if (!ResourceMgr->ExistFile(name + ".json", &FullPath))
            return;

        std::ifstream file(FullPath);
        json j;
        file >> j;

        for (json::iterator it = j.begin(); it != j.end(); ++it) {
            auto Object = this->FindObjectByName(it.key());

            if (Object == nullptr)
            {
                if (it.key() == "PointLight")
                {
                    PointLight* Object = new PointLight();

                    Object->SetRadius(200);

                    Object->SetName(it.key());

                    Object->SetPriority(1);
                    Object->SetTag("");
                    Object->SetLight(true);

                    Object->SetPrfabName(it->find("PrefabName").value().get<std::string>());
                    Object->LoadPrefab();

                    this->AddObject(Object, it.key());

                    auto Position = it->find("Position").value().get<std::vector<float>>();
                    ((Transform3D*)Object->GetComponent("Transform3D"))->SetPosition(Position[0], Position[1], Position[2]);

                    auto Rotation = it->find("Rotation").value().get<std::vector<float>>();
                    ((Transform3D*)Object->GetComponent("Transform3D"))->SetRotate(Rotation[0], Rotation[1], Rotation[2]);

                    auto Scale = it->find("Scale").value().get<std::vector<float>>();
                    ((Transform3D*)Object->GetComponent("Transform3D"))->SetScale(Scale[0], Scale[1], Scale[2]);
                }
                else
                {
                    if (it->find("IsChild").value().get<bool>())
                        continue;

                    Object = new GameObject();
                    Transform3D* tr = new Transform3D();
                    Object->AddComponent(tr);

                    Object->SetName(it.key());

                    Object->SetPriority(1);
                    Object->SetTag("");

                    Object->SetPrfabName(it->find("PrefabName").value().get<std::string>());
                    Object->LoadPrefab();

                    this->AddObject(Object, it.key());

                    auto Position = it->find("Position").value().get<std::vector<float>>();
                    ((Transform3D*)Object->GetComponent("Transform3D"))->SetPosition(Position[0], Position[1], Position[2]);

                    auto Rotation = it->find("Rotation").value().get<std::vector<float>>();
                    ((Transform3D*)Object->GetComponent("Transform3D"))->SetRotate(Rotation[0], Rotation[1], Rotation[2]);

                    auto Scale = it->find("Scale").value().get<std::vector<float>>();
                    ((Transform3D*)Object->GetComponent("Transform3D"))->SetScale(Scale[0], Scale[1], Scale[2]);
                }
            }
            else
            {
                Object->SetPrfabName(it->find("PrefabName").value().get<std::string>());
                Object->LoadPrefab();

                auto Position = it->find("Position").value().get<std::vector<float>>();
                ((Transform3D*)Object->GetComponent("Transform3D"))->SetPosition(Position[0], Position[1], Position[2]);

                auto Rotation = it->find("Rotation").value().get<std::vector<float>>();
                ((Transform3D*)Object->GetComponent("Transform3D"))->SetRotate(Rotation[0], Rotation[1], Rotation[2]);

                auto Scale = it->find("Scale").value().get<std::vector<float>>();
                ((Transform3D*)Object->GetComponent("Transform3D"))->SetScale(Scale[0], Scale[1], Scale[2]);
            }
        }

        file.close();
    }

    void Scene::SetAmbientColor(float r, float g, float b, float a)
    {
        globalAmbient.r = r;
        globalAmbient.g = g;
        globalAmbient.b = b;
        globalAmbient.a = a;
    }

    void Scene::SetAmbientColor(RGBA color)
    {
        globalAmbient = color;
    }

    RGBA Scene::GetAmbientColor()
    {
        return globalAmbient;
    }

    void Scene::OnLoadSceneData()
    {
        onLoadScene = true;
    }

    void Scene::AddPointLight(PointLight* object)
    {
        pointLightList.push_back(object);
    }

    void Scene::RemovePointLight(PointLight* object)
    {
        for (auto iter = pointLightList.begin(); iter != pointLightList.end();)
        {
            if (*iter == object)
            {
                pointLightList.erase(iter);
                break;
            }
        }
    }

    std::list<PointLight*> Scene::GetPointLights()
    {
        return pointLightList;
    }

    std::list<GameObject*> Scene::GetObjectList()
    {
        return objectList;
    }

    std::list<GameObject*> Scene::GetStaticObjectList()
    {
        return staticObjectList;
    }

    void Scene::SetFogStatus(bool on, D3DXCOLOR color, float fogStart, float fogEnd, float fogDensity, int mode)
    {
        this->enableFog = on;
        this->fogStart = fogStart;
        this->fogEnd = fogEnd;
        this->fogMode = mode;
        this->fogDensity = fogDensity;
        this->fogColor = color;

        RenderMgr->SetupPixelFog(on, color, fogStart, fogEnd, fogDensity, mode);
    }
    
    bool Scene::OnFog()
    {
        return enableFog;
    }

    float Scene::GetFogDensity()
    {
        return fogDensity;
    }
    
    float Scene::GetFogStart()
    {
        return fogStart;
    }

    float Scene::GetFogEnd()
    {
        return fogEnd;
    }
    
    int Scene::GetFogMode()
    {
        return fogMode;
    }

    D3DXCOLOR Scene::GetFogColor()
    {
        return fogColor;
    }
}