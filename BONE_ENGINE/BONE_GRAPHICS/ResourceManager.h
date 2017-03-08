#pragma once
#include "ISingleton.h"
#include "Common.h"

namespace BONE_GRAPHICS
{
	typedef struct _MESH_INFO {
		int numMaterials;
		LPD3DXBUFFER buffer;
        Mesh mesh;
	} MESH_INFO;

	class ResourceManager : public ISingleton <ResourceManager>, public BONE_SYSTEM::MultiThreadSync<ResourceManager>
	{
	private:
		std::map<string, MESH_INFO>				staticMeshList;
		std::map<string, LPDIRECT3DTEXTURE9>	textureList;
		std::map<string, LPD3DXEFFECT>			shaderList;

	public:
		void InitializeMembers();

		ResourceManager() {}
		virtual ~ResourceManager() {}
		
		virtual void ReleaseMembers();

	public:
        bool ExistFile(std::string name, std::string* full_path);

		MESH_INFO*			 LoadMesh(string name);
		MESH_INFO*			 FindMesh(string name);
		LPDIRECT3DTEXTURE9	 LoadTexture(string name);
		LPD3DXEFFECT		 LoadEffect(string name);

		void ReleaseMesh(string name);
		void ReleaseTexture(string name);
		void ReleaseEffect(string name);

		void ReleaseMesh(MESH_INFO* meshInfo);
		void ReleaseTexture(LPDIRECT3DTEXTURE9 texture);
		void ReleaseEffect(LPD3DXEFFECT name);

	};
}