#include "Common.h"
#include "SoundClip.h"
#include "SoundManager.h"
#include "Transform3D.h"

namespace BONE_GRAPHICS
{
    SoundClip::SoundClip()
    {
        SetTypeName("SoundClip");
    }

    SoundClip::~SoundClip()
    {
        for each(auto item in list)
        {
            if (item.second.sound)
                item.second.sound->drop();

            if (item.second.source)
                item.second.source->drop();
        }
    }

    void SoundClip::LoadContent()
    {
        for each(auto item in list)
        {
            item.second.source = SoundMgr->CreateSound(item.first);

            item.second.source->setDefaultVolume(item.second.volume);
            item.second.source->setDefaultMinDistance(item.second.minDist);
            item.second.source->setDefaultMaxDistance(item.second.maxDist);
        }
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
    }

    void SoundClip::RemoveClip(std::string clip)
    {
        auto item = list.find(clip);
        if (item == list.end())
            return;

        if (item->second.sound != nullptr)
            item->second.sound->drop();

        if (item->second.source != nullptr)
            item->second.source->drop();

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

    void SoundClip::PlaySound(std::string clip)
    {
        auto item = list.find(clip);
        if (item == list.end())
            return;

        Vec3 pos = GET_TRANSFORM_3D(owner)->GetPosition();

        item->second.sound = SoundMgr->GetEngine()->play3D(
            item->second.source, 
            vec3df(pos.x, pos.y, pos.z), 
            item->second.loop, 
            item->second.startPaused, 
            false, false
        );
    }
    
    void SoundClip::StopSound(std::string clip)
    {
        auto item = list.find(clip);
        
        if (item == list.end())
            return;

        item->second.sound->stop();
    }
}