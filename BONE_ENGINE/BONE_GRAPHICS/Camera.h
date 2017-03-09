#pragma once
#include "Common.h"
#include "RenderManager.h"
#include "Component.h"
#include "GameObject.h"
using namespace BONE_SYSTEM;

namespace BONE_GRAPHICS
{
	enum PROJECTION_TYPE { PRJOJECTION_PERSPACTIVE, PROJECTION_ORTHOGONAL };

	class Camera : public Component
	{
	private:
		int ID;

		Vector3 targetPosition;
		Vector3 cameraUp;
		Matrix viewMatrix;
		Matrix projectionMatrix;

		float nearDistance;
		float farDistance;
		float fov;
		
		PROJECTION_TYPE type;

		int width;
		int height;

	public:
		Camera(int ID, PROJECTION_TYPE type, Vector3 cameraUp, int width, int height, float farDist, float nearDist, float fov);
        
        void SetFov(float fov);
		void SetNearDistance(float nearDist);
		void SetFarDistance(float farDist);
		void SetTargetPosition(Vector3 targetPosition);
		void SetTargetPosition(float x, float y, float z);

        int GetID();
        int GetScreenWidth();
        int GetScreenHeight();
        PROJECTION_TYPE GetProjectionType();
        float GetFov();
        float GetNearDistance();
        float GetFarDistance();
        Vector3 GetCameraUp();
        Vector3 GetTargetPosition();
		Matrix GetViewMatrix(GameObject* owner);
		Matrix GetProjectionMatrix();

		virtual void FixedUpdate(GameObject* owner);
	};
}