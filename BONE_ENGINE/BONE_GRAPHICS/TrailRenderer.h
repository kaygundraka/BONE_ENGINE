#pragma once
#include "Common.h"
#include "ResourceManager.h"
#include "IShader.h"
#include "Component.h"
#include "GameObject.h"
#include "Transform3D.h"
#include <vector>
#include <MultiThreadSync.h>
using namespace BONE_SYSTEM;

namespace BONE_GRAPHICS
{
	struct TrailMesh{
		Vector3 pivot;
		Vector3 upDir;
		float curSize;

		float curLiveTime;
	};

	class TrailRenderer: public Component, public MultiThreadSync<TrailRenderer>
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
		
		vector<TrailMesh> trailList;

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
			
		void SetTargetObject(GameObject* _targetObject, Vector3 _pivot);

		void SetSize(float _start, float _end);
		void SetCycle(float _liveCycle, float _createCycle);
		void SetUpVector(Vector3 _upDir);
		void UpdateStatus(double _timeDelta);
		void SetActive(bool _active);
		
		void SetRenderMode(int _mode);
		void Render(IShader* _shaderOption);

		void SetTexturesAddress(string _address);
		string GetTexturesAddress();
	};
}