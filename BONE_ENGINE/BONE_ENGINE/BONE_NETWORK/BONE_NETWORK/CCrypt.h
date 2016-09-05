#pragma once
#include "Common.h"

namespace BONE_NETWORK
{
	class CCrypt {
	public:
		static BOOL Encrypt(BYTE* _source, BYTE* _destination, DWORD _length);
		static BOOL Decrypt(BYTE* _source, BYTE* _destination, DWORD _length);
	};
}