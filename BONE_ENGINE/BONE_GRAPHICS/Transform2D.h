#pragma once
#include "Common.h"
#include "Component.h"
#include "GameObject.h"
#include <MultiThreadSync.h>
using namespace BONE_SYSTEM;

namespace BONE_GRAPHICS
{
	class Transform2D : public Component, public CMultiThreadSync<Transform2D>
	{
	private:
		Vector3 position;
		float rotAngle;
		Vector2 scale;
		Vector2 pivotCenter;
		Vector2 size;

		Matrix transform;
		
		GameObject *parent;

	public:
		Transform2D();
		virtual ~Transform2D();

		void SetPosition(Vector3 _position);
		void SetPosition(float _x, float _y, float _z);
		void Translate(Vector3 _position);
		void Translate(float _x, float _y, float _z);
		Vector3 GetPosition();

		void Scale(Vector2 _scale);
		void Scale(float _x, float _y);
		void SetScale(Vector2 _scale);
		void SetScale(float _x, float _y);
		Vector2 GetScale();

		void Rotate(float _rotAngle);
		void SetRotate(float _rotAngle);
		float GetRotateAngle();
		
		void SetPivotCenter(Vector2 _pivotCenter);
		void SetPivotCenter(float _x, float _y);
		Vector2 GetPivotCenter();

		void SetSize(float _width, float _height);
		Vector2 GetSize();

		Matrix GetTransform();

		void AttachObject(GameObject* _parent);
	};
}