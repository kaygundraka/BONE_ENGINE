#include "Common.h"
#include "Crypt.h"

namespace BONE_NETWORK
{
	const INT C1 = 52845;
	const INT C2 = 22719;
	const INT KEY = 72957;

	BOOL CCrypt::Encrypt(BYTE* _source, BYTE* _destination, DWORD _length)
	{
		DWORD i;
		INT Key = KEY;

		if (!_source || !_destination || _length <= 0)
			return FALSE;

		for (i = 0; i < _length; i++)
		{
			_destination[i] = _source[i] ^ Key >> 8;
			Key = (_destination[i] + Key) * C1 + C2;
		}

		return TRUE;
	}

	BOOL CCrypt::Decrypt(BYTE* _source, BYTE* _destination, DWORD _length)
	{
		DWORD i;
		BYTE PreviousBlock;
		INT Key = KEY;

		if (!_source || !_destination || _length <= 0)
			return FALSE;

		for (i = 0; i < _length; i++)
		{
			PreviousBlock = _source[i];
			_destination[i] = _source[i] ^ Key >> 8;
			Key = (PreviousBlock + Key) * C1 + C2;
		}

		return TRUE;
	}
}