#pragma once
#include "Common.h"
#include "BoneHierarchyLoader.h"
#include "IShader.h"
#include "Component.h"
using namespace BONE_SYSTEM;

namespace BONE_GRAPHICS
{
	struct KEYFRAMERATE {
		LPD3DXANIMATIONSET AnimationSet;
		UINT Vertex_Index;
		float AnimationSpeed;
		bool IsPlay;
	};

	class SkinnedMesh : public Component {
	public:
		virtual ~SkinnedMesh();

		SkinnedMesh();
		void SetFileAddress(string _address);
		void LoadContent();
		string GetFileAddress();

		void Render(IShader* _shaderOption, GameObject* _object, float _timeDelta);
		
		void UpdateAnimation(float _timeDelta);
		void PlayAnimation(string _animationName, bool _IsPlay, bool bRotateLoop);
		void SetAnimation(string _name, int _index, float _animationSpeed);
		void SetAnimation(string _name, float _animationSpeed);

		void CheckInRay(RAY *_ray, Bone* _bone, float& _curDist);

		string address;
		LPD3DXMESH GetMesh();

	private:
		void SetupBoneMatrixPointers(Bone* _bone);

		void UpdateMatrices(Bone *_bone, D3DXMATRIX *_parentMatrix);
		void SoftwareRendering(IShader* _shaderOption, GameObject* _object, Bone *_bone);

		D3DXFRAME *rootBone;

		/// 에니메이션 관련 변수
		BoneHierarchyLoader boneHierarchy;
		LPD3DXANIMATIONCONTROLLER animationContainer;
		map<string, KEYFRAMERATE> animationSet;
		string animationName;
		bool IsPlay;
		bool IsRotateLoop;

		// 기본 3D 관련 변수
		bool IsInit;
	};
}