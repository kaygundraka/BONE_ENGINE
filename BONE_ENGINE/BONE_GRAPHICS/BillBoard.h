#pragma once
#include "Common.h"
#include "ResourceManager.h"
#include "IShader.h"
#include "Component.h"
#include "GameObject.h"
#include "Transform3D.h"
using namespace BONE_SYSTEM;

namespace BONE_GRAPHICS
{
	class BillBoard : public Component
	{
	public:
		enum RENDER_MODE {
			RENDER_STENCIL, RENDER_ALPHA, RENDER_DEFAULT,
		};

	private:
		std::string fileName;

		IDirect3DVertexBuffer9* vertexBuffer;
		IDirect3DIndexBuffer9*	indexBuffer;
		
		D3DMATERIAL9 meshMaterial;
		GameObject*	target;

		RENDER_MODE renderMode;

		bool IsInit;
        bool isShow;

		int width;
		int height;
		float rectWidth;
		float rectHeight;

		Rect originRect;

	public:
		BillBoard();

		void LoadContent();

		virtual ~BillBoard();

		void Render(IShader* shaderOption, GameObject* cameraObject);
		void SetTarget(GameObject* targetCamera);

		void SetRectSize(float width, float height);
		Vec2 GetRectSize();

		void SetOriginRect(Rect rect);
		void SetOriginRect(Vec2 leftTop, Vec2 rightBottom);
		Rect GetOriginRect();

		void SetRenderMode(RENDER_MODE mode);

		void SetTexture(std::string fileName, int width, int height);
		bool CheckMouseRayInMesh(Transform3D* tr);

        void SetShow(bool show);
        bool IsShow();

		std::string GetTexture();
	};
}