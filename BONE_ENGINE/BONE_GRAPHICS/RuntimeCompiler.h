#pragma once
#include "Common.h"
#include "ISingleton.h"
#include <IObjectFactorySystem.h>
using namespace BONE_SYSTEM;

namespace BONE_GRAPHICS
{
    class RuntimeCompiler : public ISingleton<RuntimeCompiler>, public MultiThreadSync<RuntimeCompiler>, public IObjectFactoryListener
    {
    public:
        RuntimeCompiler();
        virtual ~RuntimeCompiler();

        bool InitializeMembers();
        virtual void ReleaseMembers();

        bool AddSource(std::string name);
        bool Compile();

        IObject* GetSource(std::string name);

        virtual void OnConstructorsAdded();

    private:
        // Runtime Systems
        ICompilerLogger* compilerLogger;
        IRuntimeObjectSystem* runtimeObjectSystem;

        typedef struct _OBJECT {
            IObject* object;
            ObjectId id;
        } OBJECT;

        // Runtime object
        std::map<std::string, OBJECT> objectList;
    };
}