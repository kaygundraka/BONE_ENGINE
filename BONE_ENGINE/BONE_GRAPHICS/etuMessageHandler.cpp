#include "Common.h"
#include "etuMessageHandler.h"
#include "InputManager.h"

namespace BONE_GRAPHICS
{
	void etuEnableDrag(etuUIMessage* msg)
	{
		POINT MousePosition;
		static POINT DragDistance;
		D3DXVECTOR3 CurrentPosition;
		static bool IsDrag = false;

		switch (msg->Type)
		{
		case MS_LBDOWN:
			MousePosition = InputMgr->GetMousePosition();

			CurrentPosition = msg->Form->GetPosition();

			if (!IsDrag)
			{
				DragDistance.x = CurrentPosition.x - MousePosition.x;
				DragDistance.y = CurrentPosition.y - MousePosition.y;

				if (DragDistance.x < 0)
					DragDistance.x = -DragDistance.x;

				if (DragDistance.y < 0)
					DragDistance.y = -DragDistance.y;
			}

			IsDrag = true;

			MousePosition.x -= DragDistance.x;
			MousePosition.y -= DragDistance.y;
			msg->Form->SetPosition(D3DXVECTOR3(MousePosition.x, MousePosition.y, CurrentPosition.z));
			break;

		case MS_LBUP:
			IsDrag = false;
			break;
		}
	}
}