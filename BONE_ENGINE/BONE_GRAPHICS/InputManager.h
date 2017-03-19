#pragma once
#include "ISingleton.h"
#include "InterfaceIds.h"
#include "Common.h"

namespace BONE_GRAPHICS
{
	//enum { MODE_LOCATE_OBJECT, MODE_EDIT_TERRAIN };
	//enum { CHECKBOX_OBJECT_REPLACE, CHECKBOX_OBJECT_AUTOLOCATEINTERRAIN };
	enum MOUSE_WHEEL_STATUS { MOUSE_WHEEL_UP, MOUSE_WHEEL_DOWN, MOUSE_WHEEL_NONE };
	enum MOUSE_STATUS { MOUSE_NONE, MOUSE_LBDOWN, MOUSE_LBUP, MOUSE_RBDOWN, MOUSE_RBUP};

	class InputManager : public ISingleton<InputManager>, public BONE_SYSTEM::MultiThreadSync<InputManager>
	{
	private:
		POINT mousePosition;
		MOUSE_STATUS IsLBClick;
		MOUSE_STATUS IsRBClick;
		std::string selectObjectName;
		std::string selectNatureName;
		bool IsSelectObject;
		map<char, bool> keyStatus;
		map<int, bool> checkBoxStatus;
		MOUSE_WHEEL_STATUS mouseStatus;
		int creatorMode;
		char currentInputChar;
		int currentCharNumber;
		int focusedInputText;
		bool focusedWindow;

	public:
		InputManager() {}
		virtual ~InputManager() {}

		void InitializeMembers();
		bool KeyDown(char _key, bool _onlyOne);
		void KeyUp(char _key);
		POINT GetMousePosition();
		Vec2 GetMousePositionToVector2();
		MOUSE_STATUS GetMouseLBButtonStatus();
		MOUSE_STATUS GetMouseRBButtonStatus();
		bool GetCheckBoxStatus(int _checkBoxName);
		std::string GetSelectedObjectName();
		std::string GetSelectedNatureName();
		int GetCreatorMode();
		MOUSE_WHEEL_STATUS GetMouseWheelStatus();
		char GetCurrentChar();
		bool IsFocusedWindow();

		void SetFocusTextInput(int addNumber);
		void SetFocusWindow(bool isFocused);
		void SetCurrentChar(char _char);
		void SetMouseLBButtonStatus(MOUSE_STATUS isClicked);
		void SetMouseRBButtonStatus(MOUSE_STATUS isClicked);
		void SetMouseWheelStatus(MOUSE_WHEEL_STATUS mouseWheelStatus);
		void NotFocusTextInput();
		void SetSelectObjectValue(bool isExist);
		void SetMousePosition(POINT mousePosition);
		void SetCheckBoxStatus(int checkBoxName, bool isChecked);
		void SelectObject(std::string name);
		void SelectNature(std::string name);
		void SetCreatorMode(int index);
		bool ExistSelectObejct();

		virtual void ReleaseMembers();
	};
}