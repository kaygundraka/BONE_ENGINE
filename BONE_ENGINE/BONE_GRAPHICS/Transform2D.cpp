#include "Common.h"
#include "Transform2D.h"

namespace BONE_GRAPHICS
{
	Transform2D::Transform2D()
	{
		CThreadSync sync;

		position = Vector3(RenderMgr->GetWidth() / 2, RenderMgr->GetHeight() / 2, 0);
		pivotCenter = Vector2(0.5f, 0.5f);
		rotAngle = 0;
		scale = Vector2(1, 1);

		D3DXMatrixIdentity(&transform);

		parent = NULL;

		SetTypeName("Transform2D");
	}

	Transform2D::~Transform2D()
	{
		CThreadSync sync;
	}

	void Transform2D::SetSize(float _width, float _height)
	{
		CThreadSync sync;

		size.x = _width;
		size.y = _height;
	}
	
	Vector2 Transform2D::GetSize()
	{
		CThreadSync sync;

		return size;
	}

	void Transform2D::SetPosition(Vector3 _position)
	{
		CThreadSync sync;

		position = _position;
	}

	void Transform2D::SetPosition(float _x, float _y, float _z)
	{
		CThreadSync sync;

		position.x = _x;
		position.y = _y;
		position.z = _z;
	}

	void Transform2D::Translate(Vector3 _position)
	{
		CThreadSync sync;

		position += _position;
	}

	void Transform2D::Translate(float _x, float _y, float _z)
	{
		CThreadSync sync;

		position.x += _x;
		position.y += _y;
		position.z += _z;
	}

	Vector3 Transform2D::GetPosition()
	{
		CThreadSync sync;

		return position;
	}

	void Transform2D::Scale(Vector2 _scale)
	{
		CThreadSync sync;

		scale += _scale;
	}

	void Transform2D::Scale(float _x, float _y)
	{
		CThreadSync sync;

		scale.x += _x;
		scale.y += _y;
	}
	
	void Transform2D::SetScale(Vector2 _scale)
	{
		CThreadSync sync;

		scale = _scale;
	}

	void Transform2D::SetScale(float _x, float _y)
	{
		CThreadSync sync;

		scale.x = _x;
		scale.y = _y;
	}

	Vector2 Transform2D::GetScale()
	{
		CThreadSync sync;

		return scale;
	}

	void Transform2D::Rotate(float _rotAngle)
	{
		CThreadSync sync;

		rotAngle += _rotAngle;
	}	

	void Transform2D::SetRotate(float _rotAngle)
	{
		CThreadSync sync;

		rotAngle = _rotAngle;
	}
	
	float Transform2D::GetRotateAngle()
	{
		CThreadSync sync;

		return rotAngle;
	}

	void Transform2D::SetPivotCenter(Vector2 _pivotCenter)
	{
		CThreadSync sync;

		pivotCenter = _pivotCenter;
	}
	
	void Transform2D::SetPivotCenter(float _x, float _y)
	{
		CThreadSync sync;

		pivotCenter.x = _x;
		pivotCenter.y = _y;
	}

	Vector2 Transform2D::GetPivotCenter()
	{
		CThreadSync sync;

		return pivotCenter;
	}

	Matrix Transform2D::GetTransform()
	{
		CThreadSync sync;

		D3DXMatrixIdentity(&transform);
		D3DXMatrixTransformation2D(&transform, &pivotCenter, NULL, &scale, &pivotCenter, rotAngle, &Vector2(position.x, position.y));

		return transform;
	}

	void Transform2D::AttachObject(GameObject* _parent)
	{
		CThreadSync sync;

		parent = _parent;
	}
}