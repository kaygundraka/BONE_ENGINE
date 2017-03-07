#pragma once
#include "Common.h"
#include "IShader.h"
#include "Component.h"
#include "GameObject.h"
#include "Transform3D.h"
#include "ResourceManager.h"
#include <MultiThreadSync.h>
using namespace BONE_SYSTEM;

namespace BONE_GRAPHICS
{
	class StaticMesh : public Component, public MultiThreadSync<StaticMesh>
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

		void LoadContent();

		virtual ~StaticMesh();

		void Render(IShader* _shaderOption, GameObject* _object);
		void SetTexturesAddress(string* _address);
		bool CheckMouseRayInMesh(Transform3D* _tr);

		void SetRenderMode(int _mode);

		string* GetTexturesAddress();
		string GetFileAddress();
		void SetFileAddress(string _address);
	};
}