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
        void Render(ID3DXEffect* effect, GameObject* object);

        void UpdateAnimation();

        void SetFile(std::string fileName);
        void SetAnimation(std::string name);

        std::string GetFile();
        map<std::string, int> GetAnmimationSet();
        std::string GetCurrentAnimation();
        
        bool CheckInRay(RAY *ray, Bone* bone, float& curDist);

        std::string fileName;
        Matrix* GetBoneMatrix(std::string name, Bone* bone = nullptr, Matrix* mat = nullptr);

        void ResetTime();
        double GetAnimationRate();

        void SetAnimationLoop(bool loop);
        bool GetAnimationLoop();

    private:
        void SetupBoneMatrixPointers(Bone* bone);

        void UpdateMatrices(Bone *bone, D3DXMATRIX *parentMatrix);
        void SoftwareRendering(IShader* shaderOpt, GameObject* object, Bone *bone);
        void SoftwareRendering(ID3DXEffect* effect, GameObject* object, Bone* bone);

        void GetAnimationSets();
                
        D3DXFRAME *rootBone;

        bool animationLoop;

        BoneHierarchyLoader boneHierarchy;
        LPD3DXANIMATIONCONTROLLER animationContainer;

        map<std::string, int> animationSets;
        std::string curAnimation;
        bool IsInit;
    };
}