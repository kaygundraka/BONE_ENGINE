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
        
    public:
        PhongShader();
        ~PhongShader();

        virtual void Render(int index, GameObject* parent);
    };
}