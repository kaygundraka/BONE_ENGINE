#include "Common.h"
#include "RenderManager.h"
#include "LogManager.h"
#include "ResourceManager.h"
#include "SceneManager.h"
#include "InputManager.h"
#include "InputTextBox.h"
#include "GameObject.h"

namespace BONE_GRAPHICS
{
	InputTextBox::InputTextBox()
	{
		SetTypeName("InputTextBox");

		isClicked = false;
		isFocused = false;
		active = true;

		maxStrLength = 255;
	}

	InputTextBox::~InputTextBox()
	{
	}

	void InputTextBox::LoadContent()
	{
	}

	bool InputTextBox::IsClicked(GameObject* owner)
	{
		Transform2D* tr = ((Transform2D*)owner->GetComponent("Transform2D"));
		
		if (tr == nullptr)
			return false;

		Vec2 position(
			tr->GetPosition().x,
			tr->GetPosition().y
		);

		int width = tr->GetSize().x;
		int height = tr->GetSize().y;

		if (InputMgr->GetMouseLBButtonStatus() == MOUSE_LBDOWN)
		{
			POINT temp = InputMgr->GetMousePosition();

			if (temp.x <= position.x + width && temp.x >= position.x)
			{
				if (temp.y <= position.y + height && temp.y >= position.y)
				{
					if (!isClicked)
						InputMgr->SetFocusTextInput(1);

					isClicked = true;
					isFocused = true;

					return true;
				}
			}

			if (isFocused)
				InputMgr->SetFocusTextInput(-1);

			isFocused = false;
		}
		else
			isClicked = false;
	}

	void InputTextBox::SetMaxLength(int length)
	{
		maxStrLength = length;
	}
	
	int InputTextBox::GetMaxLength()
	{
		return maxStrLength;
	}

	std::string InputTextBox::GetString()
	{
		return str;
	}

	void InputTextBox::SetActive(bool active)
	{
		this->active = active;
	}

	void InputTextBox::ClearString()
	{
		str = "";
	}

	void InputTextBox::Update(GameObject* owner)
	{
		if (active)
		{
			IsClicked(owner);

			if (isFocused)
			{
				static float KeyDown = 0;

				if ((GetAsyncKeyState(VK_BACK) & 0x8000) && KeyDown >= 0.1f)
				{
					char Text[100];
					strcpy_s(Text, str.c_str());

					if (strlen(Text) >= 1)
						Text[strlen(Text) - 1] = 0;

					str = Text;
					KeyDown = 0;
				}
				else
				{
					if (maxStrLength >= str.size() + 1)
					{
						char Temp[2];
						Temp[0] = InputMgr->GetCurrentChar();
						Temp[1] = '\0';

						if (Temp[0] >= 32 && Temp[0] <= 126)
						{
							char Text[100];
							strcpy_s(Text, str.c_str());
							strcat_s(Text, Temp);

							str = Text;
						}
					}
				}

				if (KeyDown <= 0.1f)
					KeyDown += SceneMgr->GetTimeDelta();
			}
		}
	}
}