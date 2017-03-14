#pragma once
#include "Common.h"
#include "IShader.h"
#include "Component.h"
#include "GameObject.h"
#include "Transform3D.h"
#include "ResourceManager.h"
using namespace BONE_SYSTEM;

namespace BONE_GRAPHICS
{
	class StaticMesh : public Component
	{
	private:
		MESH_INFO*  meshInfo;
    	D3DMATERIAL9* meshMaterials;

        std::string address;
        std::string*  textureAddress;

		int renderMode;
		bool IsInit;

	public:
		enum RENDER_MODE {
			RENDER_STENCIL, RENDER_ALPHA, RENDER_DEFAULT,
		};

		StaticMesh();
        virtual ~StaticMesh();

        void LoadContent();
		void Render(IShader* shaderOpt, GameObject* object);

        void SetRenderMode(int mode);
        void SetTexturesAddress(std::string* address);
        void SetFileAddress(std::string address);

        bool CheckMouseRayInMesh(Transform3D* tr, float *dist);
		
		std::string* GetTexturesAddress();
		std::string GetFileAddress();
	};
}