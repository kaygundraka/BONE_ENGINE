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
	class SpriteBillBoard : public Component, public MultiThreadSync<SpriteBillBoard>
	{
	public:
		enum RENDER_MODE {
			RENDER_STENCIL, RENDER_ALPHA, RENDER_DEFAULT,
		};

	private:
		string			textureAddress;
		
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

		D3DMATERIAL9	meshMaterial;
		GameObject*	target;

		RENDER_MODE renderMode;
		bool IsInit;

	public:
		void SetAnimation(int _width, int _height, int _animationCut, int _animationScene, float _alpha);

		void SetPlayDirection(bool _isRight);
		void PlayCutAnimation(float _timeDelta, float _cutTimer);
		void PlayFullAnimation(float _timeDelta, float _cutTimer);
		void SelectAnimation(int _sceneIndex);
		
		void SetRectSize(float _width, float _height);
		Vector2 GetRectSize();

		SpriteBillBoard();

		void LoadContent();

		virtual ~SpriteBillBoard();

		void Render(IShader* _shaderOption, GameObject* _cameraObject);
		void SetTargetCamera(GameObject* _targetCamera);
				
		void SetRenderMode(RENDER_MODE _mode);

		void SetTexturesAddress(string _address);
		bool CheckMouseRayInMesh(Transform3D* _tr);

		string GetTexturesAddress();
	};
}