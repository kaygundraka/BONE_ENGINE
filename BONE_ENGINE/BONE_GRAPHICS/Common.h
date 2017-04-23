#pragma once
// TODO: 프로그램에 필요한 추가 헤더는 여기에서 참조합니다.
#include <reactphysics3d.h>
#include <d3d9.h>
#include <d3dx9.h>
#include <dxfile.h>
#include <windows.h>
#include <stdio.h>
#include <iostream>
#include <fstream>
#include <map>
#include <list>
#include <string>
#include <dsound.h>
#include <mmsystem.h>
#include <stdarg.h>
#include <assert.h>
#include <vector>
#include <CommCtrl.h>
#include <random>
#include <tchar.h>
#include <mutex>
#include <thread>
#include <ctime>
#include <tuple>
#include <shlwapi.h>

#include "Strsafe.h"
#include "Interface.h"
#include <tinystr.h>
#include <tinyxml.h>
#include <MultiThreadSync.h>

// RuntimeCompiled C++
#include <ICompilerLogger.h>
#include <IObject.h>
#include <IObjectFactorySystem.h>
#include <AUArray.h>
#include <BuildTool.h>
#include <FileChangeNotifier.h>
#include <RuntimeObjectSystem.h>
#include <ObjectInterfacePerModule.h>

// ImGUIs
#include "imgui.h"
#include "imgui_impl_dx9.h"

// IrrKlang
#include <irrKlang.h>

// Json
#include <json.hpp>
using json = nlohmann::json;

#pragma comment(lib, "shlwapi")
#pragma comment(lib, "comctl32.lib")
#pragma comment(lib, "dsound")
#pragma comment(lib, "winmm")
#pragma warning (disable:4251)
#pragma warning (disable:4503)
#pragma warning (disable:4244)

using namespace std;

#define Vec3 D3DXVECTOR3
#define Vec2 D3DXVECTOR2
#define Matrix D3DXMATRIX
#define Quater D3DXQUATERNION
#define Color D3DCOLOR
#define Mesh LPD3DXMESH

class Rect {
public:
	Vec2 LeftTop;
	Vec2 RightBottom;
};

namespace COLOR {
	const D3DXCOLOR      WHITE(D3DCOLOR_XRGB(255, 255, 255));
	const D3DXCOLOR      BLACK(D3DCOLOR_XRGB(0, 0, 0));
	const D3DXCOLOR        RED(D3DCOLOR_XRGB(255, 0, 0));
	const D3DXCOLOR      GREEN(D3DCOLOR_XRGB(0, 255, 0));
	const D3DXCOLOR       BLUE(D3DCOLOR_XRGB(0, 0, 255));
	const D3DXCOLOR     YELLOW(D3DCOLOR_XRGB(255, 255, 0));
	const D3DXCOLOR       CYAN(D3DCOLOR_XRGB(0, 255, 255));
	const D3DXCOLOR    MAGENTA(D3DCOLOR_XRGB(255, 0, 255));
}

typedef struct _RGBA {
    float r;
    float g;
    float b;
    float a;

    _RGBA() : r(1), g(1), b(1), a(1) {}
} RGBA;

#define RuntimeMgr RuntimeCompiler::GetInstance()
#define SceneMgr SceneManager::GetInstance()
#define RenderMgr RenderManager::GetInstance()
#define InputMgr InputManager::GetInstance()
#define ResourceMgr ResourceManager::GetInstance()
#define ConfigMgr ConfigManager::GetInstance()
#define PhysicsMgr PhysicsManager::GetInstance()
#define LogMgr LogManager::GetInstance()
#define SoundMgr SoundManager::GetInstance()

using namespace reactphysics3d;

#define GET_TRANSFORM_3D(Object) ((Transform3D*)Object->transform3D)
#define GET_COLLISION(Object)    ((BONE_GRAPHICS::Collision*)Object->GetComponent("Collision"))
#define GET_RIGIDBODY(Object)    ((BONE_GRAPHICS::RigidBody*)Object->GetComponent("RigidBody"))
#define GET_SKINNED_MESH(Object) ((SkinnedMesh*)Object->GetComponent("SkinnedMesh"))
#define GET_STATIC_MESH(Object)  ((StaticMesh*)Object->GetComponent("StaticMesh"))
#define GET_BILLBOARD(Object)    ((BillBoard*)Object->GetComponent("BillBoard"))
#define CUR_SCENE SceneMgr->CurrentScene()

//#define EMManager EnvironmentManager::GetInstance()
//#define SDManager SoundManager::GetInstance()
//#define PSManager PhysicsManager::GetInstance()
//#define R2DManager UIManager::GetInstance()