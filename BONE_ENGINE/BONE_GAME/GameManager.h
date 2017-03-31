#pragma once
#include <Script.h>
#include <GameObject.h>
#include <Transform3D.h>
#include <Transform2D.h>
#include <ScreenImage.h>
#include <Camera.h>
using namespace BONE_GRAPHICS;

class GameManager : public Script {
private:
    GameObject* firstSceneCamera;
    GameObject* player;
    ScreenImage* screenImage;

    Camera* FSCameraComponent;

    int totalSceneFlow;
    int firstSceneFlow;

    int subtitleSize;

    float screenAlpha;
    
public:
    virtual void Init();
    virtual void Reference();
    virtual void Update();
    virtual void LateUpdate();
    virtual void LateRender();

    void FadeIn();
    void FadeOut();
};
