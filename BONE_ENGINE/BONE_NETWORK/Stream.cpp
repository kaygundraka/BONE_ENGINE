#include "Common.h"
#include "Stream.h"

namespace BONE_NETWORK
{
	CStream::CStream(VOID)
	{
		bufferPointer = NULL;
		length = 0;
	}

	CStream::~CStream(VOID)
	{

	}

	BOOL CStream::SetBuffer(BYTE *_buffer)
	{
		if (_buffer == NULL)
			return FALSE;

		bufferPointer = _buffer;
		length = 0;

		return TRUE;
	}

	DWORD CStream::GetLength(VOID)
	{
		return length;
	}

	//////////////////////////////////////////////////////////////////////////////
	//// Read Function
	//////////////////////////////////////////////////////////////////////////////
	BOOL CStream::ReadInt32(INT *_data)
	{
		CopyMemory(_data, bufferPointer + length, sizeof(INT));

		length += sizeof(INT);

		return TRUE;
	}
	
	BOOL CStream::ReadDWORD(DWORD *_data)
	{
		CopyMemory(_data, bufferPointer + length, sizeof(DWORD));

		length += sizeof(DWORD);

		return TRUE;
	}

	BOOL CStream::ReadDWORD_PTR(DWORD_PTR *_data)
	{
		CopyMemory(_data, bufferPointer + length, sizeof(DWORD_PTR));

		length += sizeof(DWORD_PTR);

		return TRUE;
	}

	BOOL CStream::ReadByte(BYTE *_data)
	{
		CopyMemory(_data, bufferPointer + length, sizeof(BYTE));

		length += sizeof(BYTE);

		return TRUE;
	}

	BOOL CStream::ReadBytes(BYTE *_data, DWORD _length)
	{
		CopyMemory(_data, bufferPointer + length, _length );

		length += _length;

		return TRUE;
	}

	BOOL CStream::ReadFLOAT(FLOAT *_data)
	{
		CopyMemory(_data, bufferPointer + length, sizeof(FLOAT));

		length += sizeof(FLOAT);

		return TRUE;
	}

	BOOL CStream::ReadINT64(INT64 *_data)
	{
		CopyMemory(_data, bufferPointer + length, sizeof(INT64));

		length += sizeof(INT64);

		return TRUE;
	}

	BOOL CStream::ReadUSHORT(USHORT *_data)
	{
		CopyMemory(_data, bufferPointer + length, sizeof(USHORT));

		length += sizeof(USHORT);

		return TRUE;
	}

	BOOL CStream::ReadSHORT(SHORT *_data)
	{
		CopyMemory(_data, bufferPointer + length, sizeof(SHORT));

		length += sizeof(SHORT);

		return TRUE;
	}

	BOOL CStream::ReadBOOL(BOOL *_data)
	{
		CopyMemory(_data, bufferPointer + length, sizeof(BOOL));

		length += sizeof(BOOL);

		return TRUE;
	}

	BOOL CStream::ReadWCHAR(WCHAR *_data)
	{
		memcpy(_data, bufferPointer + length, sizeof(WCHAR));

		length += sizeof(WCHAR);

		return TRUE;
	}

	BOOL CStream::ReadWCHARs(LPWSTR _data, DWORD _length)
	{
		memcpy(_data, bufferPointer + length, _length * sizeof(WCHAR));

		length += _length * sizeof(WCHAR);

		return TRUE;
	}
	
	//////////////////////////////////////////////////////////////////////////////
	//// Write Function
	//////////////////////////////////////////////////////////////////////////////
	BOOL CStream::WriteInt32(INT _data)
	{
		CopyMemory(bufferPointer + length, &_data, sizeof(INT));

		length += sizeof(INT);

		return TRUE;
	}

	BOOL CStream::WriteDWORD(DWORD _data)
	{
		CopyMemory(bufferPointer + length, &_data, sizeof(DWORD));

		length += sizeof(DWORD);

		return TRUE;
	}

	BOOL CStream::WriteDWORD_PTR(DWORD_PTR _data)
	{
		CopyMemory(bufferPointer + length, &_data, sizeof(DWORD_PTR));

		length += sizeof(DWORD_PTR);

		return TRUE;
	}

	BOOL CStream::WriteByte(BYTE _data)
	{
		CopyMemory(bufferPointer + length, &_data, sizeof(BYTE));

		length += sizeof(BYTE);

		return TRUE;
	}

	BOOL CStream::WriteBytes(BYTE* _data, DWORD _length)
	{
		CopyMemory(bufferPointer + length, &_data, _length);

		length += _length;

		return TRUE;
	}

	BOOL CStream::WriteFLOAT(FLOAT _data)
	{
		CopyMemory(bufferPointer + length, &_data, sizeof(FLOAT));

		length += sizeof(FLOAT);

		return TRUE;
	}

	BOOL CStream::WriteINT64(INT64 _data)
	{
		CopyMemory(bufferPointer + length, &_data, sizeof(INT64));

		length += sizeof(INT64);

		return TRUE;
	}

	BOOL CStream::WriteUSHORT(USHORT _data)
	{
		CopyMemory(bufferPointer + length, &_data, sizeof(USHORT));

		length += sizeof(USHORT);

		return TRUE;
	}

	BOOL CStream::WriteSHORT(SHORT _data)
	{
		CopyMemory(bufferPointer + length, &_data, sizeof(SHORT));

		length += sizeof(SHORT);

		return TRUE;
	}

	BOOL CStream::WriteBOOL(BOOL _data)
	{
		CopyMemory(bufferPointer + length, &_data, sizeof(BOOL));

		length += sizeof(BOOL);

		return TRUE;
	}

	BOOL CStream::WriteWCHAR(WCHAR _data)
	{
		memcpy(bufferPointer + length, &_data, sizeof(WCHAR));

		length += sizeof(WCHAR);

		return TRUE;
	}

	BOOL CStream::WriteWCHARs(LPCWSTR _data, DWORD _length)
	{
		memcpy(bufferPointer + length, _data, _length * sizeof(WCHAR));

		length += _length * sizeof(WCHAR);

		return TRUE;
	}
}