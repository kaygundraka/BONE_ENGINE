#pragma once
#include "Common.h"
#include "RenderManager.h"
#include "Component.h"
#include "GameObject.h"
using namespace BONE_SYSTEM;

namespace BONE_GRAPHICS
{
	enum PROJECTION_TYPE { PRJOJECTION_PERSPACTIVE, PROJECTION_ORTHOGONAL };

	class Camera : public Component, public MultiThreadSync<Camera>
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
		Camera(int _ID, PROJECTION_TYPE _type, Vector3 _cameraUp, int _width, int _height, float _far, float _near, float _fov);

		int GetID();

		void SetFov(float _fov);
		float GetFov();

		void SetNearDistance(float _near);
		float GetNearDistance();

		void SetFarDistance(float _far);
		float GetFarDistance();

		Vector3 GetCameraUp();
		void SetTargetPosition(Vector3 _targetPosition);
		void SetTargetPosition(float x, float y, float z);
		Vector3 GetTargetPosition();
		Matrix GetViewMatrix(GameObject* _owner);
		Matrix GetProjectionMatrix();

		virtual void FixedUpdate(GameObject* _owner);
	};
}