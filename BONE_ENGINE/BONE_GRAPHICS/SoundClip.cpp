#include "Common.h"
#include "SoundClip.h"
#include "SoundManager.h"
#include "Transform3D.h"
#include "ResourceManager.h"

namespace BONE_GRAPHICS
{
    SoundClip::SoundClip()
    {
        SetTypeName("SoundClip");
    }

    SoundClip::~SoundClip()
    {
    }

    void SoundClip::LoadContent()
    {
    }

    void SoundClip::AttachObject(GameObject* owner)
    {
        this->owner = owner;
    }

    void SoundClip::AddClip(std::string file, float volume, bool loop, bool startPaused, float minDist, float maxDist)
    {
        Sound clip;
        clip.volume = volume;
        clip.loop = loop;
        clip.startPaused = startPaused;
        clip.minDist = minDist;
        clip.maxDist = maxDist;

        list.insert(std::pair<std::string, Sound>(file, clip));

        /*std::string fullpath = "";
        ResourceMgr->ExistFile(file, &fullpath);

        Vec3 pos = GET_TRANSFORM_3D(owner)->GetPosition();

        list[file].sound = SoundMgr->GetEngine()->play3D(
            fullpath.c_str(),
            vec3df(pos.x, pos.y, pos.z),
            list[file].loop,
            list[file].startPaused,
            true
        );*/
    }

    void SoundClip::ChangeInfo(std::string file, float volume, bool loop, bool startPaused, float minDist, float maxDist)
    {
        auto item = list.find(file);
        if (item == list.end())
            return;
        
        item->second.volume = volume;
        item->second.loop = loop;
        item->second.startPaused = startPaused;
        item->second.minDist = minDist;
        item->second.maxDist = maxDist;

        if (item->second.sound != nullptr)
        {
            item->second.sound->setIsLooped(item->second.loop);
            item->second.sound->setVolume(item->second.volume);
            item->second.sound->setMinDistance(item->second.minDist);
            item->second.sound->setMaxDistance(item->second.maxDist);
        }
    }

    void SoundClip::RemoveClip(std::string clip)
    {
        auto item = list.find(clip);
        if (item == list.end())
            return;

        if (item->second.sound != nullptr)
            item->second.sound->drop();

        item->second.sound = nullptr;

        list.erase(item);
    }

    void SoundClip::Update()
    {
        for each(auto item in list)
        {
            if (item.second.sound != nullptr)
            {
                Vec3 pos = GET_TRANSFORM_3D(owner)->GetPosition();

                item.second.sound->setPosition(vec3df(pos.x, pos.y, pos.z));
            }
        }
    }

    bool SoundClip::IsPlaying(std::string clip)
    {
        auto item = list.find(clip);
        if (item == list.end())
            return false;

        if (item->second.sound == nullptr)
            return false;

        if (item->second.sound->isFinished())
            return false;

        return true;
    }

    void SoundClip::Play(std::string clip)
    {
        auto item = list.find(clip);
        if (item == list.end())
            return;

        Vec3 pos = GET_TRANSFORM_3D(owner)->GetPosition();
                
        std::string fullpath = "";
        ResourceMgr->ExistFile(clip, &fullpath);

        if (item->second.sound != nullptr)
            item->second.sound->drop();

        item->second.sound = SoundMgr->GetEngine()->play3D(
            fullpath.c_str(),
            vec3df(pos.x, pos.y, pos.z),
            item->second.loop, 
            false,
            true
        );

        if (item->second.sound != nullptr)
        {
            item->second.sound->setVolume(item->second.volume);
            item->second.sound->setMinDistance(item->second.minDist);
            item->second.sound->setMaxDistance(item->second.maxDist);
        }
    }
    
    void SoundClip::Stop(std::string clip)
    {
        auto item = list.find(clip);
        
        if (item == list.end())
            return;

        if (item->second.sound != nullptr)
            item->second.sound->stop();
    }

    std::map<std::string, Sound>* SoundClip::GetClips()
    {
        return &list;
    }
}