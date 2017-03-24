#pragma once
#include "Common.h"
#include "IShader.h"
#include "GameObject.h"

namespace BONE_GRAPHICS
{
    class PhongShader : public IShader
    {
    private:
        UINT totalPasses;
        D3DXHANDLE technique;
        LPD3DXEFFECT effect;
        
    public:
        PhongShader();
        ~PhongShader();

        void SetMaterial(GameObject* parent);
        void SetEnvironment(GameObject* parent);

        virtual void Render(int index, GameObject* parent);
        virtual void Render(int index, std::vector<LPDIRECT3DTEXTURE9> textures, GameObject* parent);
    };
}