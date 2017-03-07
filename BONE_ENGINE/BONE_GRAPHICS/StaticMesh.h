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
		MESH_INFO*		meshInfo;

		string			address;
		string*			textureAddress;
		D3DMATERIAL9*	meshMaterials;
				
		int renderMode;
		bool IsInit;

	public:
		enum RENDER_MODE {
			RENDER_STENCIL, RENDER_ALPHA, RENDER_DEFAULT,
		};

		StaticMesh();
        virtual ~StaticMesh();

        void LoadContent();
		void Render(IShader* _shaderOption, GameObject* _object);

        void SetRenderMode(int _mode);
        void SetTexturesAddress(string* _address);
        void SetFileAddress(string _address);

        bool CheckMouseRayInMesh(Transform3D* _tr);
		
		string* GetTexturesAddress();
		string GetFileAddress();
	};
}