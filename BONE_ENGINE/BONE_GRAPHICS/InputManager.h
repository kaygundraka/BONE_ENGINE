#pragma once
#include "ISingleton.h"
#include <MultiThreadSync.h>
#include "Common.h"

namespace BONE_GRAPHICS
{
	//enum { MODE_LOCATE_OBJECT, MODE_EDIT_TERRAIN };
	//enum { CHECKBOX_OBJECT_REPLACE, CHECKBOX_OBJECT_AUTOLOCATEINTERRAIN };
	enum MOUSE_WHEEL_STATUS { MOUSE_WHEEL_UP, MOUSE_WHEEL_DOWN, MOUSE_WHEEL_NONE };
	enum MOUSE_STATUS { MOUSE_NONE, MOUSE_LBDOWN, MOUSE_LBUP, MOUSE_RBDOWN, MOUSE_RBUP};

	class InputManager : public ISingleton<InputManager>, public BONE_SYSTEM::CMultiThreadSync<InputManager>
	{
	private:
		POINT mousePosition;
		MOUSE_STATUS IsLBClick;
		MOUSE_STATUS IsRBClick;
		string selectObjectName;
		string selectNatureName;
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
		Vector2 GetMousePositionToVector2();
		MOUSE_STATUS GetMouseLBButtonStatus();
		MOUSE_STATUS GetMouseRBButtonStatus();
		bool GetCheckBoxStatus(int _checkBoxName);
		string GetSelectedObjectName();
		string GetSelectedNatureName();
		int GetCreatorMode();
		MOUSE_WHEEL_STATUS GetMouseWheelStatus();
		char GetCurrentChar();
		bool IsFocusedWindow();

		void SetFocusTextInput(int _addNumber);
		void SetFocusWindow(bool _isFocused);
		void SetCurrentChar(char _char);
		void SetMouseLBButtonStatus(MOUSE_STATUS _isClicked);
		void SetMouseRBButtonStatus(MOUSE_STATUS _isClicked);
		void SetMouseWheelStatus(MOUSE_WHEEL_STATUS _mouseWheelStatus);
		void NotFocusTextInput();
		void SetSelectObjectValue(bool _isExist);
		void SetMousePosition(POINT _mousePosition);
		void SetCheckBoxStatus(int _checkBoxName, bool _isChecked);
		void SelectObject(string _name);
		void SelectNature(string _name);
		void SetCreatorMode(int _index);
		bool ExistSelectObejct();

		virtual void ReleaseMembers();
	};
}