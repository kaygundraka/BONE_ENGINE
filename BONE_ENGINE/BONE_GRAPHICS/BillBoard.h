#pragma once
#include "Common.h"
#include "ResourceManager.h"
#include "IShader.h"
#include "Component.h"
#include "GameObject.h"
#include "Transform3D.h"
#include <MultiThreadSync.h>
using namespace BONE_SYSTEM;

namespace BONE_GRAPHICS
{
	class BillBoard : public Component, public MultiThreadSync<BillBoard>
	{
	public:
		enum RENDER_MODE {
			RENDER_STENCIL, RENDER_ALPHA, RENDER_DEFAULT,
		};

	private:
		string			textureAddress;

		IDirect3DVertexBuffer9* vertexBuffer;
		IDirect3DIndexBuffer9*	indexBuffer;
		
		D3DMATERIAL9	meshMaterial;
		GameObject*	target;

		RENDER_MODE renderMode;

		bool IsInit;

		int width;
		int height;
		float rectWidth;
		float rectHeight;
		Rect originRect;

	public:
		
		BillBoard();

		void LoadContent();

		virtual ~BillBoard();

		void Render(IShader* _shaderOption, GameObject* _cameraObject);
		void SetTarget(GameObject* _targetCamera);

		void SetRectSize(float _width, float _height);
		Vector2 GetRectSize();

		void SetOriginRect(Rect _rect);
		void SetOriginRect(Vector2 _leftTop, Vector2 _rightBottom);
		Rect GetOriginRect();

		void SetRenderMode(RENDER_MODE _mode);

		void SetTexturesAddress(string _address, int _width, int _height);
		bool CheckMouseRayInMesh(Transform3D* _tr);

		string GetTexturesAddress();
	};
}