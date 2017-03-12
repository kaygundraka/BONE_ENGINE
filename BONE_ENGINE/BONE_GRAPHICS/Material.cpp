#include "Common.h"
#include "Material.h"

namespace BONE_GRAPHICS
{
    Material::Material()
    {
        SetTypeName("Material");

        shininess = 1;
    }

    Material::~Material()
    {

    }

    void Material::SetAmbient(float r, float g, float b, float a)
    {
        ambient.r = r;
        ambient.g = g;
        ambient.b = b;
        ambient.a = a;
    }

    void Material::SetDiffuse(float r, float g, float b, float a)
    {
        diffuse.r = r;
        diffuse.g = g;
        diffuse.b = b;
        diffuse.a = a;
    }
    
    void Material::SetEmissive(float r, float g, float b, float a)
    {
        emissive.r = r;
        emissive.g = g;
        emissive.b = b;
        emissive.a = a;
    }

    void Material::SetSpecular(float r, float g, float b, float a)
    {
        specular.r = r;
        specular.g = g;
        specular.b = b;
        specular.a = a;
    }

    void Material::SetShininess(float value)
    {
        shininess = value;
    }
    
    void Material::SetAmbient(RGBA color)
    {
        ambient = color;
    }
    
    void Material::SetDiffuse(RGBA color)
    {
        diffuse = color;
    }

    void Material::SetEmissive(RGBA color)
    {
        emissive = color;
    }
    
    void Material::SetSpecular(RGBA color)
    {
        specular = color;
    }

    RGBA Material::GetAmbient()
    {
        return ambient;
    }

    RGBA Material::GetDiffuse()
    {
        return diffuse;
    }

    RGBA Material::GetEmissive()
    {
        return emissive;
    }
    
    RGBA Material::GetSpecular()
    {
        return specular;
    }

    float Material::GetShininess()
    {
        return shininess;
    }
}