#pragma once
#include "Common.h"
#include "Component.h"
#include "GameObject.h"
using namespace BONE_SYSTEM;

namespace BONE_GRAPHICS
{
	class Transform3D : public Component {
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

		void SetForward(Vector3 forward);
		void SetForward(float x, float y, float z);
		Vector3 GetForward();
		Vector3 GetLeft();

		void SetPosition(Vector3 pos);
		void SetPosition(float x, float y, float z);		
		void Translate(Vector3 pos);
		void Translate(float x, float y, float z);
		Vector3 GetPosition();
		Vector3 GetWorldPositon();

		void Scale(Vector3 scale);
		void Scale(float x, float y, float z);
		void SetScale(Vector3 scale);
		void SetScale(float x, float y, float z);
		Vector3 GetScale();
		Vector3 GetWolrdScale();

		void Rotate(Quaternion rotAngle);
		void Rotate(float x, float y, float z);
		void SetRotate(Quaternion rotAngle);
		void SetRotate(float x, float y, float z);
		Quaternion GetRotateAngle();
		Quaternion GetWorldRotateAngle();

		Matrix GetTransform();

		void AttachObject(GameObject* parent);
	};
}