#pragma once
#include "..\BONE_GRAPHICS\InterfaceIds.h"
#include "..\BONE_GRAPHICS\Script.h"
#include "..\RuntimeObjectSystem\ObjectInterfacePerModule.h"
#include "..\RuntimeObjectSystem\IObject.h"

RUNTIME_COMPILER_LINKLIBRARY("..\\Debug\\BONE_GRAPHICS.lib");
RUNTIME_COMPILER_LINKLIBRARY("..\\Debug\\BONE_SYSTEM.lib");
RUNTIME_COMPILER_LINKLIBRARY("..\\Library\\Lib\\x86\\reactphysics3d_debug.lib");
RUNTIME_COMPILER_LINKLIBRARY("..\\Library\\Lib\\x86\\RuntimeCompiler_Debug.lib");
RUNTIME_COMPILER_LINKLIBRARY("..\\Library\\Lib\\x86\\RuntimeObjectSystem_Debug.lib");

RUNTIME_MODIFIABLE_INCLUDE;
RUNTIME_COMPILER_SOURCEDEPENDENCY;

using namespace BONE_GRAPHICS;

class TestScript2 : public Script, public TInterface<IID_IUPDATEABLE, IObject> {
public:
    virtual void Init();
    virtual void Update();
};
