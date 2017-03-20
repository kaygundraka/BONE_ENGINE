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
	class SpriteBillBoard : public Component
	{
	public:
		enum RENDER_MODE {
			RENDER_STENCIL, RENDER_ALPHA, RENDER_DEFAULT,
		};

	private:
		std::string fileName;
		
		int width;
		int height;
		float rectWidth;
		float rectHeight;
		int animeCut;
		int animeScene;
		float alpha;
		bool IsShow;
		int curAnimeCut;
		int curAnimeScene;
		RECT curRect;
		bool rightPlay;

		IDirect3DVertexBuffer9* vertexBuffer;
		IDirect3DIndexBuffer9*	indexBuffer;

		D3DMATERIAL9 meshMaterial;
		GameObject*	target;

		RENDER_MODE renderMode;
		bool IsInit;

	public:
		void SetAnimation(int width, int height, int animationCut, int animationScene, float alpha);

		void SetPlayDirection(bool isRight);
		void PlayCutAnimation(float cutTimer);
		void PlayFullAnimation(float cutTimer);
		void SelectAnimation(int sceneIndex);
		
		void SetRectSize(float width, float height);
		Vec2 GetRectSize();

		SpriteBillBoard();
		virtual ~SpriteBillBoard();

        void LoadContent();
        void Render(IShader* shaderOpt, GameObject* cameraObject);
		void SetTargetCamera(GameObject* targetCamera);
				
		void SetRenderMode(RENDER_MODE mode);

		void SetTexture(std::string fileName);
		bool CheckMouseRayInMesh(Transform3D* tr);

		std::string GetTexture();
	};
}