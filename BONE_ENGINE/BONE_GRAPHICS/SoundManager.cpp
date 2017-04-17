#include "Common.h"
#include "SoundManager.h"
#include "ResourceManager.h"

namespace BONE_GRAPHICS
{
    void SoundManager::InitializeMembers()
    {
        engine = createIrrKlangDevice();
    }
    
    void SoundManager::ReleaseMembers()
    {
        RemoveAllSound();

        if (engine != nullptr)
            engine->drop();
    }
    
    void SoundManager::StopAllSound()
    {
        ThreadSync sync;

        engine->stopAllSounds();
    }

    ISoundEngine* SoundManager::GetEngine()
    {
        return engine;
    }

    void SoundManager::RemoveSound(ISoundSource* source)
    {
        ThreadSync sync;

        engine->removeSoundSource(source);
    }

    void SoundManager::Play2D(std::string file, float volume, bool loop)
    {
        ThreadSync sync;

        std::string FullPath;

        if (bgms.find(file) == bgms.end())
        {
            if (!ResourceMgr->ExistFile(file, &FullPath))
                return;

            ISound* sound = engine->play2D(FullPath.c_str(), loop, false, true);
            sound->setVolume(volume);

            bgms.insert(std::pair<std::string, ISound*>(file, sound));
        }
        else
        {
            if (bgms[file] != nullptr)
                bgms[file]->drop();
            
            if (!ResourceMgr->ExistFile(file, &FullPath))
                return;

            bgms[file] = engine->play2D(FullPath.c_str(), loop, false, true);
            bgms[file]->setVolume(volume);
            
            bgms[file]->setIsLooped(loop);
            bgms[file]->setVolume(volume);
        }
    }

    void SoundManager::Stop2D(std::string file)
    {
        if (bgms.find(file) == bgms.end())
            return;

        if (bgms[file] != nullptr)
            bgms[file]->stop();
    }

    bool SoundManager::IsPlaying2D(std::string file)
    {
        if (bgms.find(file) == bgms.end())
            return false;

        if (bgms[file] == nullptr)
            return false;

        if (bgms[file]->isFinished())
            return false;

        return true;
    }

    void SoundManager::RemoveAllSound()
    {
        ThreadSync sync;

        for(auto iter = bgms.begin(); iter != bgms.end(); )
        {
            if ((*iter).second != nullptr)
                (*iter).second->drop();

            iter = bgms.erase(iter);
        }
        
        engine->removeAllSoundSources();
    }

    ISoundSource* SoundManager::CreateSound(std::string name)
    {
        return engine->addSoundSourceFromFile(name.c_str());
    }

    void SoundManager::ListenerUpdate(Vec3 pos, Vec3 lookDir, Vec3 upVector)
    {
        ThreadSync sync;

        irrklang::vec3df position(pos.x, pos.y, pos.z);
        irrklang::vec3df lookDirection(lookDir.x, lookDir.y, lookDir.z);
        irrklang::vec3df velPerSecond(0, 0, 0);
        irrklang::vec3df up(upVector.x, upVector.y, upVector.z);
        
        engine->setListenerPosition(position, lookDirection, velPerSecond, up);
    }
}