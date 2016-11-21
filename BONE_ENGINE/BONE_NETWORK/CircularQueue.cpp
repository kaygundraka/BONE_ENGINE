#include "Common.h"
#include "CircularQueue.h"

namespace BONE_NETWORK
{
	CCircularQueue::CCircularQueue(VOID)
	{
		memset(queue, 0, sizeof(queue));

		queueHead = 0;
		queueTail = 0;
	}

	CCircularQueue::~CCircularQueue(VOID)
	{
	}

	BOOL CCircularQueue::Begin(VOID)
	{
		memset(queue, 0, sizeof(queue));

		queueHead = 0;
		queueTail = 0;

		return TRUE;
	}

	BOOL CCircularQueue::End(VOID)
	{
		return TRUE;
	}

	BYTE* CCircularQueue::Push(VOID *object, BYTE *data, DWORD dataLength)
	{
		CThreadSync Sync;

		//CLog::WriteLog(_T("Queue Status  : Head (%d) Tail (%d)\n"), queueHead, queueTail);

		if (!object || !data)
			return NULL;

		DWORD TempTail = (queueTail + 1) % MAX_QUEUE_LENGTH;

		if (TempTail == queueHead)
			return NULL;

		queue[TempTail].Object = object;
		queue[TempTail].DataLength = dataLength;

		memcpy(queue[TempTail].Data, data, dataLength);

		queueTail = TempTail;

		return queue[TempTail].Data;
	}

	BYTE* CCircularQueue::Push(VOID *object, DWORD protocol, BYTE *data, DWORD dataLength)
	{
		CThreadSync Sync;

		//CLog::WriteLog(_T("Queue Status  : Head (%d) Tail (%d)\n"), queueHead, queueTail);

		if (!object || !data)
			return NULL;

		DWORD TempTail = (queueTail + 1) % MAX_QUEUE_LENGTH;

		if (TempTail == queueHead)
			return NULL;

		queue[TempTail].Object = object;
		queue[TempTail].DataLength = dataLength;
		queue[TempTail].Protocol = protocol;

		memset(queue[TempTail].Data, 0, sizeof(queue[TempTail].Data));
		memcpy(queue[TempTail].Data, data, dataLength);

		queueTail = TempTail;

		return queue[TempTail].Data;
	}

	BYTE *CCircularQueue::Push(VOID *object, BYTE *data, DWORD dataLength, LPCSTR remoteAddress, USHORT remotePort)
	{
		CThreadSync Sync;

		//CLog::WriteLog(_T("Queue Status  : Head (%d) Tail (%d)\n"), queueHead, queueTail);

		if (!object || !data)
			return NULL;

		DWORD TempTail = (queueTail + 1) % MAX_QUEUE_LENGTH;

		if (TempTail == queueHead)
			return NULL;

		queue[TempTail].Object = object;
		queue[TempTail].DataLength = dataLength;
		queue[TempTail].RemotePort = remotePort;

		memset(queue[TempTail].RemoteAddress, 0, sizeof(queue[TempTail].RemoteAddress));
		strncpy(queue[TempTail].RemoteAddress, remoteAddress, sizeof(queue[TempTail].RemoteAddress));

		memset(queue[TempTail].Data, 0, sizeof(queue[TempTail].Data));
		memcpy(queue[TempTail].Data, data, dataLength);

		queueTail = TempTail;

		return queue[TempTail].Data;
	}

	BYTE *CCircularQueue::Push(VOID *object, DWORD protocol, BYTE *data, DWORD dataLength, LPCSTR remoteAddress, USHORT remotePort)
	{
		CThreadSync Sync;

		//CLog::WriteLog(_T("Queue Status  : Head (%d) Tail (%d)\n"), queueHead, queueTail);

		if (!object || !data)
			return NULL;

		DWORD TempTail = (queueTail + 1) % MAX_QUEUE_LENGTH;

		if (TempTail == queueHead)
			return NULL;

		queue[TempTail].Object = object;
		queue[TempTail].DataLength = dataLength;
		queue[TempTail].RemotePort = remotePort;
		queue[TempTail].Protocol = protocol;

		memset(queue[TempTail].RemoteAddress, 0, sizeof(queue[TempTail].RemoteAddress));
		strncpy(queue[TempTail].RemoteAddress, remoteAddress, sizeof(queue[TempTail].RemoteAddress));

		memset(queue[TempTail].Data, 0, sizeof(queue[TempTail].Data));
		memcpy(queue[TempTail].Data, data, dataLength);

		queueTail = TempTail;

		return queue[TempTail].Data;
	}

	BOOL CCircularQueue::Pop(VOID)
	{
		CThreadSync Sync;

		//CLog::WriteLog(_T("Queue Status  : Head (%d) Tail (%d)\n"), queueHead, queueTail);

		if (queueHead == queueTail)
			return FALSE;

		DWORD TempHead = (queueHead + 1) % MAX_QUEUE_LENGTH;

		queueHead = TempHead;

		return TRUE;
	}

	BOOL CCircularQueue::Pop(VOID **object, BYTE *data, DWORD &dataLength)
	{
		CThreadSync Sync;

		//CLog::WriteLog(_T("Queue Status  : Head (%d) Tail (%d)\n"), queueHead, queueTail);

		if (!object || !data)
			return FALSE;

		if (queueHead == queueTail)
			return FALSE;

		DWORD TempHead = (queueHead + 1) % MAX_QUEUE_LENGTH;

		dataLength = queue[TempHead].DataLength;
		*object = queue[TempHead].Object;

		memcpy(data, queue[TempHead].Data, queue[TempHead].DataLength);

		queueHead = TempHead;

		return TRUE;
	}

	BOOL CCircularQueue::Pop(VOID **object, DWORD &protocol, BYTE *data, DWORD &dataLength)
	{
		CThreadSync Sync;

		//CLog::WriteLog(_T("Queue Status  : Head (%d) Tail (%d)\n"), queueHead, queueTail);

		if (!object || !data)
			return FALSE;

		if (queueHead == queueTail)
			return FALSE;

		DWORD TempHead = (queueHead + 1) % MAX_QUEUE_LENGTH;

		dataLength = queue[TempHead].DataLength;
		*object = queue[TempHead].Object;
		protocol = queue[TempHead].Protocol;

		memcpy(data, queue[TempHead].Data, queue[TempHead].DataLength);

		queueHead = TempHead;

		return TRUE;
	}

	BOOL CCircularQueue::Pop(VOID **object, BYTE *data, DWORD &dataLength, LPSTR remoteAddress, USHORT &remotePort)
	{
		CThreadSync Sync;

		//CLog::WriteLog(_T("Queue Status  : Head (%d) Tail (%d)\n"), queueHead, queueTail);

		if (!object || !data)
			return FALSE;

		if (queueHead == queueTail)
			return FALSE;

		DWORD TempHead = (queueHead + 1) % MAX_QUEUE_LENGTH;

		dataLength = queue[TempHead].DataLength;
		*object = queue[TempHead].Object;
		remotePort = queue[TempHead].RemotePort;

		strncpy(remoteAddress, queue[TempHead].RemoteAddress, sizeof(queue[TempHead].RemoteAddress));
		memcpy(data, queue[TempHead].Data, queue[TempHead].DataLength);

		queueHead = TempHead;

		return TRUE;
	}

	BOOL CCircularQueue::Pop(VOID **object, DWORD &protocol, BYTE *data, DWORD &dataLength, LPSTR remoteAddress, USHORT &remotePort)
	{
		CThreadSync Sync;

		//CLog::WriteLog(_T("Queue Status  : Head (%d) Tail (%d)\n"), queueHead, queueTail);

		if (!object || !data)
			return FALSE;

		if (queueHead == queueTail)
			return FALSE;

		DWORD TempHead = (queueHead + 1) % MAX_QUEUE_LENGTH;

		dataLength = queue[TempHead].DataLength;
		*object = queue[TempHead].Object;
		remotePort = queue[TempHead].RemotePort;
		protocol = queue[TempHead].Protocol;

		strncpy(remoteAddress, queue[TempHead].RemoteAddress, sizeof(queue[TempHead].RemoteAddress));
		memcpy(data, queue[TempHead].Data, queue[TempHead].DataLength);

		queueHead = TempHead;

		return TRUE;
	}

	int	CCircularQueue::GetHeadIndex()
	{
		CThreadSync Sync;

		return queueHead;
	}
	
	int	CCircularQueue::GetTailIndex()
	{
		CThreadSync Sync;

		return queueTail;
	}

	BOOL CCircularQueue::IsEmpty(VOID)
	{
		CThreadSync Sync;

		if (queueHead == queueTail)
			return TRUE;

		return FALSE;
	}

}