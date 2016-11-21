#include "Common.h"
#include "RenderManager.h"
#include "InputManager.h"
#include "Camera.h"
#include "Transform3D.h"
#include "SceneManager.h"
#include "LogManager.h"

namespace BONE_GRAPHICS
{
	Camera::Camera(int _ID, PROJECTION_TYPE _type, Vector3 _cameraUp, int _width, int _height, float _far, float _near, float _fov)
	{
		CThreadSync sync;

		Transform3D Transform;
		
		ID = _ID;

		width = _width;
		height = _height;
		cameraUp = _cameraUp;
		farDistance = _far;
		nearDistance = _near;
		fov = _fov;

		type = _type;

		SetTypeName("Camera");
	}

	Vector3 Camera::GetTargetPosition()
	{
		CThreadSync sync;

		return targetPosition;
	}

	void Camera::SetTargetPosition(Vector3 _targetPosition)
	{
		CThreadSync sync;

		targetPosition = _targetPosition;
	}
	
	void Camera::SetTargetPosition(float x, float y, float z)
	{
		CThreadSync sync;

		targetPosition.x = x;
		targetPosition.y = y;
		targetPosition.z = z;
	}

	Matrix Camera::GetViewMatrix(GameObject* _owner)
	{
		CThreadSync sync;

		Transform3D* Tr = ((Transform3D*)_owner->GetComponent("Transform3D"));

		if (Tr != NULL)
		{
			Vector3 Position = Tr->GetPosition();

			D3DXMatrixLookAtLH(&viewMatrix, &Position, &targetPosition, &cameraUp);
		}
		else
		{
			LogMgr->ShowMessage(LOG_ERROR, "Camera %d - Don't Exist Transform3D Component.", ID);
			D3DXMatrixIdentity(&viewMatrix);
		}

		return viewMatrix;
	}

	Matrix Camera::GetProjectionMatrix()
	{
		CThreadSync sync;

		if (type == PRJOJECTION_PERSPACTIVE)
			D3DXMatrixPerspectiveFovLH(&projectionMatrix, fov, width / height, nearDistance, farDistance);
		else if (type == PROJECTION_ORTHOGONAL)
			D3DXMatrixOrthoLH(&projectionMatrix, width, height, nearDistance, farDistance);

		return projectionMatrix;
	}

	void Camera::FixedUpdate(GameObject* _owner)
	{
		CThreadSync sync;

		if (SceneMgr->CurrentScene()->GetCameraIndex() == ID)
		{
			Transform3D* Tr = ((Transform3D*)_owner->GetComponent("Transform3D"));
			
			Vector3 Position(0, 0, 0);

			if (Tr != NULL)
				Position = Tr->GetWorldPositon();
			else
				LogMgr->ShowMessage(LOG_ERROR, "Camera %d - Don't Exist Transform3D Component.", ID);

			D3DXMatrixLookAtLH(&viewMatrix, &Position, &targetPosition, &cameraUp);
			RenderMgr->GetDevice()->SetTransform(D3DTS_VIEW, &viewMatrix);

			if (type == PRJOJECTION_PERSPACTIVE)
				D3DXMatrixPerspectiveFovLH(&projectionMatrix, fov, width / height, nearDistance, farDistance);
			else if (type == PROJECTION_ORTHOGONAL)
				D3DXMatrixOrthoLH(&projectionMatrix, width, height, nearDistance, farDistance);

			RenderMgr->GetDevice()->SetTransform(D3DTS_PROJECTION, &projectionMatrix);
		}
	}

	void Camera::SetFov(float _fov)
	{
		CThreadSync sync;

		fov = _fov;
	}
	
	float Camera::GetFov()
	{
		CThreadSync sync;

		return fov;
	}

	void Camera::SetNearDistance(float _near)
	{
		CThreadSync sync;

		nearDistance = _near;
	}
	
	float Camera::GetNearDistance()
	{
		CThreadSync sync;

		return nearDistance;
	}

	void Camera::SetFarDistance(float _far)
	{
		CThreadSync sync;

		farDistance = _far;
	}
	
	float Camera::GetFarDistance()
	{
		CThreadSync sync;

		return farDistance;
	}

	Vector3 Camera::GetCameraUp()
	{
		CThreadSync sync;

		return cameraUp;
	}

	int Camera::GetID()
	{
		CThreadSync sync;

		return ID;
	}
}