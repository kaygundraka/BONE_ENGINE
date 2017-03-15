#pragma once
#include "Common.h"
#include "etuForm.h"
#include <queue>

namespace BONE_GRAPHICS
{
	struct etuUIMessage
	{
		etuForm* Form;
		int Type;

		etuUIMessage(etuForm* _Form, int _Message)
		{
			Form = _Form;
			Type = _Message;
		}
	};

	enum { MS_CREATE, MS_LBDOWN, MS_LBUP };

	template <typename T>
	class etuMessageHandler
	{
	private:
		queue<etuUIMessage> MessageQueue;
		void(T::*MsgProc) (etuUIMessage* etuUIMessage);
		T* etuFrameWork;

	public:
		etuMessageHandler() {}

		void SetMessageProc(T* _FrameWork, void(T::*p_MsgProc) (etuUIMessage* etuUIMessage))
		{
			etuFrameWork = _FrameWork;
			MsgProc = _MsgProc;
		}

		void ProcessMessage()
		{
			for (int i = 0; i < MessageQueue.size(); i++)
			{
				(etuFrameWork->*MsgProc)(&MessageQueue.front());
				MessageQueue.pop();
			}
		}

		void AddMessage(etuUIMessage Message)
		{
			MessageQueue.push(Message);
		}
	};

	void etuEnableDrag(etuUIMessage*);
}