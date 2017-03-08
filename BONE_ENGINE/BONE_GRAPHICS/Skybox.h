#pragma once
#include "Common.h"
#include "Vertex.h"
#include "Camera.h"

namespace BONE_GRAPHICS
{
	class Skybox
	{
	private:
		IDirect3DVertexBuffer9* VB[6];
		IDirect3DIndexBuffer9* IB[6];
		LPDIRECT3DTEXTURE9 meshTextures[6];
		D3DXMATRIX matWrd;
		bool Is_Init;

	public:
		Skybox();

		bool SetSkybox(char* name, std::string fileType);
		void Render(GameObject* cameraObject);
		void Release();
	};
}