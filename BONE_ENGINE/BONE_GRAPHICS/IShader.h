#pragma once
#include "Common.h"
#include "Component.h"
#include "GameObject.h"

namespace BONE_GRAPHICS
{
	class IShader : public Component
	{
	protected:
		LPD3DXEFFECT* shader;
		std::string address;
        
        void SetAddress(std::string address);

	public:
		IShader();
		virtual ~IShader();

		void LoadContent();
		
        std::string GetAddress();

        virtual void Render(int index, std::vector<LPDIRECT3DTEXTURE9> textures, GameObject* parent) {}
		virtual void Render(int index, GameObject* parent) = 0;

		LPD3DXEFFECT GetShader();
	};
}