#pragma once
#include <InterfaceIds.h>
#include <Script.h>
#include <SceneManager.h>
#include <Transform2D.h>

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

    GameObject* getItemGUI;

    GameObject* hpBar;
    Transform2D* hpBarTr;

    GameObject* steminaBar;
    Transform2D* steminaBarTr;

    int hp;
    int stemina;
    bool showGetItem;
        
public:
    void ShowGUI(bool show);
        
    virtual void Init();
    virtual void Reference();

    void SetStatus(PLAYER_STATUS status);
    
    PLAYER_STATUS GetStatus();

    void ShowGetItem(bool show);

    void SetStemina(int stemina);
    void SetHP(int hp);
};
