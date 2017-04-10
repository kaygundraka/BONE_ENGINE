#pragma once
#include "Common.h"
#include "Component.h"
#include "GameObject.h"

using namespace irrklang;
using namespace BONE_SYSTEM;

namespace BONE_GRAPHICS
{
    struct Sound {
        ISound* sound;

        float volume;

        bool loop;
        bool startPaused;

        float minDist;
        float maxDist;

        Sound()
        {
            sound = nullptr;
        }
    };

    class SoundClip : public Component
    {
    protected:
        GameObject* owner;
        std::map<std::string, Sound> list;
        
    public:
        SoundClip();
        ~SoundClip();

        void LoadContent();

        void AttachObject(GameObject* owner);
        std::map<std::string, Sound>* GetClips();

        void AddClip(std::string file, float volume, bool loop, bool startPaused, float minDist, float maxDist);
        void ChangeInfo(std::string file, float volume, bool loop, bool startPaused, float minDist, float maxDist);
        void RemoveClip(std::string file);
        
        void Update();
        void PlaySound(std::string clip);
        void StopSound(std::string clip);
    };
}