#pragma once
#include "Common.h"
#include "BoneHierarchyLoader.h"
#include "IShader.h"
#include "Component.h"
using namespace BONE_SYSTEM;

namespace BONE_GRAPHICS
{
	typedef struct _KEYFRAMERATE {
		LPD3DXANIMATIONSET AnimationSet;
		UINT Vertex_Index;
		float AnimationSpeed;
		bool IsPlay;
	} KEYFRAMERATE;

	class SkinnedMesh : public Component {
	public:
        SkinnedMesh();
        virtual ~SkinnedMesh();

        void LoadContent();
		void Render(IShader* shaderOpt, GameObject* object);
		
		void UpdateAnimation();
		void PlayAnimation(std::string animationName, bool isPlay, bool rotateLoop);

        void SetAnimation(std::string name, int index, float animationSpeed);
		void SetAnimation(std::string name, float animationSpeed);
        void SetFileAddress(std::string address);

        std::string GetFileAddress();

		void CheckInRay(RAY *ray, Bone* bone, float& curDist);

		std::string address;
		LPD3DXMESH GetMesh();

	private:
		void SetupBoneMatrixPointers(Bone* bone);

		void UpdateMatrices(Bone *bone, D3DXMATRIX *parentMatrix);
		void SoftwareRendering(IShader* shaderOpt, GameObject* object, Bone *bone);

		D3DXFRAME *rootBone;

		BoneHierarchyLoader boneHierarchy;
		
        LPD3DXANIMATIONCONTROLLER animationContainer;
		
        map<std::string, KEYFRAMERATE> animationSet;
		std::string animationName;
		
        bool IsPlay;
		bool IsRotateLoop;
        bool IsInit;
	};
}