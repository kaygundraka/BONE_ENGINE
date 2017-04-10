#include "Common.h"
#include "SoundManager.h"

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

    void SoundManager::Play2D(std::string file, float Volume)
    {

    }

    void SoundManager::RemoveAllSound()
    {
        ThreadSync sync;

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