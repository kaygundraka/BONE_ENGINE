#pragma once
#include <MultiThreadSync.h>
#include "ISingleton.h"
#include "Common.h"
#include <map>

namespace BONE_GRAPHICS
{
	struct MeshInfo {
		DWORD _numMaterials;
		LPD3DXBUFFER _buffer;
		LPD3DXMESH _mesh;
	};

	class ResourceManager : public ISingleton <ResourceManager>, public BONE_SYSTEM::CMultiThreadSync<ResourceManager>
	{
	private:
		std::map<string, MeshInfo>				staticMeshList;
		std::map<string, LPDIRECT3DTEXTURE9>	textureList;
		std::map<string, LPD3DXEFFECT>			shaderList;

	public:
		void InitializeMembers();

		ResourceManager() {}
		virtual ~ResourceManager() {}
		
		virtual void ReleaseMembers();

	public:
		MeshInfo*			 LoadMesh(string _address);
		MeshInfo*			 FindMesh(string _address);
		LPDIRECT3DTEXTURE9	 LoadTexture(string _address);
		LPD3DXEFFECT		 LoadEffect(string _address);

		void ReleaseMesh(string _address);
		void ReleaseTexture(string _address);
		void ReleaseEffect(string _address);

		void ReleaseMesh(MeshInfo* _meshInfo);
		void ReleaseTexture(LPDIRECT3DTEXTURE9 _texture);
		void ReleaseEffect(LPD3DXEFFECT _address);

	};
}