#include "Common.h"
#include "LogManager.h"
#include "RenderManager.h"
#include "Scene.h"
#include "SceneManager.h"
#include "InputManager.h"
#include "etuImage.h"
#include <thread>
#include <tuple>
#include "LogManager.h"
#pragma warning (disable:4996)

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

	bool Scene::SetLoading(string _imageAddress, int _width, int _height)
	{
		RECT rect = { 0, 0, _width, _height };
		return image_LoadingBackground.SetInformaition("image_LoadingBackground", _imageAddress, D3DXVECTOR3(0, 0, 0), RenderMgr->GetWidth(), RenderMgr->GetHeight(), &rect, NULL);
	}

	bool Scene::InitializeMembers()
	{
		for (auto Iter = objectList.begin(); Iter != objectList.end(); Iter++)
		{
			(*Iter)->Init();
		}

		IsFrameworkFlag = true;

		return IsFrameworkFlag;
	}

	bool Scene::LoadContents()
	{
		for (auto Iter = objectList.begin(); Iter != objectList.end(); Iter++)
		{
			(*Iter)->LoadContents();
		}

		IsFrameworkFlag = true;
		CompleateLoading = true;

		return IsFrameworkFlag;
	}

	void Scene::Render(double _timeDelta)
	{
		for (auto Iter = staticObjectList.begin(); Iter != staticObjectList.end(); Iter++)
		{
			if ((*Iter)->GetActive())
				(*Iter)->Render(_timeDelta);
		}

		for (auto Iter = objectList.begin(); Iter != objectList.end(); Iter++)
		{
			if ((*Iter)->GetActive())
				(*Iter)->Render(_timeDelta);
		}
	}

	void Scene::LateRender()
	{
		for (auto Iter = staticObjectList.begin(); Iter != staticObjectList.end(); Iter++)
		{
			if ((*Iter)->GetActive())
				(*Iter)->LateRender();
		}

		for (auto Iter = objectList.begin(); Iter != objectList.end(); Iter++)
		{
			if ((*Iter)->GetActive())
				(*Iter)->LateRender();
		}
	}

	void Scene::SetSkybox(string _dirName, string _fileType)
	{
		char Path[MAX_PATH];
		strcpy(Path, _dirName.c_str());
		skybox.SetSkybox(Path, _fileType);
	}

	GameObject* Scene::GetCurrentCamera()
	{
		for (auto Iter = objectList.begin(); Iter != objectList.end(); Iter++)
		{
			if (((Camera*)(*Iter)->GetComponent("Camera")) != NULL)
			{
				if (((Camera*)(*Iter)->GetComponent("Camera"))->GetID() == SceneMgr->CurrentScene()->GetCameraIndex())
					return (*Iter);
			}
		}
	}

	void Scene::Reference()
	{
		for (auto Iter = objectList.begin(); Iter != objectList.end(); Iter++)
		{
			(*Iter)->Reference();
		}
	}
	
	void Scene::Update(double _timeDelta)
	{
		skybox.Render(GetCurrentCamera());

		for (auto Iter = objectList.begin(); Iter != objectList.end(); Iter++)
		{
			if ((*Iter)->GetActive())
				(*Iter)->Update(_timeDelta);
		}
	}

	void Scene::LateUpdate(double _timeDelta)
	{
		for (auto Iter = objectList.begin(); Iter != objectList.end(); Iter++)
		{
			if ((*Iter)->GetActive())
				(*Iter)->LateUpdate(_timeDelta);
		}
	}

	void Scene::AddObject(GameObject* _object)
	{
		if (_object->GetStatic())
			staticObjectList.push_back(_object);
		else
			objectList.push_back(_object);

		if (CompleateLoading)
		{
			_object->Init();
			_object->Reference();
			_object->LoadContents();
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

	void Scene::AddObjects(GameObject** _objects, int _size)
	{
		for (int i = 0; i < _size; i++)
		{
			if (_objects[i]->GetStatic())
				staticObjectList.push_back(_objects[i]);
			else
				objectList.push_back(_objects[i]);

			if (CompleateLoading)
			{
				_objects[i]->Init();
				_objects[i]->Reference();
				_objects[i]->LoadContents();
			}
		}
	}

	bool Scene::EndLoading()
	{
		return CompleateLoading;
	}

	GameObject* Scene::FindObjectByTag(string _tag)
	{
		for (auto Iter = objectList.begin(); Iter != objectList.end(); Iter++)
			if (_tag == (*Iter)->Tag())
				return (*Iter);

		for (auto Iter = staticObjectList.begin(); Iter != staticObjectList.end(); Iter++)
			if (_tag == (*Iter)->Tag())
				return (*Iter);

		return NULL;
	}

	std::tuple<GameObject**, int> Scene::FindObjectsByTag(string _tag)
	{
		std::tuple<GameObject**, int> Result;
		
		GameObject** ObjArray = new GameObject*[objectList.size() + staticObjectList.size()];
		int Size = 0;

		for (int i = 0; i < objectList.size() + staticObjectList.size(); i++)
			ObjArray[i] = NULL;

		for (auto Iter = objectList.begin(); Iter != objectList.end(); Iter++)
		{
			if (_tag == (*Iter)->Tag())
			{
				ObjArray[Size++] = (*Iter);
			}
		}

		for (auto Iter = staticObjectList.begin(); Iter != staticObjectList.end(); Iter++)
		{
			if (_tag == (*Iter)->Tag())
			{
				ObjArray[Size++] = (*Iter);
			}
		}

		std::get<0>(Result) = ObjArray;
		std::get<1>(Result) = Size;

		return Result;
	}

	bool Scene::GetSceneFlag()
	{
		return IsFrameworkFlag;
	}

	void Scene::Destroy(GameObject* _gameObject)
	{
		for (auto Iter = objectList.begin(); Iter != objectList.end(); Iter++)
		{
			if (_gameObject == (*Iter))
			{
				objectList.remove(_gameObject);
				break;
			}
		}

		for (auto Iter = staticObjectList.begin(); Iter != staticObjectList.end(); Iter++)
		{
			if (_gameObject == (*Iter))
			{
				objectList.remove(_gameObject);
				break;
			}
		}

		delete _gameObject;
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