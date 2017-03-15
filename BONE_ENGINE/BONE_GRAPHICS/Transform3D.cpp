#include "Common.h"
#include "Transform3D.h"

namespace BONE_GRAPHICS
{
	Transform3D::Transform3D()
	{
		position = Vector3(0, 0, 0);
		rotAngle = Quaternion(0, 0, 0, 1);
		scale = Vector3(1, 1, 1);
		forward = Vector3(0, 0, -1);
		up = Vector3(0, 1, 0);
		
		D3DXMatrixIdentity(&transform);
		D3DXMatrixIdentity(&rotateTransform);
		D3DXMatrixIdentity(&translateTransform);
		D3DXMatrixIdentity(&scaleTransform);

		parent = nullptr;

		SetTypeName("Transform3D");
	}

	Transform3D::~Transform3D()
	{
	}

	void Transform3D::SetForward(Vector3 forward)
	{
		this->forward = forward;
		D3DXVec3Normalize(&this->forward, &this->forward);
	}

	void Transform3D::SetForward(float x, float y, float z)
	{
		forward.x = x;
		forward.y = y;
		forward.z = z;

		D3DXVec3Normalize(&forward, &forward);
	}

	Vector3 Transform3D::GetForward()
	{
		Vector3 Forward = forward;

		D3DXVec3TransformNormal(&Forward, &Forward, &transform);
		D3DXVec3Normalize(&Forward, &Forward);

		return Forward;
	}

	Vector3 Transform3D::GetLeft()
	{
		Vector3 Left;
		Vector3 Forward = forward;
		Vector3 Up = up;

		D3DXVec3TransformNormal(&Up, &Up, &transform);
		D3DXVec3Normalize(&Up, &Up);

		D3DXVec3TransformNormal(&Forward, &Forward, &transform);
		D3DXVec3Normalize(&Forward, &Forward);

		D3DXVec3Cross(&Left, &Forward, &Up);
		D3DXVec3Normalize(&Left, &Left);
		
		return Left;
	}

	void Transform3D::SetPosition(Vector3 pos)
	{
		this->position = pos;
	}

	void Transform3D::SetPosition(float x, float y, float z)
	{
		position.x = x;
		position.y = y;
		position.z = z;
	}

	void Transform3D::Translate(Vector3 pos)
	{
		position += pos;
	}

	void Transform3D::Translate(float x, float y, float z)
	{
		position.x += x;
		position.y += y;
		position.z += z;
	}

	Vector3 Transform3D::GetPosition()
	{
		return position;
	}

	Vector3 Transform3D::GetWorldPositon()
	{
		GameObject* parentPtr = parent;

		Vector3 Posit = position;

		while (parentPtr != nullptr)
		{
			Posit += ((Transform3D*)parentPtr->GetComponent("Transform3D"))->GetPosition();

			parentPtr = parentPtr->GetParent();
		}

		D3DXMatrixTranslation(&translateTransform, Posit.x, Posit.y, Posit.z);

		return Posit;
	}

	void Transform3D::Scale(Vector3 scale)
	{
		this->scale.x += scale.x;
        this->scale.y += scale.y;
        this->scale.z += scale.z;
	}

	void Transform3D::Scale(float x, float y, float z)
	{
		scale.x += x;
		scale.y += y;
		scale.z += z;
	}

	void Transform3D::SetScale(Vector3 scale)
	{
        this->scale = scale;
	}

	void Transform3D::SetScale(float x, float y, float z)
	{
		scale.x = x;
		scale.y = y;
		scale.z = z;
	}

	Vector3 Transform3D::GetScale()
	{
		return scale;
	}

	Vector3 Transform3D::GetWolrdScale()
	{
		GameObject* parentPtr = parent;

		Vector3 Scale = scale;

		while (parentPtr != nullptr)
		{
			Scale.x *= ((Transform3D*)parentPtr->GetComponent("Transform3D"))->GetScale().x;
			Scale.y *= ((Transform3D*)parentPtr->GetComponent("Transform3D"))->GetScale().y;
			Scale.z *= ((Transform3D*)parentPtr->GetComponent("Transform3D"))->GetScale().z;

			parentPtr = parentPtr->GetParent();
		}

		D3DXMatrixScaling(&scaleTransform, Scale.x, Scale.y, Scale.z);

		return Scale;
	}

	void Transform3D::Rotate(Quaternion rotAngle)
	{
		D3DXQuaternionMultiply(&this->rotAngle, &this->rotAngle, &rotAngle);
	}

	void Transform3D::Rotate(float x, float y, float z)
	{
		Quaternion Quater;
		D3DXQuaternionRotationYawPitchRoll(&Quater, y, x, z);
		
		D3DXQuaternionMultiply(&rotAngle, &rotAngle, &Quater);
	}

	void Transform3D::SetRotate(Quaternion rotAngle)
	{
		this->rotAngle = rotAngle;
	}

	void Transform3D::SetRotate(float x, float y, float z)
	{
		Quaternion Quater;
		D3DXQuaternionRotationYawPitchRoll(&Quater, y, x, z);

		rotAngle = Quater;
	}

	Quaternion Transform3D::GetRotateAngle()
	{
		return rotAngle;
	}

	Quaternion Transform3D::GetWorldRotateAngle()
	{
		GameObject* parentPtr = parent;

		Quaternion result = rotAngle;

		while (parentPtr != nullptr)
		{
			result *= ((Transform3D*)parentPtr->GetComponent("Transform3D"))->GetRotateAngle();

			parentPtr = parentPtr->GetParent();
		}

		D3DXMatrixRotationQuaternion(&rotateTransform, &result);
		
		return result;
	}

	void Transform3D::AttachObject(GameObject* parent)
	{
		this->parent = parent;
	}

	Matrix Transform3D::GetTransform()
	{
		GameObject* parentPtr = parent;

		Vector3 Scale = scale;
		Vector3 Posit = position;
		Quaternion RotAngle = rotAngle;

		while (parentPtr != nullptr)
		{
			Scale.x *= ((Transform3D*)parentPtr->GetComponent("Transform3D"))->GetScale().x;
			Scale.y *= ((Transform3D*)parentPtr->GetComponent("Transform3D"))->GetScale().y;
			Scale.z *= ((Transform3D*)parentPtr->GetComponent("Transform3D"))->GetScale().z;

			Posit += ((Transform3D*)parentPtr->GetComponent("Transform3D"))->GetPosition();

			RotAngle *= ((Transform3D*)parentPtr->GetComponent("Transform3D"))->GetRotateAngle();

			parentPtr = parentPtr->GetParent();
		}

		Quaternion RotQuaternion;
	
		D3DXMatrixRotationQuaternion(&rotateTransform, &RotAngle);
		D3DXMatrixTranslation(&translateTransform, Posit.x, Posit.y, Posit.z);
		D3DXMatrixScaling(&scaleTransform, Scale.x, Scale.y, Scale.z);
		
		transform = scaleTransform * rotateTransform * translateTransform;

		return transform;
	}
}