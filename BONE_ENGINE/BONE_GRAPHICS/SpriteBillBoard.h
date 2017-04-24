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
    typedef struct _SPRITE_INFO {
        int width;
        int height;
        int animationCut;
        int animationScene;
    } SPRITE_INFO;

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
        double CutTimer;
        double speed;

		IDirect3DVertexBuffer9* vertexBuffer;
		IDirect3DIndexBuffer9*	indexBuffer;

		D3DMATERIAL9 meshMaterial;
		
		RENDER_MODE renderMode;
		bool IsInit;

        bool isTargetCamera;
        bool fullAnimations;

	public:
		void SetAnimation(int width, int height, int animationCut, int animationScene, float alpha);

		void SetPlayDirection(bool isRight);
		void PlayCutAnimation(float cutTimer);
		void PlayFullAnimation(float cutTimer);
		void SelectAnimation(int sceneIndex);
		
		void SetRectSize(float width, float height);
		Vec2 GetRectSize();

        SPRITE_INFO GetSpriteInfo();

		SpriteBillBoard();
		virtual ~SpriteBillBoard();

        void IsFullAnimation(bool isFull);
        bool IsFullAnimation();

        void SetSpeed(float speed);
        float GetSpeed();
        float GetAlpha();

        void LoadContent();
        void Render(IShader* shaderOpt, GameObject* cameraObject);
		void SetTargetCamera(bool isTargetCamera);
        bool IsTargetCamera();
				
		void SetRenderMode(RENDER_MODE mode);

		void SetTexture(std::string fileName);
		bool CheckMouseRayInMesh(Transform3D* tr);

		std::string GetTexture();
	};
}