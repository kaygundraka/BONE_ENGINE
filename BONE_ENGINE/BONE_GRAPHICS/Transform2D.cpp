#include "Common.h"
#include "Transform2D.h"

namespace BONE_GRAPHICS
{
	Transform2D::Transform2D()
	{
		position = Vec3(RenderMgr->GetWidth() / 2, RenderMgr->GetHeight() / 2, 0);
		pivotCenter = Vec2(0.5f, 0.5f);
		rotAngle = 0;
		scale = Vec2(1, 1);

		D3DXMatrixIdentity(&transform);

		parent = nullptr;

		SetTypeName("Transform2D");
	}

	Transform2D::~Transform2D()
	{
	}

	void Transform2D::SetSize(float width, float height)
	{
		size.x = width;
		size.y = height;
	}
	
	Vec2 Transform2D::GetSize()
	{
		return size;
	}

	void Transform2D::SetPosition(Vec3 pos)
	{
		position = pos;
	}

	void Transform2D::SetPosition(float _x, float _y, float _z)
	{
		position.x = _x;
		position.y = _y;
		position.z = _z;
	}

	void Transform2D::Translate(Vec3 _position)
	{
		position += _position;
	}

	void Transform2D::Translate(float _x, float _y, float _z)
	{
		position.x += _x;
		position.y += _y;
		position.z += _z;
	}

	Vec3 Transform2D::GetPosition()
	{
		return position;
	}

	void Transform2D::Scale(Vec2 _scale)
	{
		scale += _scale;
	}

	void Transform2D::Scale(float _x, float _y)
	{
		scale.x += _x;
		scale.y += _y;
	}
	
	void Transform2D::SetScale(Vec2 _scale)
	{
		scale = _scale;
	}

	void Transform2D::SetScale(float _x, float _y)
	{
		scale.x = _x;
		scale.y = _y;
	}

	Vec2 Transform2D::GetScale()
	{
		return scale;
	}

	void Transform2D::Rotate(float _rotAngle)
	{
		rotAngle += _rotAngle;
	}	

	void Transform2D::SetRotate(float _rotAngle)
	{
		rotAngle = _rotAngle;
	}
	
	float Transform2D::GetRotateAngle()
	{
		return rotAngle;
	}

	void Transform2D::SetPivotCenter(Vec2 _pivotCenter)
	{
		pivotCenter = _pivotCenter;
	}
	
	void Transform2D::SetPivotCenter(float _x, float _y)
	{
		pivotCenter.x = _x;
		pivotCenter.y = _y;
	}

	Vec2 Transform2D::GetPivotCenter()
	{
		return pivotCenter;
	}

	Matrix Transform2D::GetTransform()
	{
		D3DXMatrixIdentity(&transform);
		D3DXMatrixTransformation2D(&transform, &pivotCenter, 0, &scale, &pivotCenter, rotAngle, &Vec2(position.x, position.y));

		return transform;
	}

	void Transform2D::AttachObject(GameObject* _parent)
	{
		parent = _parent;
	}
}