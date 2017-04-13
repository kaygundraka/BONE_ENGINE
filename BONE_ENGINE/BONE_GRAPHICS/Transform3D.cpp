#include "Common.h"
#include "Transform3D.h"

namespace BONE_GRAPHICS
{
	Transform3D::Transform3D()
	{
		position = Vec3(0, 0, 0);
		rotAngle = Quater(0, 0, 0, 1);
		scale = Vec3(1, 1, 1);
		forward = Vec3(0, 0, -1);
		up = Vec3(0, 1, 0);
		
		D3DXMatrixIdentity(&transform);
		D3DXMatrixIdentity(&rotateTransform);
		D3DXMatrixIdentity(&translateTransform);
		D3DXMatrixIdentity(&scaleTransform);

		parent = nullptr;
        combineMatrix = nullptr;

		SetTypeName("Transform3D");
	}

	Transform3D::~Transform3D()
	{
	}

	void Transform3D::SetForward(Vec3 forward)
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

	Vec3 Transform3D::GetForward()
	{
		Vec3 Forward = forward;

		D3DXVec3TransformNormal(&Forward, &Forward, &transform);
		D3DXVec3Normalize(&Forward, &Forward);

		return Forward;
	}

	Vec3 Transform3D::GetLeft()
	{
		Vec3 Left;
		Vec3 Forward = forward;
		Vec3 Up = up;

		D3DXVec3TransformNormal(&Up, &Up, &transform);
		D3DXVec3Normalize(&Up, &Up);

		D3DXVec3TransformNormal(&Forward, &Forward, &transform);
		D3DXVec3Normalize(&Forward, &Forward);

		D3DXVec3Cross(&Left, &Forward, &Up);
		D3DXVec3Normalize(&Left, &Left);
		
		return Left;
	}

	void Transform3D::SetPosition(Vec3 pos)
	{
		this->position = pos;
	}

	void Transform3D::SetPosition(float x, float y, float z)
	{
		position.x = x;
		position.y = y;
		position.z = z;
	}

	void Transform3D::Translate(Vec3 pos)
	{
		position += pos;
	}

	void Transform3D::Translate(float x, float y, float z)
	{
		position.x += x;
		position.y += y;
		position.z += z;
	}

	Vec3 Transform3D::GetPosition()
	{
        Vec3 result = position;

        if (combineMatrix != nullptr)
        {
            //D3DXVec3TransformCoord(&result, &result, combineMatrix);
            result.x += ((*combineMatrix)._41);
            result.y += ((*combineMatrix)._42);
            result.z += ((*combineMatrix)._43);
        }
		return result;
	}

	Vec3 Transform3D::GetWorldPositon()
	{
		GameObject* parentPtr = parent;

		Vec3 Posit = position;

        if (combineMatrix != nullptr)
        {
            Posit.x += ((*combineMatrix)._41);
            Posit.y += ((*combineMatrix)._42);
            Posit.z += ((*combineMatrix)._43);
            //D3DXVec3TransformCoord(&Posit, &Posit, combineMatrix);
        }

		while (parentPtr != nullptr)
		{
			Posit += ((Transform3D*)parentPtr->GetComponent("Transform3D"))->GetPosition();

			parentPtr = parentPtr->GetParent();
		}

		D3DXMatrixTranslation(&translateTransform, Posit.x, Posit.y, Posit.z);

		return Posit;
	}

	void Transform3D::Scale(Vec3 scale)
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

	void Transform3D::SetScale(Vec3 scale)
	{
        this->scale = scale;
	}

	void Transform3D::SetScale(float x, float y, float z)
	{
		scale.x = x;
		scale.y = y;
		scale.z = z;
	}

	Vec3 Transform3D::GetScale()
	{
		return scale;
	}

	Vec3 Transform3D::GetWolrdScale()
	{
		GameObject* parentPtr = parent;

		Vec3 Scale = scale;

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

	void Transform3D::Rotate(Quater rotAngle)
	{
		D3DXQuaternionMultiply(&this->rotAngle, &this->rotAngle, &rotAngle);
	}

	void Transform3D::Rotate(float x, float y, float z)
	{
		Quater quater;
		D3DXQuaternionRotationYawPitchRoll(&quater, y, x, z);
		
		D3DXQuaternionMultiply(&rotAngle, &rotAngle, &quater);
	}

	void Transform3D::SetRotate(Quater rotAngle)
	{
		this->rotAngle = rotAngle;
	}

	void Transform3D::SetRotate(float x, float y, float z)
	{
		Quater quater;
		D3DXQuaternionRotationYawPitchRoll(&quater, y, x, z);

		rotAngle = quater;
	}

	Vec3 Transform3D::GetRotateAngle()
	{
        float sqw = rotAngle.w * rotAngle.w;
        float sqx = rotAngle.x * rotAngle.x;
        float sqy = rotAngle.y * rotAngle.y;
        float sqz = rotAngle.z * rotAngle.z;

        float pitch = asinf(2.0f * (rotAngle.w * rotAngle.x - rotAngle.y * rotAngle.z));
        float yaw = atan2f(2.0f * (rotAngle.x * rotAngle.z + rotAngle.w * rotAngle.y), (-sqx - sqy + sqz + sqw));
        float roll = atan2f(2.0f * (rotAngle.x * rotAngle.y + rotAngle.w * rotAngle.z), (-sqx + sqy - sqz + sqw));

		return Vec3(pitch, yaw, roll);
	}

    Vec3 Transform3D::GetWorldRotateAngle()
    {
        GameObject* parentPtr = parent;

        Quater result = rotAngle;

        while (parentPtr != nullptr)
        {
            result *= ((Transform3D*)parentPtr->GetComponent("Transform3D"))->GetRotateQuater();

            parentPtr = parentPtr->GetParent();
        }

        float sqw = result.w * result.w;
        float sqx = result.x * result.x;
        float sqy = result.y * result.y;
        float sqz = result.z * result.z;

        float pitch = asinf(2.0f * (result.w * result.x - result.y * result.z));
        float yaw = atan2f(2.0f * (result.x * result.z + result.w * result.y), (-sqx - sqy + sqz + sqw));
        float roll = atan2f(2.0f * (result.x * result.y + result.w * result.z), (-sqx + sqy - sqz + sqw));

        return Vec3(pitch, yaw, roll);

        //D3DXMatrixRotationQuaternion(&rotateTransform, &result);

        //return result;
    }

    Quater Transform3D::GetRotateQuater()
    {
        return rotAngle;
    }

    Quater Transform3D::GetWorldRotateQuater()
    {
        GameObject* parentPtr = parent;

        Quater result = rotAngle;

        while (parentPtr != nullptr)
        {
            result *= ((Transform3D*)parentPtr->GetComponent("Transform3D"))->GetRotateQuater();

            parentPtr = parentPtr->GetParent();
        }

        return result;
    }

	void Transform3D::AttachObject(GameObject* parent)
	{
		this->parent = parent;
	}

    void Transform3D::CombineMatrix(Matrix* matrix)
    {
        combineMatrix = matrix;
    }

	Matrix Transform3D::GetTransform()
	{
		GameObject* parentPtr = parent;

		Vec3 Scale = scale;
		Vec3 Posit = position;
		Quater RotAngle = rotAngle;

        D3DXMatrixRotationQuaternion(&rotateTransform, &RotAngle);
        D3DXMatrixTranslation(&translateTransform, Posit.x, Posit.y, Posit.z);
        D3DXMatrixScaling(&scaleTransform, Scale.x, Scale.y, Scale.z);

        transform = scaleTransform * rotateTransform * translateTransform;

        if (combineMatrix != nullptr)
        {
            Matrix mat;
            //D3DXMatrixRotationY(&mat, 3.14f);
            //D3DXMatrixMultiply(&transform, &transform, &mat);

            //Matrix matBone = *combineMatrix;

            //D3DXMatrixMultiply(&matBone, &matBone, &mat);
            transform *= *combineMatrix;
        }

		while (parentPtr != nullptr)
		{
            transform *= ((Transform3D*)parentPtr->transform3D)->GetTransform();

            parentPtr = parentPtr->GetParent();
		}
        
		return transform;
	}
}