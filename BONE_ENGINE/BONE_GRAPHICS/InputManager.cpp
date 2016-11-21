#include "Common.h"
#include "InputManager.h"
#include "LogManager.h"

namespace BONE_GRAPHICS
{
	//InputManager* InputManager::pInst = NULL;

	void InputManager::InitializeMembers()
	{
		CThreadSync sync;

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

	void InputManager::SelectObject(string _name)
	{
		CThreadSync sync;

		IsSelectObject = true;
		selectObjectName = _name;
	}

	void InputManager::SelectNature(string _name)
	{
		CThreadSync sync;

		selectNatureName = _name;
	}

	string InputManager::GetSelectedObjectName()
	{
		CThreadSync sync;

		return selectObjectName;
	}

	void InputManager::SetSelectObjectValue(bool _isExist)
	{
		CThreadSync sync;

		IsSelectObject = _isExist;
	}

	bool InputManager::ExistSelectObejct()
	{
		CThreadSync sync;
	
		return IsSelectObject;
	}

	char InputManager::GetCurrentChar()
	{
		CThreadSync sync;

		if (currentCharNumber >= 1)
			currentCharNumber--;
		else
			return 0;

		return currentInputChar;
	}

	bool InputManager::IsFocusedWindow()
	{
		CThreadSync sync;

		return focusedWindow;
	}

	void InputManager::SetFocusWindow(bool _isFocused)
	{
		CThreadSync sync;

		focusedWindow = _isFocused;
	}

	void InputManager::SetCurrentChar(char _char)
	{
		CThreadSync sync;

		if (focusedInputText >= 1)
		{
			currentInputChar = _char;
			currentCharNumber++;
		}
	}

	void InputManager::SetFocusTextInput(int _addNumber)
	{
		CThreadSync sync;

		focusedInputText += _addNumber;
	}

	void InputManager::NotFocusTextInput()
	{
		CThreadSync sync;

		focusedInputText = 0;
	}

	bool InputManager::KeyDown(char _key, bool _onlyOne)
	{
		CThreadSync sync;

		if (!focusedInputText && focusedWindow)
		{
			if (_onlyOne)
			{
				if ((GetAsyncKeyState(_key) & 0x8000) && keyStatus[_key] == true)
				{
					keyStatus[_key] = false;
					return true;
				}
			}
			else if ((GetAsyncKeyState(_key) & 0x8000))
			{
				return true;
			}
		}
		return false;
	}

	string InputManager::GetSelectedNatureName()
	{
		CThreadSync sync;

		return selectNatureName;
	}

	MOUSE_WHEEL_STATUS InputManager::GetMouseWheelStatus()
	{
		CThreadSync sync;

		return mouseStatus;
	}

	void InputManager::SetMouseWheelStatus(MOUSE_WHEEL_STATUS _mouseWheelStatus)
	{
		CThreadSync sync;

		mouseStatus = _mouseWheelStatus;
	}

	bool InputManager::GetCheckBoxStatus(int _checkBoxName)
	{
		CThreadSync sync;

		return checkBoxStatus[_checkBoxName];
	}

	void InputManager::SetCheckBoxStatus(int _checkBoxName, bool _isChecked)
	{
		CThreadSync sync;

		checkBoxStatus[_checkBoxName] = _isChecked;
	}

	void InputManager::KeyUp(char _key)
	{
		CThreadSync sync;

		keyStatus[_key] = true;
	}

	POINT InputManager::GetMousePosition()
	{
		CThreadSync sync;

		return mousePosition;
	}

	Vector2 InputManager::GetMousePositionToVector2()
	{
		CThreadSync sync;

		return Vector2(mousePosition.x, mousePosition.y);
	}

	MOUSE_STATUS InputManager::GetMouseLBButtonStatus()
	{
		CThreadSync sync;

		return IsLBClick;
	}

	int InputManager::GetCreatorMode()
	{
		CThreadSync sync;

		return creatorMode;
	}

	void InputManager::SetCreatorMode(int _index)
	{
		CThreadSync sync;

		creatorMode = _index;
	}

	void InputManager::SetMouseLBButtonStatus(MOUSE_STATUS _isClicked)
	{
		CThreadSync sync;

		IsLBClick = _isClicked;
	}

	MOUSE_STATUS InputManager::GetMouseRBButtonStatus()
	{
		CThreadSync sync;

		return IsRBClick;
	}

	void InputManager::SetMouseRBButtonStatus(MOUSE_STATUS _isClicked)
	{
		CThreadSync sync;

		IsRBClick = _isClicked;
	}

	void InputManager::SetMousePosition(POINT _mousePosition)
	{
		CThreadSync sync;

		mousePosition = _mousePosition;
	}

	void InputManager::ReleaseMembers()
	{
		CThreadSync sync;

		LogMgr->ShowMessage(LOG_MESSAGE, "inputManager is clear.");
	}
}