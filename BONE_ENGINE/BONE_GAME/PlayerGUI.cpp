#include "PlayerGUI.h"
#include <InputManager.h>
#include <ScreenImage.h>

void PlayerGUI::Init()
{
    gameObject->SetPriority(1);
    gameObject->SetTag("PlayerGUI");

    status = NORMAL;
}

void PlayerGUI::Reference()
{
    statusNormal_Active     = new GameObject();
    statusNormal_Inactive   = new GameObject();
    statusNormal_Inactive->SetActive(false);

    statusSneaking_Active = new GameObject();
    statusSneaking_Inactive = new GameObject();
    statusSneaking_Active->SetActive(false);

    statusCombat_Active     = new GameObject();
    statusCombat_Inactive   = new GameObject();
    statusCombat_Active->SetActive(false);
    statusCombat_Inactive->SetActive(false);

    Transform2D* tr[6];
    
    for (int i = 0; i < 6; i++)
        tr[i] = new Transform2D();

    tr[0]->SetPosition(RenderMgr->GetWidth() - 340, 10, 0);
    tr[1]->SetPosition(RenderMgr->GetWidth() - 310, 10, 0);

    tr[2]->SetPosition(RenderMgr->GetWidth() - 340, 45, 0);
    tr[3]->SetPosition(RenderMgr->GetWidth() - 310, 45, 0);

    tr[4]->SetPosition(RenderMgr->GetWidth() - 340, 80, 0);
    tr[5]->SetPosition(RenderMgr->GetWidth() - 310, 80, 0);

    statusNormal_Active->AddComponent(tr[0]);
    statusNormal_Inactive->AddComponent(tr[1]);
    statusSneaking_Active->AddComponent(tr[2]);
    statusSneaking_Inactive->AddComponent(tr[3]);
    statusCombat_Active->AddComponent(tr[4]);
    statusCombat_Inactive->AddComponent(tr[5]);

    Rect originRect;
    originRect.LeftTop = Vec2(0, 0);
    originRect.RightBottom = Vec2(303, 33);

    ScreenImage* ImageNormal_Active = new ScreenImage();
    ImageNormal_Active->SetImageFile("Normal_Active.png");
    ImageNormal_Active->SetOriginRect(originRect);
    statusNormal_Active->AddComponent(ImageNormal_Active);

    ScreenImage* ImageNormal_Inactive = new ScreenImage();
    ImageNormal_Inactive->SetImageFile("Normal_Inactive.png");
    ImageNormal_Inactive->SetOriginRect(originRect);
    statusNormal_Inactive->AddComponent(ImageNormal_Inactive);

    ScreenImage* ImageSneaking_Active = new ScreenImage();
    ImageSneaking_Active->SetImageFile("Sneaking_Active.png");
    ImageSneaking_Active->SetOriginRect(originRect);
    statusSneaking_Active->AddComponent(ImageSneaking_Active);

    ScreenImage* ImageSneaking_Inactive = new ScreenImage();
    ImageSneaking_Inactive->SetImageFile("Sneaking_Inactive.png");
    ImageSneaking_Inactive->SetOriginRect(originRect);
    statusSneaking_Inactive->AddComponent(ImageSneaking_Inactive);

    ScreenImage* ImageCombat_Active = new ScreenImage();
    ImageCombat_Active->SetImageFile("Combat_Active.png");
    ImageCombat_Active->SetOriginRect(originRect);
    statusCombat_Active->AddComponent(ImageCombat_Active);

    ScreenImage* ImageCombat_Inactive = new ScreenImage();
    ImageCombat_Inactive->SetImageFile("Combat_Inactive.png");
    ImageCombat_Inactive->SetOriginRect(originRect);
    statusCombat_Inactive->AddComponent(ImageCombat_Inactive);

    CUR_SCENE->AddObject(statusNormal_Active, "GUI_STATUS_NORMAL_A");
    CUR_SCENE->AddObject(statusNormal_Inactive, "GUI_STATUS_NORMAL_I");
    CUR_SCENE->AddObject(statusSneaking_Active, "GUI_STATUS_SNEAKING_A");
    CUR_SCENE->AddObject(statusSneaking_Inactive, "GUI_STATUS_SNEAKING_I");
    CUR_SCENE->AddObject(statusCombat_Active, "GUI_STATUS_COMBAT_A");
    CUR_SCENE->AddObject(statusCombat_Inactive, "GUI_STATUS_COMBAT_I");
    
    statusFrame = new GameObject();

    Transform2D* StatusFrameTr = new Transform2D();
    StatusFrameTr->SetPosition(0, 10, 0);
    statusFrame->AddComponent(StatusFrameTr);
    
    ScreenImage* ImageStatusFrame = new ScreenImage();
    ImageStatusFrame->SetImageFile("StatusFrame.png");
    ImageStatusFrame->SetOriginRect(Vec2(0, 0), Vec2(315, 42));
    statusFrame->AddComponent(ImageStatusFrame);

    CUR_SCENE->AddObject(statusFrame, "GUI_STATUS_FRAME");


    hpBar = new GameObject();
    hpBar->SetPriority(1);

    hpBarTr = new Transform2D();
    hpBarTr->SetPosition(StatusFrameTr->GetPosition() + Vec3(8, 25, 1));
    hpBar->AddComponent(hpBarTr);

    ScreenImage* ImageHpBar = new ScreenImage();
    ImageHpBar->SetImageFile("HP_Bar.png");
    ImageHpBar->SetOriginRect(Vec2(0, 0), Vec2(193, 2));
    hpBar->AddComponent(ImageHpBar);

    CUR_SCENE->AddObject(hpBar, "GUI_HP_BAR");

    steminaBar = new GameObject();
    steminaBar->SetPriority(1);

    steminaBarTr = new Transform2D();
    steminaBarTr->SetPosition(StatusFrameTr->GetPosition() + Vec3(8, 30, 1));
    steminaBar->AddComponent(steminaBarTr);

    ScreenImage* ImageSteminaBar = new ScreenImage();
    ImageSteminaBar->SetImageFile("Status_Bar.png");
    ImageSteminaBar->SetOriginRect(Vec2(0, 0), Vec2(193, 2));
    steminaBar->AddComponent(ImageSteminaBar);

    CUR_SCENE->AddObject(steminaBar, "GUI_STEMINA_BAR");


    getItemGUI = new GameObject();

    Transform2D* showGetItemTr = new Transform2D();
    showGetItemTr->SetPosition(0, RenderMgr->GetHeight() - 100, 0);
    getItemGUI->AddComponent(showGetItemTr);

    ScreenImage* ImageGetItems = new ScreenImage();
    ImageGetItems->SetImageFile("GetItems.png");
    ImageGetItems->SetOriginRect(Vec2(0, 0), Vec2(392, 35));
    getItemGUI->AddComponent(ImageGetItems);

    CUR_SCENE->AddObject(getItemGUI, "GUI_SHOW_ITEM");
    getItemGUI->SetActive(false);

    ShowGUI(false);
}

void PlayerGUI::ShowGetItem(bool show)
{
    getItemGUI->SetActive(show);
}

void PlayerGUI::ShowGUI(bool show)
{
    if (show == false)
    {
        statusNormal_Active->SetActive(false);
        statusNormal_Inactive->SetActive(false);
        statusCombat_Active->SetActive(false);
        statusCombat_Inactive->SetActive(false);
        statusSneaking_Active->SetActive(false);
        statusSneaking_Inactive->SetActive(false);
 
        statusFrame->SetActive(false);
        hpBar->SetActive(false);
        steminaBar->SetActive(false);

    }
    else
    {
        if (status == PLAYER_STATUS::NORMAL)
        {
            statusNormal_Active->SetActive(true);
            statusNormal_Inactive->SetActive(false);
        }
        else
        {
            statusNormal_Active->SetActive(false);
            statusNormal_Inactive->SetActive(true);
        }

        if (status == PLAYER_STATUS::COMBAT)
        {
            statusCombat_Active->SetActive(true);
            statusCombat_Inactive->SetActive(false);
        }
        else
        {
            statusCombat_Active->SetActive(false);
            statusCombat_Inactive->SetActive(true);
        }

        if (status == PLAYER_STATUS::SNEAKING)
        {
            statusSneaking_Active->SetActive(true);
            statusSneaking_Inactive->SetActive(false);
        }
        else
        {
            statusSneaking_Active->SetActive(false);
            statusSneaking_Inactive->SetActive(true);
        }

        statusFrame->SetActive(true);
        hpBar->SetActive(true);
        steminaBar->SetActive(true);
    }
}

void PlayerGUI::SetStatus(PLAYER_STATUS status)
{
    this->status = status;

    if (status == PLAYER_STATUS::NORMAL)
    {
        statusNormal_Active->SetActive(true);
        statusNormal_Inactive->SetActive(false);
    }
    else
    {
        statusNormal_Active->SetActive(false);
        statusNormal_Inactive->SetActive(true);
    }

    if (status == PLAYER_STATUS::COMBAT)
    {
        statusCombat_Active->SetActive(true);
        statusCombat_Inactive->SetActive(false);
    }
    else
    {
        statusCombat_Active->SetActive(false);
        statusCombat_Inactive->SetActive(true);
    }

    if (status == PLAYER_STATUS::SNEAKING)
    {
        statusSneaking_Active->SetActive(true);
        statusSneaking_Inactive->SetActive(false);
    }
    else
    {
        statusSneaking_Active->SetActive(false);
        statusSneaking_Inactive->SetActive(true);
    }
}

PlayerGUI::PLAYER_STATUS PlayerGUI::GetStatus()
{
    return status;
}

void PlayerGUI::SetStemina(int stemina)
{
    if (stemina == 0)
        GET_TRANSFORM_2D(steminaBar)->SetScale(0.01f, 1);
    else
        GET_TRANSFORM_2D(steminaBar)->SetScale((float)stemina / 100.0f, 1);
}

void PlayerGUI::SetHP(int hp)
{
    if (hp == 0)
        GET_TRANSFORM_2D(hpBar)->SetScale(0.01f, 1);
    else
        GET_TRANSFORM_2D(hpBar)->SetScale((float)hp / 100.0f, 1);

}