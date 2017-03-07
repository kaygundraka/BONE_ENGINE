#pragma once
#include "Common.h"
#include "Component.h"
#include "GameObject.h"
#include <MultiThreadSync.h>
using namespace BONE_SYSTEM;

namespace BONE_GRAPHICS
{
	class Transform2D : public Component
	{
	private:
		Vector3 position;
		float rotAngle;
		Vector2 scale;
		Vector2 pivotCenter;
		Vector2 size;

		Matrix transform;
		
		GameObject* parent;

	public:
		Transform2D();
		virtual ~Transform2D();

		void SetPosition(Vector3 pos);
		void SetPosition(float x, float y, float z);
		void Translate(Vector3 pos);
		void Translate(float x, float y, float z);
		Vector3 GetPosition();

		void Scale(Vector2 scale);
		void Scale(float x, float y);
		void SetScale(Vector2 scale);
		void SetScale(float x, float y);
		Vector2 GetScale();

		void Rotate(float rotAngle);
		void SetRotate(float rotAngle);
		float GetRotateAngle();
		
		void SetPivotCenter(Vector2 pivotCenter);
		void SetPivotCenter(float x, float y);
		Vector2 GetPivotCenter();

		void SetSize(float width, float height);
		Vector2 GetSize();

		Matrix GetTransform();

		void AttachObject(GameObject* parent);
	};
}