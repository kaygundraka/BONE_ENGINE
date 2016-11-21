#pragma once
#include "Common.h"
#include "Skybox.h"
#include "Camera.h"
#include "etuImage.h"
#include "GameObject.h"
#include "Skybox.h"
#include "SmartPointer.h"
#include <list>

namespace BONE_GRAPHICS
{
	class Scene
	{
	private:
		std::list<GameObject*> objectList;
		std::list<GameObject*> staticObjectList;
		etuImage image_LoadingBackground;
		bool IsFrameworkFlag;
		int cameraIndex;
		Skybox skybox;
		bool CompleateLoading;
 
	public:
		bool InitializeMembers();
		void Reference();
		bool LoadContents();
		void Render(double _timeDelta);
		void Update(double _timeDelta);
		void LateUpdate(double _timeDelta);
		void LateRender();

	public:
		Scene();
		~Scene();
		
		void AddObject(GameObject* _object);
		void AddObjects(GameObject** _objects, int _size);
		
		void SortPriorityObject();

		std::tuple<GameObject**, int> FindObjectsByTag(string _tag);
		GameObject* FindObjectByTag(string _tag);
		void Destroy(GameObject* _gameObject);

		GameObject* GetCurrentCamera();

		void SetCamera(int ID);
		bool SetLoading(string _imageAddress, int _width, int _height);

		void SetSkybox(string _dirName, string _fileType);

		bool GetSceneFlag();
		void SetSceneFlag(bool _flag);
		bool EndLoading();

		void SetCameraIndex(int _index);
		int GetCameraIndex();

	};
}