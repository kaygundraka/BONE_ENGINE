#include "Common.h"
#include "RenderManager.h"
#include "InputManager.h"
#include "Camera.h"
#include "Transform3D.h"
#include "SceneManager.h"
#include "LogManager.h"

namespace BONE_GRAPHICS
{
	Camera::Camera(int ID, PROJECTION_TYPE type, Vector3 cameraUp, int width, int height, float farDist, float nearDist, float fov)
	{
		Transform3D Transform;
		
		this->ID = ID;
        this->width = width;
        this->height = height;
        this->cameraUp = cameraUp;
        this->farDistance = farDist;
        this->nearDistance = nearDist;
        this->fov = fov;
        this->type = type;

		SetTypeName("Camera");
	}

	Vector3 Camera::GetTargetPosition()
	{
		return targetPosition;
	}

	void Camera::SetTargetPosition(Vector3 targetPosition)
	{
		this->targetPosition = targetPosition;
	}
	
	void Camera::SetTargetPosition(float x, float y, float z)
	{
		targetPosition.x = x;
		targetPosition.y = y;
		targetPosition.z = z;
	}

	Matrix Camera::GetViewMatrix(GameObject* owner)
	{
		Transform3D* Tr = ((Transform3D*)owner->GetComponent("Transform3D"));

		if (Tr != nullptr)
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
		if (type == PRJOJECTION_PERSPACTIVE)
			D3DXMatrixPerspectiveFovLH(&projectionMatrix, fov, width / height, nearDistance, farDistance);
		else if (type == PROJECTION_ORTHOGONAL)
			D3DXMatrixOrthoLH(&projectionMatrix, width, height, nearDistance, farDistance);

		return projectionMatrix;
	}

	void Camera::FixedUpdate(GameObject* owner)
	{
        this->owner = owner;

		if (SceneMgr->CurrentScene()->GetCameraIndex() == ID)
		{
			Transform3D* Tr = ((Transform3D*)owner->GetComponent("Transform3D"));
            
			Vector3 Position(0, 0, 0);

			if (Tr != nullptr)
				Position = Tr->GetWorldPositon();
			else
				LogMgr->ShowMessage(LOG_ERROR, "Camera %d - Don't Exist Transform3D Component.", ID);

            D3DXVec3Normalize(&viewVector, &(targetPosition - Tr->GetWorldPositon()));
            D3DXVec3Cross(&crossVector, &cameraUp, &viewVector);

			D3DXMatrixLookAtLH(&viewMatrix, &Position, &targetPosition, &cameraUp);
			RenderMgr->GetDevice()->SetTransform(D3DTS_VIEW, &viewMatrix);

			if (type == PRJOJECTION_PERSPACTIVE)
				D3DXMatrixPerspectiveFovLH(&projectionMatrix, fov, width / height, nearDistance, farDistance);
			else if (type == PROJECTION_ORTHOGONAL)
				D3DXMatrixOrthoLH(&projectionMatrix, width, height, nearDistance, farDistance);

			RenderMgr->GetDevice()->SetTransform(D3DTS_PROJECTION, &projectionMatrix);
		}
	}

    int Camera::GetScreenWidth()
    {
        return width;
    }
    
    int Camera::GetScreenHeight()
    {
        return height;
    }
    
    PROJECTION_TYPE Camera::GetProjectionType()
    {
        return type;
    }

	void Camera::SetFov(float fov)
	{
		this->fov = fov;
	}
	
	float Camera::GetFov()
	{
		return fov;
	}

	void Camera::SetNearDistance(float nearDist)
	{
		nearDistance = nearDist;
	}
	
	float Camera::GetNearDistance()
	{
		return nearDistance;
	}

	void Camera::SetFarDistance(float farDist)
	{
		farDistance = farDist;
	}
	
	float Camera::GetFarDistance()
	{
		return farDistance;
	}

	Vector3 Camera::GetCameraUp()
	{
		return cameraUp;
	}

	int Camera::GetID()
	{
		return ID;
	}

    void Camera::RotateLocalX(float angle)
    {
        D3DXMATRIXA16 matRot;
        D3DXMatrixRotationAxis(&matRot, &crossVector, angle);

        D3DXVECTOR3 vNewDst, vNewUp;
        D3DXVec3TransformCoord(&vNewDst, &viewVector, &matRot);
      
        Transform3D* Tr = ((Transform3D*)owner->GetComponent("Transform3D"));
        auto EyePos = Tr->GetPosition();
        vNewDst += EyePos;
        targetPosition = vNewDst;

        D3DXVec3Normalize(&viewVector, &(targetPosition - Tr->GetWorldPositon()));
        D3DXVec3Cross(&crossVector, &cameraUp, &viewVector);
    }

    void Camera::RotateLocalY(float angle)
    {
        D3DXMATRIXA16 matRot;
        D3DXMatrixRotationAxis(&matRot, &cameraUp, angle);

        D3DXVECTOR3 vNewDst;
        D3DXVec3TransformCoord(&vNewDst, &viewVector, &matRot);
        Transform3D* Tr = ((Transform3D*)owner->GetComponent("Transform3D"));
        auto EyePos = Tr->GetPosition();
        vNewDst += EyePos;
        targetPosition = vNewDst;

        D3DXVec3Normalize(&viewVector, &(targetPosition - Tr->GetWorldPositon()));
        D3DXVec3Cross(&crossVector, &cameraUp, &viewVector);
    }

    Vector3 Camera::GetViewVector()
    {
        return viewVector;
    }

    Vector3 Camera::GetCrossVector()
    {
        return crossVector;
    }
}