#pragma once
// TODO: 프로그램에 필요한 추가 헤더는 여기에서 참조합니다.

#include <d3d9.h>
#include <d3dx9.h>
#include <dxfile.h>
#include <windows.h>
#include <stdio.h>
#include <iostream>
#include <fstream>
#include <math.h>
#include <map>
#include <list>
#include <string>
#include <dsound.h>
#include <mmsystem.h>
#include <vector>
#include <CommCtrl.h>
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
#include <json.hpp>

#pragma comment(lib, "shlwapi")
#pragma comment(lib, "comctl32.lib")
#pragma comment(lib, "dsound")
#pragma comment(lib, "winmm")
#pragma warning (disable:4251)

using namespace std;
using json = nlohmann::json;

#define Vector3 D3DXVECTOR3
#define Vector2 D3DXVECTOR2
#define Matrix D3DXMATRIX
#define Quaternion D3DXQUATERNION
#define Color D3DCOLOR
#define Mesh LPD3DXMESH

class Rect {
public:
	Vector2 LeftTop;
	Vector2 RightBottom;
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

#define SceneMgr SceneManager::GetInstance()
#define RenderMgr RenderManager::GetInstance()
#define InputMgr InputManager::GetInstance()
#define ResourceMgr ResourceManager::GetInstance()
#define ConfigMgr ConfigManager::GetInstance()
//#define EMManager EnvironmentManager::GetInstance()
//#define SDManager SoundManager::GetInstance()
//#define PSManager PhysicsManager::GetInstance()
//#define R2DManager UIManager::GetInstance()
#define LogMgr LogManager::GetInstance()