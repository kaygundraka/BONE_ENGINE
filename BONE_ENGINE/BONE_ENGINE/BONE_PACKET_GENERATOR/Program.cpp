#include <tchar.h>
#include <Windows.h>
#include <stdio.h>
#pragma warning(disable:4996)


// �ִ� �� ���� ����
#define MAX_LINE_LENGTH			255

// �� �������� �� �ִ� ���� �� �ִ� �Ķ���� ����
#define MAX_PARAMETER_COUNT		255

// �ִ� �������� ����
#define MAX_PROTOCOL_COUNT		255

// ���������� ũ�� �������ݰ� �Ķ���ͷ� ���������ϴ�.
// �Ķ����
typedef struct _PARAMETER
{
	// �Ķ������
	TCHAR Type[MAX_LINE_LENGTH];

	// �Ķ���� ������
	TCHAR Name[MAX_LINE_LENGTH];

	// �Ķ���� ����
	DWORD Length;	// _BYTE�������� ���̰� �ֽ��ϴ�.

} PARAMETER;

// ��������
typedef struct _PROTOCOL
{
	// �������� �̸�
	TCHAR Name[MAX_LINE_LENGTH];

	// �Ķ���� ����
	DWORD ParameterCount;

	// �Ķ���� ���� : �迭�� ��� �ִ� �Ķ���͸� ���� �� �ִ� ������ 255�� �Դϴ�.
	PARAMETER Parameters[MAX_PARAMETER_COUNT];
} PROTOCOL;

int _tmain(int argc, _TCHAR* argv[])
{
	// ���� �̸� �Ķ���Ͱ� ���ٸ� �������� �ʽ��ϴ�.
	//if (argc == 1)
		//return 0;

#pragma region PARSER_FILE
	// �������� ������ ������ PROTOCOL ����ü �迭�� �����մϴ�.
	PROTOCOL *Protocol = new PROTOCOL[MAX_PROTOCOL_COUNT];
	// ���� ���������� ����� �������� ����ü�� ��ġ�Դϴ�.
	DWORD ProtocolPos = 0;

	// ������ structure ���� �̸�
	TCHAR StructureFileName[MAX_PATH] = { 0, };
	// ������ Read �Լ� ���� �̸�
	TCHAR ReadPacketFileName[MAX_PATH] = { 0, };
	// ������ Write �Լ� ���� �̸�
	TCHAR WritePacketFileName[MAX_PATH] = { 0, };

	// Structure ������ �ڿ� _Structure.h�� ���Դϴ�.
	// read ������ �ڿ� _ReadPacket.h�� ���Դϴ�.
	// write ������ �ڿ� _WritePacket.h�� ���Դϴ�.
	_sntprintf(StructureFileName, MAX_PATH, _T("%s_Structure.h"), _T("PT"));		//argv[2]
	_sntprintf(ReadPacketFileName, MAX_PATH, _T("%s_ReadPacket.h"), _T("PT"));	//argv[2]
	_sntprintf(WritePacketFileName, MAX_PATH, _T("%s_WritePacket.h"), _T("PT")); //argv[2]

	// �������� ������ ������ �޸𸮸� �ʱ�ȭ���ݴϴ�.
	ZeroMemory(Protocol, sizeof(PROTOCOL) * MAX_PROTOCOL_COUNT);

	// ���� ���� ���⼭�� Protocol.h�� ���ϴ�.
	FILE *SourceFile = _tfopen(_T("Protocol.h"), _T("r")); //argv[1]

	// �� ������ �о ������ �����Դϴ�.
	TCHAR Data[MAX_LINE_LENGTH] = { 0, };
	// ���� �Ķ���͸� �д� �������� Ȯ���ϴ� FLAG ���� �Դϴ�.
	// �Ķ���ʹ� �׻� /**/ �ȿ� ��� �ֱ� ������ üũ�� �� �ֽ��ϴ�.
	BOOL IsParameterOpen = FALSE;
	// ���� �� ��° �Ķ���������� Ȯ���ϴ� �����Դϴ�.
	DWORD ParameterPos = 0;
	// �� ���κ��� �о ���������� �޸𸮿� �����մϴ�.
	while (_fgetts(Data, MAX_LINE_LENGTH, SourceFile))
	{
		// ����, ��, ���� ���� ������ �����͸� �����ϴ� ����
		TCHAR TempData[MAX_LINE_LENGTH] = { 0, };
		// ���� �а� �ִ� ������ ��ġ
		DWORD TempDataPos = 0;
		for (DWORD i = 0; i < _tcslen(Data); i++)
		{
			// ���� �� ���� ���� ����, ��, ���� ���� ��� ���� �ݴϴ�.
			if (Data[i] != ' ' && Data[i] != '\t' && Data[i] != '\n' && Data[i] != '\r')
				// TempData ���ۿ� �����ؼ� �ֽ��ϴ�.
				TempData[TempDataPos++] = Data[i];
		}

		// �� ������ �����Ͱ� �������� �̸����� �Ķ�������� �������ݴϴ�.
		// �Ķ������ ��� ���� ���������� �̷������ �������� �̸��� ���󰩴ϴ�.
		// �Ķ���ʹ� �ִ� 255������ �� �� �ֽ��ϴ�.

		// ���� �ȿ� ,�� ������ ���������Դϴ�.
		// enum�� Ư¡�� ,�� Ȯ���մϴ�.
		// �� �κ� ������ �Ķ���Ϳ��� ,�� ������ �ȵ˴ϴ�.
		TCHAR *Pos = _tcsstr(TempData, _T(","));
		if (Pos)
		{
			// ���������� ��� VERSION�̶�� ���ڿ��� ������ ��� �����մϴ�.
			// �����ϰ� ���� ���� ���� �������� ��ġ�� �ϳ� �������Ѽ� �������� �̸��� �����մϴ�.
			if (!_tcsstr(TempData, _T("VERSION")))
				_tcsncpy(Protocol[ProtocolPos++].Name, TempData, Pos - TempData);
			else
			{

			}
		}
		// �������� �̸��� �ƴ� ���
		// �̰��� ������� �������̰ų� �Ķ���� ������ ���Դϴ�.
		else
		{
			// �����Ϳ� �Ķ���� ���� ��ȣ�� ���� ���
			if (_tcsstr(TempData, _T("/*")))
			{
				// �Ķ���� ���� FLAG�� TRUE�� ����ϴ�.
				IsParameterOpen = TRUE;

				// �Ķ���� ���� ������ �ʱ�ȭ�մϴ�.
				ParameterPos = 0;

				// ���� ������ �н��ϴ�.
				continue;
			}

			// �����Ϳ� �Ķ���� ���� ��ȣ�� ���� ���
			if (_tcsstr(TempData, _T("*/")))
			{
				// �Ķ���� ���� FLAG�� FALSE�� ����ϴ�.
				IsParameterOpen = FALSE;
				// �ش� ���������� �Ķ���� ������ �Է��մϴ�.
				Protocol[ProtocolPos - 1].ParameterCount = ParameterPos;
				// �Ķ���� ������ �ʱ�ȭ�մϴ�.
				ParameterPos = 0;
				// ���� ������ �н��ϴ�.
				continue;
			}

			// �Ķ���� ���� ��ȣ�� �ݱ� ��ȣ�� �ƴ� ���
			if (_tcslen(TempData) != 0)
			{
				// �Ķ���Ͱ� ���� ������ ��� (�� ���� �Ķ���� �����͸� �о���� ���Դϴ�.)
				if (IsParameterOpen)
				{
					// ���𰡰� ���� ���
					// �Ķ�����ε�, ��, �̸�, ����� �и����ݴϴ�.
					// �����ڴ� : �� [] �Դϴ�.
					Pos = _tcsstr(TempData, _T(":"));

					// ���ο� ��, ������ �����ڰ� ���� ���
					if (Pos)
					{
						// �Ķ���� ���� �����մϴ�.
						_tcsncpy(Protocol[ProtocolPos - 1].Parameters[ParameterPos++].Type, TempData, Pos - TempData);
					}
					// : ��ȣ�� ���� ��� ���� ������ �н��ϴ�.
					else
						continue;

					// :�� �ִ� ���ο� [ ��ȣ�� ���� ��� �� ��ġ�� ã���ϴ�.
					TCHAR *StartPos = _tcsstr(TempData, _T("["));
					// �� ��ġ�� 0�� �ƴ� ���
					if (StartPos)
					{
						// ] ��ȣ�� �ִ� ��ġ�� ã���ϴ�.
						TCHAR *EndPos = _tcsstr(TempData, _T("]"));
						TCHAR Length[MAX_LINE_LENGTH] = { 0, };

						// [���ۺ��� ]�������� ���̸� ���ؼ� ������ �Ķ���Ͱ� �迭�� ���
						// �� �迭 ũ�⸦ ���ؼ� Length�� �����մϴ�.
						_tcsncpy(Length, StartPos + 1, EndPos - StartPos - 1);
						// �������� ����ü�� �����մϴ�.
						Protocol[ProtocolPos - 1].Parameters[ParameterPos - 1].Length = _tstoi(Length);

						// �Ķ���� �̸��� �����մϴ�.
						_tcsncpy(Protocol[ProtocolPos - 1].Parameters[ParameterPos - 1].Name, Pos + 1, StartPos - Pos - 1);
					}
					else
					{
						// ���� [ ��ȣ�� ���� ���� �迭���� �ƴϱ� ������ ���̸� 0���� �����մϴ�.
						Protocol[ProtocolPos - 1].Parameters[ParameterPos - 1].Length = 0;

						// �׸��� �Ķ���� �̸��� �����մϴ�.
						_tcscpy(Protocol[ProtocolPos - 1].Parameters[ParameterPos - 1].Name, Pos + 1);
					}
				}
			}
		}
	}

	fclose(SourceFile);
	// ���� ������ ��ġ�� �Ǹ� Protocol.h ������ �о
	// �� ������ �������� �迭�� �Ķ���� ���� �����ؼ� �����ϰ� �˴ϴ�.
	// �Ʒ��� ���� ���ʹ� ����� �������� �迭���� Output ���ϵ��� ������ ���� ������ �����ݴϴ�.

#pragma endregion

#pragma region STRUCTURE_FILE_CREATE
	// structure �����
	FILE *StructureFile = _tfopen(StructureFileName, _T("w"));

	_ftprintf(StructureFile, _T("#pragma once\n\n"));

	// ���� �������� ��ü ������ŭ �ݺ��˴ϴ�.
	for (DWORD i = 0; i < ProtocolPos; i++)
	{
		// ���� �������ݰ� �Ķ���� ������ �̿��ؼ� ����ü�� �����մϴ�.
		_ftprintf(StructureFile, _T("typedef struct _S_%s\n{\n"), Protocol[i].Name);
		// ���� ���������� �Ķ���� ������ŭ �ݺ��մϴ�.
		for (DWORD j = 0; j < Protocol[i].ParameterCount; j++)
		{
			// �迭���� �ƴ� �Ķ������ ���
			if (Protocol[i].Parameters[j].Length == 0)
			{
				// ���������� �������� �� �ݴϴ�.
				_ftprintf(StructureFile, _T("\t%s %s;\n"), Protocol[i].Parameters[j].Type, Protocol[i].Parameters[j].Name);
			}
			else
			{
				// ��������, ������, �迭�� ũ�⸦ �� �ݴϴ�.
				_ftprintf(StructureFile, _T("\t%s %s[%d];\n"), Protocol[i].Parameters[j].Type, Protocol[i].Parameters[j].Name, Protocol[i].Parameters[j].Length);
			}
		}

		// ���������� ����ü�� �ݰ� �̸��� ���� �ݴϴ�.
		_ftprintf(StructureFile, _T("} S_%s;\n\n"), Protocol[i].Name);
	}
	
	fclose(StructureFile);
	// ���� �۾��� �ݺ��ϰ� �Ǹ� ��� ���������� �Ķ���͸� ������ ����ü�� �����˴ϴ�.

#pragma endregion

#pragma region READ_FILE_CREATE
	// Read �Լ� �����
	FILE *ReadPacketFile = _tfopen(ReadPacketFileName, _T("w"));

	_ftprintf(ReadPacketFile, _T("#pragma once\n\n"));

	for (DWORD i = 0; i < ProtocolPos; i++)
	{
		// READ �Լ� �����Դϴ�. inline ���·� ����˴ϴ�.
		// �� READ �Լ� ���� ������ �̸� ����� ���� ������ �̿��ؼ� �����͸� �аԵ˴ϴ�.
		_ftprintf(ReadPacketFile, _T("inline VOID READ_%s(BYTE *buffer, S_%s &parameter)\n{\n"), Protocol[i].Name, Protocol[i].Name);

		// ���̳ʸ����� �����͸� �б� ���ؼ� CStreamSP Ŭ������ �̿��մϴ�.
		_ftprintf(ReadPacketFile, _T("\tCStreamSP Stream;\n"));
		_ftprintf(ReadPacketFile, _T("\tStream->SetBuffer(buffer);\n\n"));

		// �ش� ���������� �Ķ���� ���� ��ŭ �ݺ��մϴ�.
		for (DWORD j = 0; j < Protocol[i].ParameterCount; j++)
		{
			// �Ķ������ ���� Ȯ��
			if (!_tcscmp(Protocol[i].Parameters[j].Type, _T("INT")))
			{
				// ReadInt32 �Լ��� �̿��ؼ� �����͸� ����ü�� �Է��մϴ�.
				_ftprintf(ReadPacketFile, _T("\tStream->ReadInt32(&parameter.%s);\n"), Protocol[i].Parameters[j].Name);
			}
			else if (!_tcscmp(Protocol[i].Parameters[j].Type, _T("DWORD")))
			{
				// ReadDWORD �Լ��� �̿��ؼ� �����͸� ����ü�� �Է��մϴ�.
				_ftprintf(ReadPacketFile, _T("\tStream->ReadDWORD(&parameter.%s);\n"), Protocol[i].Parameters[j].Name);
			}
			else if (!_tcscmp(Protocol[i].Parameters[j].Type, _T("DWORD_PTR")))
			{
				// ReadDWORD_PTR �Լ��� �̿��ؼ� �����͸� ����ü�� �Է��մϴ�.
				_ftprintf(ReadPacketFile, _T("\tStream->ReadDWORD_PTR(&parameter.%s);\n"), Protocol[i].Parameters[j].Name);
			}
			else if (!_tcscmp(Protocol[i].Parameters[j].Type, _T("FLOAT")))
			{
				// ReadFLOAT �Լ��� �̿��ؼ� �����͸� ����ü�� �Է��մϴ�.
				_ftprintf(ReadPacketFile, _T("\tStream->ReadFLOAT(&parameter.%s);\n"), Protocol[i].Parameters[j].Name);
			}
			else if (!_tcscmp(Protocol[i].Parameters[j].Type, _T("INT64")))
			{
				// ReadINT64 �Լ��� �̿��ؼ� �����͸� ����ü�� �Է��մϴ�.
				_ftprintf(ReadPacketFile, _T("\tStream->ReadINT64(&parameter.%s);\n"), Protocol[i].Parameters[j].Name);
			}
			else if (!_tcscmp(Protocol[i].Parameters[j].Type, _T("USHORT")))
			{
				// ReadUSHORT �Լ��� �̿��ؼ� �����͸� ����ü�� �Է��մϴ�.
				_ftprintf(ReadPacketFile, _T("\tStream->ReadUSHORT(&parameter.%s);\n"), Protocol[i].Parameters[j].Name);
			}
			else if (!_tcscmp(Protocol[i].Parameters[j].Type, _T("SHORT")))
			{
				// ReadSHORT �Լ��� �̿��ؼ� �����͸� ����ü�� �Է��մϴ�.
				_ftprintf(ReadPacketFile, _T("\tStream->ReadSHORT(&parameter.%s);\n"), Protocol[i].Parameters[j].Name);
			}
			else if (!_tcscmp(Protocol[i].Parameters[j].Type, _T("BOOL")))
			{
				// ReadBOOL �Լ��� �̿��ؼ� �����͸� ����ü�� �Է��մϴ�.
				_ftprintf(ReadPacketFile, _T("\tStream->ReadBOOL(&parameter.%s);\n"), Protocol[i].Parameters[j].Name);
			}
			else if (!_tcscmp(Protocol[i].Parameters[j].Type, _T("BYTE")))
			{
				// ���� �Ķ������ ���̰� 0�̸�
				if (Protocol[i].Parameters[j].Length == 0)
				{
					// ReadByte �Լ��� �̿��ؼ� �����͸� ����ü�� �Է��մϴ�.
					_ftprintf(ReadPacketFile, _T("\tStream->ReadByte(&parameter.%s);\n"), Protocol[i].Parameters[j].Name);
				}
				else
				{
					// ReadBytes �Լ��� �̿��ؼ� �����͸� ����ü�� �Է��մϴ�.
					_ftprintf(ReadPacketFile, _T("\tStream->ReadBytes(parameter.%s, %d);\n"), 
						Protocol[i].Parameters[j].Name,
						Protocol[i].Parameters[j].Length);
				}
			}
			else if (!_tcscmp(Protocol[i].Parameters[j].Type, _T("WCHAR")))
			{
				// ���� �Ķ������ ���̰� 0�̸�
				if (Protocol[i].Parameters[j].Length == 0)
				{
					// ReadWCHAR �Լ��� �̿��ؼ� �����͸� ����ü�� �Է��մϴ�.
					_ftprintf(ReadPacketFile, _T("\tStream->ReadWCHAR(&parameter.%s);\n"), Protocol[i].Parameters[j].Name);
				}
				else
				{
					// ReadWCHARs �Լ��� �̿��ؼ� �����͸� ����ü�� �Է��մϴ�.
					_ftprintf(ReadPacketFile, _T("\tStream->ReadWCHARs(parameter.%s, %d);\n"),
						Protocol[i].Parameters[j].Name,
						Protocol[i].Parameters[j].Length);
				}
			}
			else
			{
				// ReadBytes �Լ��� �̿��ؼ� �����͸� ����ü�� �Է��մϴ�.
				_ftprintf(ReadPacketFile, _T("\tStream->ReadBytes((BYTE*)parameter.%s, %d);\n"),
					Protocol[i].Parameters[j].Name,
					Protocol[i].Parameters[j].Length);
			}
		}
		_ftprintf(ReadPacketFile, _T("}\n\n"));
	}

	fclose(ReadPacketFile);
	// �� �κ��� �����ϸ� Read �Լ��� ������ ���� ������ �Ѱ� �����˴ϴ�.
#pragma endregion

#pragma region WRITE_FILE_CREATE
	// �������� ���캼 Write �Լ� ������ 2������ Write �Լ��� �����մϴ�.
	// ����ü�� �̿��� Write �Լ��� �Ķ���͸� �ϳ��� �Է��ϴ� �Ķ���� Ÿ���� �����մϴ�.
	// ����ü Ÿ���� Write �Լ� �����
	FILE *WritePacketFile = _tfopen(WritePacketFileName, _T("w"));

	_ftprintf(WritePacketFile, _T("#pragma once\n\n"));

	for (DWORD i = 0; i < ProtocolPos; i++)
	{
		// inline ���·� Write �Լ��� �����մϴ�.
		_ftprintf(WritePacketFile, _T("inline DWORD WRITE_%s(BYTE *buffer, S_%s &parameter)\n{\n"), Protocol[i].Name, Protocol[i].Name);

		// ��Ŷ�� ����� �ڵ带 ����� ���ؼ� CStreamSP Ŭ������ �̿��մϴ�.
		_ftprintf(WritePacketFile, _T("\tCStreamSP Stream;\n"));
		_ftprintf(WritePacketFile, _T("\tStream->SetBuffer(buffer);\n\n"));

		// �ش� ���������� �Ķ���� ���� ��ŭ �ݺ��մϴ�.
		for (DWORD j = 0; j < Protocol[i].ParameterCount; j++)
		{
			// �Ķ������ ���� Ȯ��
			if (!_tcscmp(Protocol[i].Parameters[j].Type, _T("INT")))
			{
				// WriteINT32 �Լ��� �̿��ؼ� �����͸� �Է��մϴ�.
				_ftprintf(WritePacketFile, _T("\tStream->WriteINT32(parameter.%s);\n"), Protocol[i].Parameters[j].Name);
			}
			else if (!_tcscmp(Protocol[i].Parameters[j].Type, _T("DWORD")))
			{
				// WriteDWORD �Լ��� �̿��ؼ� �����͸� �Է��մϴ�.
				_ftprintf(WritePacketFile, _T("\tStream->WriteDWORD(parameter.%s);\n"), Protocol[i].Parameters[j].Name);
			}
			else if (!_tcscmp(Protocol[i].Parameters[j].Type, _T("DWORD_PTR")))
			{
				// WriteDWORD_PTR �Լ��� �̿��ؼ� �����͸� �Է��մϴ�.
				_ftprintf(WritePacketFile, _T("\tStream->WriteDWORD_PTR(parameter.%s);\n"), Protocol[i].Parameters[j].Name);
			}
			else if (!_tcscmp(Protocol[i].Parameters[j].Type, _T("FLOAT")))
			{
				// WriteFLOAT �Լ��� �̿��ؼ� �����͸� �Է��մϴ�.
				_ftprintf(WritePacketFile, _T("\tStream->WriteFLOAT(parameter.%s);\n"), Protocol[i].Parameters[j].Name);
			}
			else if (!_tcscmp(Protocol[i].Parameters[j].Type, _T("INT64")))
			{
				// WriteINT64 �Լ��� �̿��ؼ� �����͸� �Է��մϴ�.
				_ftprintf(WritePacketFile, _T("\tStream->WriteINT64(parameter.%s);\n"), Protocol[i].Parameters[j].Name);
			}
			else if (!_tcscmp(Protocol[i].Parameters[j].Type, _T("USHORT")))
			{
				// WriteUSHORT �Լ��� �̿��ؼ� �����͸� �Է��մϴ�.
				_ftprintf(WritePacketFile, _T("\tStream->WriteUSHORT(parameter.%s);\n"), Protocol[i].Parameters[j].Name);
			}
			else if (!_tcscmp(Protocol[i].Parameters[j].Type, _T("SHORT")))
			{
				// WriteSHORT �Լ��� �̿��ؼ� �����͸� �Է��մϴ�.
				_ftprintf(WritePacketFile, _T("\tStream->WriteSHORT(parameter.%s);\n"), Protocol[i].Parameters[j].Name);
			}
			else if (!_tcscmp(Protocol[i].Parameters[j].Type, _T("BOOL")))
			{
				// WriteBOOL �Լ��� �̿��ؼ� �����͸� �Է��մϴ�.
				_ftprintf(WritePacketFile, _T("\tStream->WriteBOOL(parameter.%s);\n"), Protocol[i].Parameters[j].Name);
			}
			else if (!_tcscmp(Protocol[i].Parameters[j].Type, _T("BYTE")))
			{
				// ���� �Ķ������ ���̰� 0�̸�
				if (Protocol[i].Parameters[j].Length == 0)
				{
					// WriteByte �Լ��� �̿��ؼ� �����͸� �Է��մϴ�.
					_ftprintf(WritePacketFile, _T("\tStream->WriteByte(parameter.%s);\n"), Protocol[i].Parameters[j].Name);
				}
				else
				{
					// WriteBytes �Լ��� �̿��ؼ� �����͸� �Է��մϴ�.
					_ftprintf(WritePacketFile, _T("\tStream->WriteBytes(parameter.%s, %d);\n"),
						Protocol[i].Parameters[j].Name,
						Protocol[i].Parameters[j].Length);
				}
			}
			else if (!_tcscmp(Protocol[i].Parameters[j].Type, _T("WCHAR")))
			{
				// ���� �Ķ������ ���̰� 0�̸�
				if (Protocol[i].Parameters[j].Length == 0)
				{
					// WriteWCHAR �Լ��� �̿��ؼ� �����͸� �Է��մϴ�.
					_ftprintf(WritePacketFile, _T("\tStream->WriteWCHAR(parameter.%s);\n"), Protocol[i].Parameters[j].Name);
				}
				else
				{
					// WriteWCHARs �Լ��� �̿��ؼ� �����͸� �Է��մϴ�.
					_ftprintf(WritePacketFile, _T("\tStream->WriteWCHARs(parameter.%s, %d);\n"),
						Protocol[i].Parameters[j].Name,
						Protocol[i].Parameters[j].Length);
				}
			}
			else
			{
				// WriteBytes �Լ��� �̿��ؼ� �����͸� �Է��մϴ�.
				_ftprintf(WritePacketFile, _T("\tStream->WriteBytes((BYTE*)parameter.%s, %d);\n"),
					Protocol[i].Parameters[j].Name,
					Protocol[i].Parameters[j].Length);
			}
		}

		_ftprintf(WritePacketFile, _T("\n\treturn Stream->GetLength();\n}\n\n"));
	}


	// ���� �Լ� �̸��� �Ķ���͸� �޸��ؼ� �Ķ���� Ÿ�� �Լ��� �����մϴ�.
	// Write �Լ� �Ķ���� Ÿ��
	// ��ü �������� ���� ��ŭ �ݺ��մϴ�.
	for (DWORD i = 0; i < ProtocolPos; i++)
	{
		// inline ���·� Write �Լ��� �����մϴ�.
		_ftprintf(WritePacketFile, _T("inline DWORD WRITE_%s(BYTE *buffer"), Protocol[i].Name);

		// �ش� ���������� �Ķ���� ���� ��ŭ �ݺ��մϴ�.
		for (DWORD j = 0; j < Protocol[i].ParameterCount; j++)
		{
			// �Ķ������ ���̰� 0 ���� Ŭ ��� ������ ������ ����ϴ�.
			if (Protocol[i].Parameters[j].Length > 0)
				_ftprintf(WritePacketFile, _T(", %s *%s"), Protocol[i].Parameters[j].Type, _tcslwr(Protocol[i].Parameters[j].Name));
			else 
				// �Ķ������ ���̰� 0�� ��� �Ϲ� ������ ����ϴ�.
				_ftprintf(WritePacketFile, _T(", %s %s"), Protocol[i].Parameters[j].Type, _tcslwr(Protocol[i].Parameters[j].Name));
		}

		_ftprintf(WritePacketFile, _T(")\n{\n"));

		// ��Ŷ�� ����� �ڵ带 ����� ���ؼ� CStreamSP Ŭ������ �̿��մϴ�.
		_ftprintf(WritePacketFile, _T("\tCStreamSP Stream;\n"));
		_ftprintf(WritePacketFile, _T("\tStream->SetBuffer(buffer);\n\n"));

		// �ش� ���������� �Ķ���� ���� ��ŭ �ݺ��մϴ�.
		for (DWORD j = 0; j < Protocol[i].ParameterCount; j++)
		{
			// �Ķ������ ���� Ȯ��
			if (!_tcscmp(Protocol[i].Parameters[j].Type, _T("INT")))
			{
				// WriteINT32 �Լ��� �̿��ؼ� �����͸� �Է��մϴ�.
				_ftprintf(WritePacketFile, _T("\tStream->WriteINT32(%s);\n"), _tcslwr(Protocol[i].Parameters[j].Name));
			}
			else if (!_tcscmp(Protocol[i].Parameters[j].Type, _T("DWORD")))
			{
				// WriteDWORD �Լ��� �̿��ؼ� �����͸� �Է��մϴ�.
				_ftprintf(WritePacketFile, _T("\tStream->WriteDWORD(%s);\n"), _tcslwr(Protocol[i].Parameters[j].Name));
			}
			else if (!_tcscmp(Protocol[i].Parameters[j].Type, _T("DWORD_PTR")))
			{
				// WriteDWORD_PTR �Լ��� �̿��ؼ� �����͸� �Է��մϴ�.
				_ftprintf(WritePacketFile, _T("\tStream->WriteDWORD_PTR(%s);\n"), _tcslwr(Protocol[i].Parameters[j].Name));
			}
			else if (!_tcscmp(Protocol[i].Parameters[j].Type, _T("FLOAT")))
			{
				// WriteFLOAT �Լ��� �̿��ؼ� �����͸� �Է��մϴ�.
				_ftprintf(WritePacketFile, _T("\tStream->WriteFLOAT(%s);\n"), _tcslwr(Protocol[i].Parameters[j].Name));
			}
			else if (!_tcscmp(Protocol[i].Parameters[j].Type, _T("INT64")))
			{
				// WriteINT64 �Լ��� �̿��ؼ� �����͸� �Է��մϴ�.
				_ftprintf(WritePacketFile, _T("\tStream->WriteINT64(%s);\n"), _tcslwr(Protocol[i].Parameters[j].Name));
			}
			else if (!_tcscmp(Protocol[i].Parameters[j].Type, _T("USHORT")))
			{
				// WriteUSHORT �Լ��� �̿��ؼ� �����͸� �Է��մϴ�.
				_ftprintf(WritePacketFile, _T("\tStream->WriteUSHORT(%s);\n"), _tcslwr(Protocol[i].Parameters[j].Name));
			}
			else if (!_tcscmp(Protocol[i].Parameters[j].Type, _T("SHORT")))
			{
				// WriteSHORT �Լ��� �̿��ؼ� �����͸� �Է��մϴ�.
				_ftprintf(WritePacketFile, _T("\tStream->WriteSHORT(%s);\n"), _tcslwr(Protocol[i].Parameters[j].Name));
			}
			else if (!_tcscmp(Protocol[i].Parameters[j].Type, _T("BOOL")))
			{
				// WriteBOOL �Լ��� �̿��ؼ� �����͸� �Է��մϴ�.
				_ftprintf(WritePacketFile, _T("\tStream->WriteBOOL(%s);\n"), _tcslwr(Protocol[i].Parameters[j].Name));
			}
			else if (!_tcscmp(Protocol[i].Parameters[j].Type, _T("BYTE")))
			{
				// ���� �Ķ������ ���̰� 0�̸�
				if (Protocol[i].Parameters[j].Length == 0)
				{
					// WriteByte �Լ��� �̿��ؼ� �����͸� �Է��մϴ�.
					_ftprintf(WritePacketFile, _T("\tStream->WriteByte(%s);\n"), _tcslwr(Protocol[i].Parameters[j].Name));
				}
				else
				{
					// WriteBytes �Լ��� �̿��ؼ� �����͸� �Է��մϴ�.
					_ftprintf(WritePacketFile, _T("\tStream->WriteBytes(%s, %d);\n"),
						_tcslwr(Protocol[i].Parameters[j].Name),
						Protocol[i].Parameters[j].Length);
				}
			}
			else if (!_tcscmp(Protocol[i].Parameters[j].Type, _T("WCHAR")))
			{
				// ���� �Ķ������ ���̰� 0�̸�
				if (Protocol[i].Parameters[j].Length == 0)
				{
					// WriteWCHAR �Լ��� �̿��ؼ� �����͸� �Է��մϴ�.
					_ftprintf(WritePacketFile, _T("\tStream->WriteWCHAR(%s);\n"), _tcslwr(Protocol[i].Parameters[j].Name));
				}
				else
				{
					// WriteWCHARs �Լ��� �̿��ؼ� �����͸� �Է��մϴ�.

					_ftprintf(WritePacketFile, _T("\tWCHAR _%s[%d] = { 0, };\n"),
						_tcslwr(Protocol[i].Parameters[j].Name),
						Protocol[i].Parameters[j].Length);

					_ftprintf(WritePacketFile, _T("\t_tcsncpy(_%s, %s, %d);\n"),
						_tcslwr(Protocol[i].Parameters[j].Name),
						_tcslwr(Protocol[i].Parameters[j].Name),
						Protocol[i].Parameters[j].Length);

					_ftprintf(WritePacketFile, _T("\tStream->WriteWCHARs(%s, %d);\n"),
						_tcslwr(Protocol[i].Parameters[j].Name),
						Protocol[i].Parameters[j].Length);
				}
			}
			else
			{
				// WriteBytes �Լ��� �̿��ؼ� �����͸� �Է��մϴ�.
				_ftprintf(WritePacketFile, _T("\tStream->WriteBytes((BYTE*)%s, %d);\n"),
					Protocol[i].Parameters[j].Name,
					Protocol[i].Parameters[j].Length);
			}
		}

		_ftprintf(WritePacketFile, _T("\n\treturn Stream->GetLength();\n}\n\n"));
	}

	fclose(WritePacketFile);
	// Write ��Ŷ ���� �Ϸ�

	delete[] Protocol;

	return 0;

#pragma endregion

}