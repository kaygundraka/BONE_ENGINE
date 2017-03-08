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
	typedef struct _TRAIL_MESH{
		Vector3 pivot;
		Vector3 upDir;

        float curSize;
		float curLiveTime;
	} TRAIL_MESH;

	class TrailRenderer: public Component
	{
	private:
		IDirect3DVertexBuffer9* vertexBuffer;
		IDirect3DIndexBuffer9*	indexBuffer;
		
		string		textureAddress;
		GameObject*	target;

		float liveCycle;
		float createCycle;
		float curCreateTime;

		Vector3 pivot;
		Vector3 upDir;
		
		float startSize;
		float endSize;
		
		vector<TRAIL_MESH> trailList;

		int renderMode;
		
		bool IsActive;
		bool IsInit;

		void SetMeshBuffer();

	public:
		enum RENDER_MODE {
			RENDER_STENCIL, RENDER_ALPHA, RENDER_DEFAULT,
		};

		TrailRenderer();
		virtual ~TrailRenderer();

		void LoadContent();
			
		void SetTargetObject(GameObject* targetObject, Vector3 pivot);
        void SetSize(float start, float end);
		void SetCycle(float liveCycle, float createCycle);
		void SetUpVector(Vector3 upDir);
        void SetActive(bool active);
		void SetRenderMode(int mode);

        void UpdateStatus();

        void Render(IShader* shaderOpt);

		void SetTexturesAddress(string address);
		string GetTexturesAddress();
	};
}