#pragma once
#include "Common.h"
#include "ISingleton.h"
using namespace BONE_SYSTEM;

namespace BONE_GRAPHICS
{
    class RuntimeCompiler : public ISingleton< RuntimeCompiler >, public MultiThreadSync< RuntimeCompiler > 
    {
    public:
        RuntimeCompiler();
        virtual ~RuntimeCompiler();

        void AddSource();

        bool Compile();

        void InitializeMembers();
        virtual void ReleaseMembers();
    };
}