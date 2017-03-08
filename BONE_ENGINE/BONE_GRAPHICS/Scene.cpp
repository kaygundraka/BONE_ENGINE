#include "Common.h"
#include "LogManager.h"
#include "RenderManager.h"
#include "Scene.h"
#include "SceneManager.h"
#include "InputManager.h"
#include "etuImage.h"
#include "LogManager.h"

namespace BONE_GRAPHICS
{
	Scene::Scene()
	{
		IsFrameworkFlag = false;
		cameraIndex = 0;
		CompleateLoading = false;
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

	bool Scene::SetLoading(string imageAddress, int width, int height)
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
		for (auto Iter = objectList.begin(); Iter != objectList.end(); Iter++)
			(*Iter)->LoadContents();
		
		IsFrameworkFlag = true;
		CompleateLoading = true;

		return IsFrameworkFlag;
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

	void Scene::SetSkybox(string dirName, string fileType)
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

	void Scene::AddObject(GameObject* object)
	{
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
	public :
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

	GameObject* Scene::FindObjectByTag(string tag)
	{
		for (auto Iter = objectList.begin(); Iter != objectList.end(); Iter++)
			if (tag == (*Iter)->Tag())
				return (*Iter);

		for (auto Iter = staticObjectList.begin(); Iter != staticObjectList.end(); Iter++)
			if (tag == (*Iter)->Tag())
				return (*Iter);

        return nullptr;
	}

	std::tuple<GameObject**, int> Scene::FindObjectsByTag(string tag)
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
}