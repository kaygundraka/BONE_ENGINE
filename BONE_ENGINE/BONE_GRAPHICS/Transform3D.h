#pragma once
#include "Common.h"
#include "Component.h"
#include "GameObject.h"
using namespace BONE_SYSTEM;

namespace BONE_GRAPHICS
{
	class Transform3D : public Component {
	private:
		Vec3 position;
		Quater rotAngle;
		Vec3 scale;
		Vec3 forward;
		Vec3 up;

		Matrix transform;
		Matrix rotateTransform;
		Matrix translateTransform;
		Matrix scaleTransform;

		GameObject *parent;
        Matrix* combineMatrix;

	public:

		Transform3D();
		virtual ~Transform3D();

		void SetForward(Vec3 forward);
		void SetForward(float x, float y, float z);
		Vec3 GetForward();
		Vec3 GetLeft();

		void SetPosition(Vec3 pos);
		void SetPosition(float x, float y, float z);		
		void Translate(Vec3 pos);
		void Translate(float x, float y, float z);
		Vec3 GetPosition();
		Vec3 GetWorldPositon();

		void Scale(Vec3 scale);
		void Scale(float x, float y, float z);
		void SetScale(Vec3 scale);
		void SetScale(float x, float y, float z);
		Vec3 GetScale();
		Vec3 GetWolrdScale();

		void Rotate(Quater rotAngle);
		void Rotate(float x, float y, float z);
		void SetRotate(Quater rotAngle);
		void SetRotate(float x, float y, float z);

        Vec3 GetRotateAngle();
        Quater GetRotateQuater();
		Vec3 GetWorldRotateAngle();

		Matrix GetTransform();
        void CombineMatrix(Matrix* matrix);

		void AttachObject(GameObject* parent);
	};
}