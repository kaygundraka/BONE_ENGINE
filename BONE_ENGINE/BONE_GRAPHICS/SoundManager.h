#pragma once
#include "ISingleton.h"
#include "Common.h"
#include <MultiThreadSync.h>
using namespace irrklang;

namespace BONE_GRAPHICS
{
    class SoundManager : public ISingleton <SoundManager>, public BONE_SYSTEM::MultiThreadSync<SoundManager>
    {
    private:
        ISoundEngine* engine;

    public:
        void InitializeMembers();

        SoundManager() {}
        virtual ~SoundManager() {}

        virtual void ReleaseMembers();

    public:
        ISoundSource* CreateSound(std::string name);
        ISoundEngine* GetEngine();

        void ListenerUpdate(Vec3 pos, Vec3 lookDir, Vec3 upVector);
        void StopAllSound();

        void Play2D(std::string file, float Volume);

        void RemoveSound(ISoundSource* source);
        void RemoveAllSound();
    };
}