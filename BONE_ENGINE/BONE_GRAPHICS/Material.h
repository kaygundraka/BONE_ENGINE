#pragma once
#include "Common.h"
#include "Component.h"

namespace BONE_GRAPHICS
{
    class Material : public Component 
    {
    private:
        RGBA ambient;
        RGBA diffuse;
        RGBA emissive;
        RGBA specular;
        
        float shininess;
    
    public:
        Material();
        virtual ~Material();

        void SetAmbient(float r, float g, float b, float a);
        void SetDiffuse(float r, float g, float b, float a);
        void SetEmissive(float r, float g, float b, float a);
        void SetSpecular(float r, float g, float b, float a);
        void SetShininess(float value);

        void SetAmbient(RGBA color);
        void SetDiffuse(RGBA color);
        void SetEmissive(RGBA color);
        void SetSpecular(RGBA color);

        RGBA GetAmbient();
        RGBA GetDiffuse();
        RGBA GetEmissive();
        RGBA GetSpecular();
        float GetShininess();
    };
}