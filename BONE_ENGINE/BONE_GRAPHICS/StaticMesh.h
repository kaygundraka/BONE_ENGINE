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

        std::string fileName;
        std::string* textureFiles;
        std::string* normalTextureFiles;

		int renderMode;
		bool IsInit;
        bool showBox;
        bool show;

        D3DXCOLOR color;

	public:
		enum RENDER_MODE {
			RENDER_STENCIL, RENDER_ALPHA, RENDER_DEFAULT,
		};

		StaticMesh();
        virtual ~StaticMesh();

        void LoadContent();
		void Render(IShader* shaderOpt, GameObject* object);
        
        void SetRenderMode(int mode);
        bool IsShowMeshBOx();
        void SetTextures(std::string* fileNames);
        void SetFile(std::string fileName);

        bool CheckMouseRayInMesh(Transform3D* tr, float *dist);

        void Hide();
        void Show();
        bool IsShow();
		
		std::string* GetTextures();
        std::string* GetNormalTextures();
		std::string GetFile();
	};
}