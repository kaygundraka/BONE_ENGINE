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
		Vec3 position;
		float rotAngle;
		Vec2 scale;
		Vec2 pivotCenter;
		Vec2 size;

		Matrix transform;
		
		GameObject* parent;

	public:
		Transform2D();
		virtual ~Transform2D();

		void SetPosition(Vec3 pos);
		void SetPosition(float x, float y, float z);
		void Translate(Vec3 pos);
		void Translate(float x, float y, float z);
		Vec3 GetPosition();

		void Scale(Vec2 scale);
		void Scale(float x, float y);
		void SetScale(Vec2 scale);
		void SetScale(float x, float y);
		Vec2 GetScale();

		void Rotate(float rotAngle);
		void SetRotate(float rotAngle);
		float GetRotateAngle();
		
		void SetPivotCenter(Vec2 pivotCenter);
		void SetPivotCenter(float x, float y);
		Vec2 GetPivotCenter();

		void SetSize(float width, float height);
		Vec2 GetSize();

		Matrix GetTransform();

		void AttachObject(GameObject* parent);
	};
}