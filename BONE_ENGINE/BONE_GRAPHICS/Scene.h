#pragma once
#include "Common.h"
#include "Skybox.h"
#include "Camera.h"
#include "etuImage.h"
#include "GameObject.h"
#include "Skybox.h"
#include "PointLight.h"

namespace BONE_GRAPHICS
{
	class Scene
	{
	private:
		std::list<GameObject*> objectList;
		std::list<GameObject*> staticObjectList;
        std::list<PointLight*> pointLightList;

        etuImage image_LoadingBackground;
		int cameraIndex;
        bool IsFrameworkFlag;
        bool CompleateLoading;
        Skybox skybox;
        
        RGBA globalAmbient;
        std::string name;
         
	public:
		bool InitializeMembers();
		void Reference();
		bool LoadContents();
		void Render();
		void Update();
		void LateUpdate();
		void LateRender();

	public:
		Scene();
		~Scene();
		
		void AddObject(GameObject* object, std::string name);
		void AddObjects(GameObject** objects, int size);

		void SortPriorityObject();

		std::tuple<GameObject**, int> FindObjectsByTag(std::string tag);
		GameObject* FindObjectByTag(std::string tag);
        GameObject* FindObjectByName(std::string name);

		void Destroy(GameObject* gameObject);

		GameObject* GetCurrentCamera();

		void SetCamera(int ID);
		bool SetLoading(std::string imageAddress, int width, int height);

		void SetSkybox(std::string dirName, std::string fileType);

		bool GetSceneFlag();
		void SetSceneFlag(bool flag);
		bool EndLoading();

        void LoadSceneData();
        void EditSceneMode(bool actvie);

		void SetCameraIndex(int index);
		int GetCameraIndex();
        
        void SetName(std::string name);
        std::string GetName();

        void SetAmbientColor(float r, float g, float b, float a);
        void SetAmbientColor(RGBA color);
        RGBA GetAmbientColor();

        void AddPointLight(PointLight* object);
        void RemovePointLight(PointLight* object);
        std::list<PointLight*>  GetPointLights();
	};
}