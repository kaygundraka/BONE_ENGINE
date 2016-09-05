#pragma once
#include "Common.h"

namespace BONE_NETWORK
{
	class CBoneClient{
	private:

	public:
		bool Initialize(int _mode, TCHAR* _ipAddress, int _port);
	};
}