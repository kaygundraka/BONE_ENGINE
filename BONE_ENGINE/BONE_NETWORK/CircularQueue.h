#pragma once
#include <MultiThreadSync.h>
#include "Common.h"

namespace BONE_NETWORK
{
	typedef struct _QUEUE_DATA
	{
		VOID	*Object;
		BYTE	Data[MAX_BUFFER_LENGTH];
		DWORD	DataLength;

		CHAR	RemoteAddress[32];
		USHORT	RemotePort;

		DWORD	Protocol;
	} QUEUE_DATA;

	class CCircularQueue : public BONE_SYSTEM::CMultiThreadSync<CCircularQueue>
	{
	public:
		CCircularQueue(VOID);
		~CCircularQueue(VOID);

	private:
		QUEUE_DATA	queue[MAX_QUEUE_LENGTH];
		DWORD		queueHead;
		DWORD		queueTail;

	public:
		BOOL		Begin(VOID);
		BOOL		End(VOID);

		BYTE*		Push(VOID *_object, BYTE *_data, DWORD _dataLength);
		BYTE*		Push(VOID *_object, BYTE *_data, DWORD _dataLength, LPCSTR _remoteAddress, USHORT _remotePort);
		BYTE*		Push(VOID *_object, DWORD _protocol, BYTE *_data, DWORD _dataLength);
		BYTE*		Push(VOID *_object, DWORD _protocol, BYTE *_data, DWORD _dataLength, LPCSTR _remoteAddress, USHORT _remotePort);

		BOOL		Pop(VOID **_object, BYTE *_data, DWORD &_dataLength);
		BOOL		Pop(VOID **_object, BYTE *_data, DWORD &_dataLength, LPSTR _remoteAddress, USHORT &_remotePort);
		BOOL		Pop(VOID **_object, DWORD &_protocol, BYTE *_data, DWORD &_dataLength);
		BOOL		Pop(VOID **_object, DWORD &_protocol, BYTE *_data, DWORD &_dataLength, LPSTR _remoteAddress, USHORT &_remotePort);
		BOOL		Pop(VOID);

		int			GetHeadIndex();
		int			GetTailIndex();

		BOOL		IsEmpty(VOID);
	};
}