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
		std::map<std::string, MESH_INFO>				staticMeshList;
		std::map<std::string, LPDIRECT3DTEXTURE9>	textureList;
		std::map<std::string, LPD3DXEFFECT>			shaderList;

	public:
		void InitializeMembers();

		ResourceManager() {}
		virtual ~ResourceManager() {}
		
		virtual void ReleaseMembers();

	public:
        bool ExistFile(std::string name, std::string* full_path);
        std::vector<std::string> ExistFiles(std::string path);

		MESH_INFO*			 LoadMesh(std::string name);
		MESH_INFO*			 FindMesh(std::string name);
		LPDIRECT3DTEXTURE9	 LoadTexture(std::string name);
		LPD3DXEFFECT		 LoadEffect(std::string name);

		void ReleaseMesh(std::string name);
		void ReleaseTexture(std::string name);
		void ReleaseEffect(std::string name);

		void ReleaseMesh(MESH_INFO* meshInfo);
		void ReleaseTexture(LPDIRECT3DTEXTURE9 texture);
		void ReleaseEffect(LPD3DXEFFECT name);

	};
}