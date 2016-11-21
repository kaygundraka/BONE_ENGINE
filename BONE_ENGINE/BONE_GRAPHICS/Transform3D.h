#pragma once
#include "Common.h"
#include "Component.h"
#include "GameObject.h"
#include <MultiThreadSync.h>
using namespace BONE_SYSTEM;

namespace BONE_GRAPHICS
{
	class Transform3D : public Component, public CMultiThreadSync<Transform3D> {
	private:
		Vector3 position;
		Quaternion rotAngle;
		Vector3 scale;
		Vector3 forward;
		Vector3 up;

		Matrix transform;
		Matrix rotateTransform;
		Matrix translateTransform;
		Matrix scaleTransform;

		GameObject *parent;

	public:

		Transform3D();
		virtual ~Transform3D();

		void SetForward(Vector3 _forward);
		void SetForward(float _x, float _y, float _z);
		Vector3 GetForward();
		Vector3 GetLeft();

		void SetPosition(Vector3 _position);
		void SetPosition(float _x, float _y, float _z);		
		void Translate(Vector3 _position);
		void Translate(float _x, float _y, float _z);
		Vector3 GetPosition();
		Vector3 GetWorldPositon();

		void Scale(Vector3 _scale);
		void Scale(float _x, float _y, float _z);
		void SetScale(Vector3 _scale);
		void SetScale(float _x, float _y, float _z);
		Vector3 GetScale();
		Vector3 GetWolrdScale();

		void Rotate(Quaternion _rotateAngle);
		void Rotate(float _x, float _y, float _z);
		void SetRotate(Quaternion _rotateAngle);
		void SetRotate(float _x, float _y, float _z);
		Quaternion GetRotateAngle();
		Quaternion GetWorldRotateAngle();

		Matrix GetTransform();

		void AttachObject(GameObject* _parent);
	};
}