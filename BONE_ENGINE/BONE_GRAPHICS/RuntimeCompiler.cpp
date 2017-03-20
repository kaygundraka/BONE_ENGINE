#include "Common.h"
#include "RuntimeCompiler.h"
#include "RuntimeCompilerLogger.h"
#include "SceneManager.h"
#include "InputManager.h"
#include <Windows.h>
#include "Script.h"

#include <iostream>
#ifdef WIN32
#include <conio.h>
#include <tchar.h>
#else
#include <unistd.h>
int _getche()
{
    int ret = getchar();
    return ret;
}
int _kbhit()
{
    std::cout << "This port needs a fix, CTRL-C to quit\n";
    return 0;
}

int Sleep(int msecs)
{
    return usleep(msecs * 1000);
}
#endif
#include <sstream>
#include <vector>
#include <algorithm>
#include <string>
#include <stdarg.h>

#if defined _WINDOWS_ && defined GetObject
#undef GetObject
#endif
using FileSystemUtils::Path;

namespace BONE_GRAPHICS
{
    RuntimeCompiler::RuntimeCompiler() : compilerLogger(0), runtimeObjectSystem(0)
    {

    }

    RuntimeCompiler::~RuntimeCompiler()
    {
    }

    bool RuntimeCompiler::InitializeMembers()
    {
        ThreadSync sync;

        runtimeObjectSystem = new RuntimeObjectSystem;
        compilerLogger = new RuntimeCompileLogger;

        if (!runtimeObjectSystem->Initialise(compilerLogger, 0))
        {
            runtimeObjectSystem = 0;
            return false;
        }

        runtimeObjectSystem->GetObjectFactorySystem()->AddListener(this);
        
        runtimeObjectSystem->AddIncludeDir("..\\BONE_SYSTEM");
        runtimeObjectSystem->AddIncludeDir("..\\BONE_GRAPHICS");
        runtimeObjectSystem->AddIncludeDir("C:\\Program Files (x86)\\Microsoft DirectX SDK (June 2010)\\Include");
        
        return true;
    }
    
    void RuntimeCompiler::ReleaseMembers()
    {
        ThreadSync sync;

        if (runtimeObjectSystem)
            runtimeObjectSystem->CleanObjectFiles();

        if (runtimeObjectSystem && runtimeObjectSystem->GetObjectFactorySystem())
        {
            runtimeObjectSystem->GetObjectFactorySystem()->RemoveListener(this);

            // delete object via correct interface

            for each (auto var in objectList)
            {
                IObject* pObj = runtimeObjectSystem->GetObjectFactorySystem()->GetObjectA(var.second.id);
                delete pObj;
            }
        }

        delete runtimeObjectSystem;
        delete compilerLogger;
    }

    bool RuntimeCompiler::AddSource(std::string name)
    {
        ThreadSync sync;

        // construct first object
        IObjectConstructor* pCtor = runtimeObjectSystem->GetObjectFactorySystem()->GetConstructor(name.c_str());
        
        OBJECT Object;

        if (pCtor)
        {
            IObject* Obj = pCtor->Construct();
            Obj->GetInterface(&Object.object);

            if (!Obj)
            {
                delete Obj;
                compilerLogger->LogError("no IObject interface found");
                return false;
            }

            Object.id = Obj->GetObjectId();
        }
        else
            return false;

        objectList[name] = Object;

        return true;
    }

    IObject* RuntimeCompiler::GetSource(std::string name)
    {
        return objectList[name].object;
    }

    bool RuntimeCompiler::Compile()
    {
        ThreadSync sync;

        if (InputMgr->KeyDown('O', true))
            runtimeObjectSystem->CompileAll(true);
        if (InputMgr->KeyDown('P', true))
            runtimeObjectSystem->GetObjectFactorySystem()->UndoObjectConstructorChange();

        // check status of any compile
        if (runtimeObjectSystem->GetIsCompiledComplete())
        {
            // load module when compile complete
            runtimeObjectSystem->LoadCompiledModule();
            
            if (runtimeObjectSystem->GetLastLoadModuleSuccess())
                compilerLogger->LogInfo("Runtime Compile Complete");
            else 
                compilerLogger->LogInfo("Runtime Compile Failed");
        
            
        }

        if (!runtimeObjectSystem->GetIsCompiling())
        {        
            runtimeObjectSystem->GetFileChangeNotifier()->Update(SceneMgr->GetTimeDelta());
        }
    
        return true;
    }

    void RuntimeCompiler::OnConstructorsAdded()
    {
        ThreadSync sync;

        // This could have resulted in a change of object pointer, so release old and get new one.
        for (auto var = objectList.begin(); var != objectList.end();)
        {
            if (var->second.object)
            {
                IObject* Obj = runtimeObjectSystem->GetObjectFactorySystem()->GetObjectA(var->second.id);
                Obj->GetInterface(&(var->second.object));
                
                if (!(var->second.object))
                {
                    delete Obj;
                    compilerLogger->LogError("no IObject interface found");

                    var = objectList.erase(var);
                }
                else
                    var++;                
            }
            else
                var++;
        }
    }
}