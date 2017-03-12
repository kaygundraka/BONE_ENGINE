#pragma once
#include "Common.h"
#include "GameObject.h"
#include "Transform3D.h"

namespace BONE_GRAPHICS
{
    class PointLight : public GameObject
    {
    private:
        Transform3D* tr;
        Vector3 pos;

        RGBA ambient;
        RGBA diffuse;
        RGBA specular;
        
        float radius;
        bool status;
        
    public:
        PointLight();
        virtual ~PointLight();

        void SetAmbient(float r, float g, float b, float a);
        void SetDiffuse(float r, float g, float b, float a);
        void SetSpecular(float r, float g, float b, float a);
        void SetRadius(float value);

        void SetLight(bool status);
        void SetAmbient(RGBA color);
        void SetDiffuse(RGBA color);
        void SetSpecular(RGBA color);
        void SetPosition(Vector3 pos);

        RGBA GetAmbient();
        RGBA GetDiffuse();
        RGBA GetSpecular();
        Vector3 GetPosition();
        float GetRadius();

        virtual void Init();
    };
}