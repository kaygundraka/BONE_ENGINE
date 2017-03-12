#include "Common.h"
#include "InputManager.h"
#include "LogManager.h"

namespace BONE_GRAPHICS
{
	void InputManager::InitializeMembers()
	{
		ThreadSync sync;

		mousePosition.x = 400;
		mousePosition.y = 300;
		IsLBClick = MOUSE_NONE;
		IsRBClick = MOUSE_NONE;
		focusedInputText = 0;
		IsSelectObject = false;
		currentCharNumber = 0;
		focusedWindow = true;

		for (int i = 0; i < 127; i++)
			keyStatus[(char)i] = true;

		LogMgr->ShowMessage(LOG_MESSAGE, "InputManager is initialized");
	}
    
	void InputManager::SelectObject(std::string name)
	{
		ThreadSync sync;

		IsSelectObject = true;
		selectObjectName = name;
	}

	void InputManager::SelectNature(std::string name)
	{
		ThreadSync sync;

		selectNatureName = name;
	}

	std::string InputManager::GetSelectedObjectName()
	{
		ThreadSync sync;

		return selectObjectName;
	}

	void InputManager::SetSelectObjectValue(bool isExist)
	{
		ThreadSync sync;

		IsSelectObject = isExist;
	}

	bool InputManager::ExistSelectObejct()
	{
		ThreadSync sync;
	
		return IsSelectObject;
	}

	char InputManager::GetCurrentChar()
	{
		ThreadSync sync;

		if (currentCharNumber >= 1)
			currentCharNumber--;
		else
			return 0;

		return currentInputChar;
	}

	bool InputManager::IsFocusedWindow()
	{
		ThreadSync sync;

		return focusedWindow;
	}

	void InputManager::SetFocusWindow(bool isFocused)
	{
		ThreadSync sync;

		focusedWindow = isFocused;
	}

	void InputManager::SetCurrentChar(char _char)
	{
		ThreadSync sync;

		if (focusedInputText >= 1)
		{
			currentInputChar = _char;
			currentCharNumber++;
		}
	}

	void InputManager::SetFocusTextInput(int addNumber)
	{
		ThreadSync sync;

		focusedInputText += addNumber;
	}

	void InputManager::NotFocusTextInput()
	{
		ThreadSync sync;

		focusedInputText = 0;
	}

	bool InputManager::KeyDown(char key, bool onlyOne)
	{
		ThreadSync sync;

		if (!focusedInputText && focusedWindow)
		{
			if (onlyOne)
			{
				if ((GetAsyncKeyState(key) & 0x8000) && keyStatus[key] == true)
				{
					keyStatus[key] = false;
					return true;
				}
			}
			else if ((GetAsyncKeyState(key) & 0x8000))
				return true;
		}
		return false;
	}

	std::string InputManager::GetSelectedNatureName()
	{
		return selectNatureName;
	}

	MOUSE_WHEEL_STATUS InputManager::GetMouseWheelStatus()
	{
		return mouseStatus;
	}

	void InputManager::SetMouseWheelStatus(MOUSE_WHEEL_STATUS mouseWheelStatus)
	{
		ThreadSync sync;

		mouseStatus = mouseWheelStatus;
	}

	bool InputManager::GetCheckBoxStatus(int checkBoxName)
	{
		return checkBoxStatus[checkBoxName];
	}

	void InputManager::SetCheckBoxStatus(int checkBoxName, bool isChecked)
	{
		ThreadSync sync;

		checkBoxStatus[checkBoxName] = isChecked;
	}

	void InputManager::KeyUp(char key)
	{
		ThreadSync sync;

		keyStatus[key] = true;
	}

	POINT InputManager::GetMousePosition()
	{
		return mousePosition;
	}

	Vector2 InputManager::GetMousePositionToVector2()
	{
		return Vector2(mousePosition.x, mousePosition.y);
	}

	MOUSE_STATUS InputManager::GetMouseLBButtonStatus()
	{
		return IsLBClick;
	}

	int InputManager::GetCreatorMode()
	{
		return creatorMode;
	}

	void InputManager::SetCreatorMode(int index)
	{
		ThreadSync sync;

		creatorMode = index;
	}

	void InputManager::SetMouseLBButtonStatus(MOUSE_STATUS isClicked)
	{
		ThreadSync sync;

		IsLBClick = isClicked;
	}

	MOUSE_STATUS InputManager::GetMouseRBButtonStatus()
	{
		ThreadSync sync;

		return IsRBClick;
	}

	void InputManager::SetMouseRBButtonStatus(MOUSE_STATUS isClicked)
	{
		ThreadSync sync;

		IsRBClick = isClicked;
	}

	void InputManager::SetMousePosition(POINT mousePosition)
	{
		ThreadSync sync;

		mousePosition = mousePosition;
	}

	void InputManager::ReleaseMembers()
	{
		ThreadSync sync;

		LogMgr->ShowMessage(LOG_MESSAGE, "inputManager is clear.");
	}
}