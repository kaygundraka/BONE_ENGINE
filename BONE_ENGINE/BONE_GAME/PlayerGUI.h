#pragma once
#include <InterfaceIds.h>
#include <Script.h>
#include <SceneManager.h>

using namespace BONE_GRAPHICS;

class PlayerGUI : public Script {
public:
    enum PLAYER_STATUS {
        NORMAL, COMBAT, SNEAKING
    };

private:
    PLAYER_STATUS status;

    GameObject* statusNormal_Active;
    GameObject* statusNormal_Inactive;

    GameObject* statusCombat_Active;
    GameObject* statusCombat_Inactive;

    GameObject* statusSneaking_Active;
    GameObject* statusSneaking_Inactive;

    GameObject* statusFrame;

public:
    void ShowGUI(bool show);
        
    virtual void Init();
    virtual void Reference();
    virtual void Update();
    virtual void LateUpdate();
};
