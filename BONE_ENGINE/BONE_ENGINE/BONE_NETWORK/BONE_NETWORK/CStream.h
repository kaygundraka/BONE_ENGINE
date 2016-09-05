#pragma once
#include "Common.h"
#include "CMemoryPool.h"

namespace BONE_NETWORK
{
	class CStream : public CMemoryPool<CStream>
	{
	public:
		CStream(VOID);
		~CStream(VOID);

	private:
		BYTE *bufferPointer;
		DWORD length;

	public:
		BOOL SetBuffer(BYTE *_buffer);
		DWORD GetLength(VOID);

		BOOL ReadInt32(INT *_data);
		BOOL ReadDWORD(DWORD *_data);
		BOOL ReadDWORD_PTR(DWORD_PTR *_data);
		BOOL ReadByte(BYTE *_data);
		BOOL ReadBytes(BYTE *_data, DWORD _length);
		BOOL ReadFloat(FLOAT *_data);
		BOOL ReadINT64(INT64 *_data);
		BOOL ReadUSHORT(USHORT *_data);
		BOOL ReadSHORT(SHORT *_data);
		BOOL ReadBOOL(BOOL *_data);
		BOOL ReadWCHAR(WCHAR *data);
		BOOL ReadWCHARs(LPWSTR data, DWORD length);

		BOOL WriteInt32(INT _data);
		BOOL WriteDWORD(DWORD _data);
		BOOL WriteDWORD_PTR(DWORD_PTR _data);
		BOOL WriteByte(BYTE _data);
		BOOL WriteBytes(BYTE *_data, DWORD _length);
		BOOL WriteFloat(FLOAT _data);
		BOOL WriteINT64(INT64 data);
		BOOL WriteUSHORT(USHORT _data);
		BOOL WriteSHORT(SHORT _data);
		BOOL WriteBOOL(BOOL _data);
		BOOL WriteWCHAR(WCHAR data);
		BOOL WriteWCHARs(LPCWSTR data, DWORD length);
	};

	class CStreamSP {
	private:
		CStream *stream;

	public:
		CStreamSP(VOID) { stream = new CStream(); }
		~CStreamSP(VOID) { delete stream; }

		CStream* operator->(VOID) { return stream; }
		operator CStream*(VOID) { return stream; }
	};
}