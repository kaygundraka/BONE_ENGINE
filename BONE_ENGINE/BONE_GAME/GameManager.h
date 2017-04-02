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
    GameObject* sceneCamera_1_1;
    GameObject* sceneCamera_1_2;

    Camera* FSCameraComponent_1_1;
    Camera* FSCameraComponent_1_2;
    
    GameObject* player;
    ScreenImage* screenImage;


    int totalSceneFlow;

    int sceneFlow[5];

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
    void ShowSubtitle(std::string str);
};
