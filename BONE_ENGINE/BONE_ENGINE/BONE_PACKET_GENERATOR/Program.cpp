#include <tchar.h>
#include <Windows.h>
#include <stdio.h>
#pragma warning(disable:4996)


// 최대 한 라인 길이
#define MAX_LINE_LENGTH			255

// 한 프로토콜 당 최대 가질 수 있는 파라미터 개수
#define MAX_PARAMETER_COUNT		255

// 최대 프로토콜 개수
#define MAX_PROTOCOL_COUNT		255

// 프로토콜은 크게 프로토콜과 파라미터로 나뉘어집니다.
// 파라미터
typedef struct _PARAMETER
{
	// 파라미터형
	TCHAR Type[MAX_LINE_LENGTH];

	// 파라미터 변수형
	TCHAR Name[MAX_LINE_LENGTH];

	// 파라미터 길이
	DWORD Length;	// _BYTE형에서만 길이가 있습니다.

} PARAMETER;

// 프로토콜
typedef struct _PROTOCOL
{
	// 프로토콜 이름
	TCHAR Name[MAX_LINE_LENGTH];

	// 파라미터 개수
	DWORD ParameterCount;

	// 파라미터 정보 : 배열로 잡고 최대 파라미터를 가질 수 있는 개수는 255개 입니다.
	PARAMETER Parameters[MAX_PARAMETER_COUNT];
} PROTOCOL;

int _tmain(int argc, _TCHAR* argv[])
{
	// 파일 이름 파라미터가 없다면 실행하지 않습니다.
	//if (argc == 1)
		//return 0;

#pragma region PARSER_FILE
	// 프로토콜 정보를 저장할 PROTOCOL 구조체 배열을 생성합니다.
	PROTOCOL *Protocol = new PROTOCOL[MAX_PROTOCOL_COUNT];
	// 현재 마지막으로 저장된 프로토콜 구조체의 위치입니다.
	DWORD ProtocolPos = 0;

	// 저장할 structure 파일 이름
	TCHAR StructureFileName[MAX_PATH] = { 0, };
	// 저장할 Read 함수 파일 이름
	TCHAR ReadPacketFileName[MAX_PATH] = { 0, };
	// 저장할 Write 함수 파일 이름
	TCHAR WritePacketFileName[MAX_PATH] = { 0, };

	// Structure 파일은 뒤에 _Structure.h를 붙입니다.
	// read 파일은 뒤에 _ReadPacket.h를 붙입니다.
	// write 파일은 뒤에 _WritePacket.h를 붙입니다.
	_sntprintf(StructureFileName, MAX_PATH, _T("%s_Structure.h"), _T("PT"));		//argv[2]
	_sntprintf(ReadPacketFileName, MAX_PATH, _T("%s_ReadPacket.h"), _T("PT"));	//argv[2]
	_sntprintf(WritePacketFileName, MAX_PATH, _T("%s_WritePacket.h"), _T("PT")); //argv[2]

	// 프로토콜 정보를 저장할 메모리를 초기화해줍니다.
	ZeroMemory(Protocol, sizeof(PROTOCOL) * MAX_PROTOCOL_COUNT);

	// 원본 파일 여기서는 Protocol.h를 엽니다.
	FILE *SourceFile = _tfopen(_T("Protocol.h"), _T("r")); //argv[1]

	// 한 라인을 읽어서 저장할 버퍼입니다.
	TCHAR Data[MAX_LINE_LENGTH] = { 0, };
	// 현재 파라미터를 읽는 중인지를 확인하는 FLAG 변수 입니다.
	// 파라미터는 항상 /**/ 안에 들어 있기 때문에 체크할 수 있습니다.
	BOOL IsParameterOpen = FALSE;
	// 현재 몇 번째 파라미터인지를 확인하는 변수입니다.
	DWORD ParameterPos = 0;
	// 한 라인별로 읽어서 프로토콜을 메모리에 저장합니다.
	while (_fgetts(Data, MAX_LINE_LENGTH, SourceFile))
	{
		// 공백, 탭, 엔터 등을 정리한 데이터를 저장하는 버퍼
		TCHAR TempData[MAX_LINE_LENGTH] = { 0, };
		// 현재 읽고 있는 버퍼의 위치
		DWORD TempDataPos = 0;
		for (DWORD i = 0; i < _tcslen(Data); i++)
		{
			// 읽은 한 라인 내의 공백, 탭, 엔터 값을 모두 없애 줍니다.
			if (Data[i] != ' ' && Data[i] != '\t' && Data[i] != '\n' && Data[i] != '\r')
				// TempData 버퍼에 정리해서 넣습니다.
				TempData[TempDataPos++] = Data[i];
		}

		// 이 라인의 데이터가 프로토콜 이름인지 파라미터인지 구분해줍니다.
		// 파라미터일 경우 형과 변수명으로 이루어지고 프로토콜 이름에 따라갑니다.
		// 파라미터는 최대 255개까지 줄 수 있습니다.

		// 라인 안에 ,가 있으면 프로토콜입니다.
		// enum의 특징상 ,로 확인합니다.
		// 이 부분 때문에 파라미터에는 ,가 들어가서는 안됩니다.
		TCHAR *Pos = _tcsstr(TempData, _T(","));
		if (Pos)
		{
			// 프로토콜일 경우 VERSION이라는 문자열을 포함한 경우 무시합니다.
			// 포함하고 있지 않은 경우는 프로토콜 위치를 하나 증가시켜서 프로토콜 이름을 복사합니다.
			if (!_tcsstr(TempData, _T("VERSION")))
				_tcsncpy(Protocol[ProtocolPos++].Name, TempData, Pos - TempData);
			else
			{

			}
		}
		// 프로토콜 이름이 아닐 경우
		// 이경우는 쓸모없는 데이터이거나 파라미터 정보일 것입니다.
		else
		{
			// 데이터에 파라미터 시작 기호가 있을 경우
			if (_tcsstr(TempData, _T("/*")))
			{
				// 파라미터 오픈 FLAG를 TRUE로 만듭니다.
				IsParameterOpen = TRUE;

				// 파라미터 순서 변수를 초기화합니다.
				ParameterPos = 0;

				// 다은 라인을 읽습니다.
				continue;
			}

			// 데이터에 파라미터 종료 기호가 있을 경우
			if (_tcsstr(TempData, _T("*/")))
			{
				// 파라미터 오픈 FLAG를 FALSE로 만듭니다.
				IsParameterOpen = FALSE;
				// 해당 프로토콜의 파라미터 개수를 입력합니다.
				Protocol[ProtocolPos - 1].ParameterCount = ParameterPos;
				// 파라미터 순서를 초기화합니다.
				ParameterPos = 0;
				// 다음 라인을 읽습니다.
				continue;
			}

			// 파라미터 열기 기호나 닫기 기호가 아닐 경우
			if (_tcslen(TempData) != 0)
			{
				// 파라미터가 열린 상태일 경우 (이 경우는 파라미터 데이터를 읽어오는 것입니다.)
				if (IsParameterOpen)
				{
					// 무언가가 있을 경우
					// 파라미터인데, 형, 이름, 사이즈를 분리해줍니다.
					// 구분자는 : 와 [] 입니다.
					Pos = _tcsstr(TempData, _T(":"));

					// 라인에 형, 변스명 구분자가 있을 경우
					if (Pos)
					{
						// 파라미터 형을 복사합니다.
						_tcsncpy(Protocol[ProtocolPos - 1].Parameters[ParameterPos++].Type, TempData, Pos - TempData);
					}
					// : 기호가 없을 경우 다음 라인을 읽습니다.
					else
						continue;

					// :가 있는 라인에 [ 기호가 있을 경우 그 위치를 찾습니다.
					TCHAR *StartPos = _tcsstr(TempData, _T("["));
					// 그 위치가 0이 아닐 경우
					if (StartPos)
					{
						// ] 기호가 있는 위치를 찾습니다.
						TCHAR *EndPos = _tcsstr(TempData, _T("]"));
						TCHAR Length[MAX_LINE_LENGTH] = { 0, };

						// [시작부터 ]끝까지의 길이를 구해서 선언한 파라미터가 배열일 경우
						// 그 배열 크기를 구해서 Length에 저장합니다.
						_tcsncpy(Length, StartPos + 1, EndPos - StartPos - 1);
						// 프로토콜 구조체에 저장합니다.
						Protocol[ProtocolPos - 1].Parameters[ParameterPos - 1].Length = _tstoi(Length);

						// 파라미터 이름을 복사합니다.
						_tcsncpy(Protocol[ProtocolPos - 1].Parameters[ParameterPos - 1].Name, Pos + 1, StartPos - Pos - 1);
					}
					else
					{
						// 만약 [ 기호가 없을 경우는 배열형이 아니기 때문에 길이를 0으로 셋팅합니다.
						Protocol[ProtocolPos - 1].Parameters[ParameterPos - 1].Length = 0;

						// 그리고 파라미터 이름을 복사합니다.
						_tcscpy(Protocol[ProtocolPos - 1].Parameters[ParameterPos - 1].Name, Pos + 1);
					}
				}
			}
		}
	}

	fclose(SourceFile);
	// 위의 과정을 거치게 되면 Protocol.h 파일을 읽어서
	// 그 내용을 프로토콜 배열에 파라미터 값을 포함해서 저장하게 됩니다.
	// 아래의 내용 부터는 저장된 프로토콜 배열에서 Output 파일들을 생성해 내는 과정을 보여줍니다.

#pragma endregion

#pragma region STRUCTURE_FILE_CREATE
	// structure 만들기
	FILE *StructureFile = _tfopen(StructureFileName, _T("w"));

	_ftprintf(StructureFile, _T("#pragma once\n\n"));

	// 읽은 프로토콜 전체 개수만큼 반복됩니다.
	for (DWORD i = 0; i < ProtocolPos; i++)
	{
		// 읽은 프로토콜과 파라미터 정보를 이용해서 구조체를 선언합니다.
		_ftprintf(StructureFile, _T("typedef struct _S_%s\n{\n"), Protocol[i].Name);
		// 읽은 프로토콜의 파라미터 개수만큼 반복합니다.
		for (DWORD j = 0; j < Protocol[i].ParameterCount; j++)
		{
			// 배열형이 아닌 파라미터일 경우
			if (Protocol[i].Parameters[j].Length == 0)
			{
				// 데이터형과 변수명을 써 줍니다.
				_ftprintf(StructureFile, _T("\t%s %s;\n"), Protocol[i].Parameters[j].Type, Protocol[i].Parameters[j].Name);
			}
			else
			{
				// 데이터형, 변수명, 배열의 크기를 써 줍니다.
				_ftprintf(StructureFile, _T("\t%s %s[%d];\n"), Protocol[i].Parameters[j].Type, Protocol[i].Parameters[j].Name, Protocol[i].Parameters[j].Length);
			}
		}

		// 마지막으로 구조체를 닫고 이름을 정해 줍니다.
		_ftprintf(StructureFile, _T("} S_%s;\n\n"), Protocol[i].Name);
	}
	
	fclose(StructureFile);
	// 위의 작업을 반복하게 되면 모든 프로토콜의 파라미터를 포함한 구조체가 생성됩니다.

#pragma endregion

#pragma region READ_FILE_CREATE
	// Read 함수 만들기
	FILE *ReadPacketFile = _tfopen(ReadPacketFileName, _T("w"));

	_ftprintf(ReadPacketFile, _T("#pragma once\n\n"));

	for (DWORD i = 0; i < ProtocolPos; i++)
	{
		// READ 함수 선언입니다. inline 형태로 선언됩니다.
		// 이 READ 함수 들은 위에서 미리 만들어 놓은 구조를 이용해서 데이터를 읽게됩니다.
		_ftprintf(ReadPacketFile, _T("inline VOID READ_%s(BYTE *buffer, S_%s &parameter)\n{\n"), Protocol[i].Name, Protocol[i].Name);

		// 바이너리에서 데이터를 읽기 위해서 CStreamSP 클래스를 이용합니다.
		_ftprintf(ReadPacketFile, _T("\tCStreamSP Stream;\n"));
		_ftprintf(ReadPacketFile, _T("\tStream->SetBuffer(buffer);\n\n"));

		// 해당 프로토콜의 파라미터 개수 만큼 반복합니다.
		for (DWORD j = 0; j < Protocol[i].ParameterCount; j++)
		{
			// 파라미터의 형별 확인
			if (!_tcscmp(Protocol[i].Parameters[j].Type, _T("INT")))
			{
				// ReadInt32 함수를 이용해서 데이터를 구조체에 입력합니다.
				_ftprintf(ReadPacketFile, _T("\tStream->ReadInt32(&parameter.%s);\n"), Protocol[i].Parameters[j].Name);
			}
			else if (!_tcscmp(Protocol[i].Parameters[j].Type, _T("DWORD")))
			{
				// ReadDWORD 함수를 이용해서 데이터를 구조체에 입력합니다.
				_ftprintf(ReadPacketFile, _T("\tStream->ReadDWORD(&parameter.%s);\n"), Protocol[i].Parameters[j].Name);
			}
			else if (!_tcscmp(Protocol[i].Parameters[j].Type, _T("DWORD_PTR")))
			{
				// ReadDWORD_PTR 함수를 이용해서 데이터를 구조체에 입력합니다.
				_ftprintf(ReadPacketFile, _T("\tStream->ReadDWORD_PTR(&parameter.%s);\n"), Protocol[i].Parameters[j].Name);
			}
			else if (!_tcscmp(Protocol[i].Parameters[j].Type, _T("FLOAT")))
			{
				// ReadFLOAT 함수를 이용해서 데이터를 구조체에 입력합니다.
				_ftprintf(ReadPacketFile, _T("\tStream->ReadFLOAT(&parameter.%s);\n"), Protocol[i].Parameters[j].Name);
			}
			else if (!_tcscmp(Protocol[i].Parameters[j].Type, _T("INT64")))
			{
				// ReadINT64 함수를 이용해서 데이터를 구조체에 입력합니다.
				_ftprintf(ReadPacketFile, _T("\tStream->ReadINT64(&parameter.%s);\n"), Protocol[i].Parameters[j].Name);
			}
			else if (!_tcscmp(Protocol[i].Parameters[j].Type, _T("USHORT")))
			{
				// ReadUSHORT 함수를 이용해서 데이터를 구조체에 입력합니다.
				_ftprintf(ReadPacketFile, _T("\tStream->ReadUSHORT(&parameter.%s);\n"), Protocol[i].Parameters[j].Name);
			}
			else if (!_tcscmp(Protocol[i].Parameters[j].Type, _T("SHORT")))
			{
				// ReadSHORT 함수를 이용해서 데이터를 구조체에 입력합니다.
				_ftprintf(ReadPacketFile, _T("\tStream->ReadSHORT(&parameter.%s);\n"), Protocol[i].Parameters[j].Name);
			}
			else if (!_tcscmp(Protocol[i].Parameters[j].Type, _T("BOOL")))
			{
				// ReadBOOL 함수를 이용해서 데이터를 구조체에 입력합니다.
				_ftprintf(ReadPacketFile, _T("\tStream->ReadBOOL(&parameter.%s);\n"), Protocol[i].Parameters[j].Name);
			}
			else if (!_tcscmp(Protocol[i].Parameters[j].Type, _T("BYTE")))
			{
				// 만약 파라미터의 길이가 0이면
				if (Protocol[i].Parameters[j].Length == 0)
				{
					// ReadByte 함수를 이용해서 데이터를 구조체에 입력합니다.
					_ftprintf(ReadPacketFile, _T("\tStream->ReadByte(&parameter.%s);\n"), Protocol[i].Parameters[j].Name);
				}
				else
				{
					// ReadBytes 함수를 이용해서 데이터를 구조체에 입력합니다.
					_ftprintf(ReadPacketFile, _T("\tStream->ReadBytes(parameter.%s, %d);\n"), 
						Protocol[i].Parameters[j].Name,
						Protocol[i].Parameters[j].Length);
				}
			}
			else if (!_tcscmp(Protocol[i].Parameters[j].Type, _T("WCHAR")))
			{
				// 만약 파라미터의 길이가 0이면
				if (Protocol[i].Parameters[j].Length == 0)
				{
					// ReadWCHAR 함수를 이용해서 데이터를 구조체에 입력합니다.
					_ftprintf(ReadPacketFile, _T("\tStream->ReadWCHAR(&parameter.%s);\n"), Protocol[i].Parameters[j].Name);
				}
				else
				{
					// ReadWCHARs 함수를 이용해서 데이터를 구조체에 입력합니다.
					_ftprintf(ReadPacketFile, _T("\tStream->ReadWCHARs(parameter.%s, %d);\n"),
						Protocol[i].Parameters[j].Name,
						Protocol[i].Parameters[j].Length);
				}
			}
			else
			{
				// ReadBytes 함수를 이용해서 데이터를 구조체에 입력합니다.
				_ftprintf(ReadPacketFile, _T("\tStream->ReadBytes((BYTE*)parameter.%s, %d);\n"),
					Protocol[i].Parameters[j].Name,
					Protocol[i].Parameters[j].Length);
			}
		}
		_ftprintf(ReadPacketFile, _T("}\n\n"));
	}

	fclose(ReadPacketFile);
	// 위 부분을 종료하면 Read 함수를 저장해 놓은 파일이 한개 생성됩니다.
#pragma endregion

#pragma region WRITE_FILE_CREATE
	// 이제부터 살펴볼 Write 함수 파일은 2가지의 Write 함수가 존재합니다.
	// 구조체를 이용한 Write 함수와 파라미터를 하나씩 입력하는 파라미터 타입이 존재합니다.
	// 구조체 타입의 Write 함수 만들기
	FILE *WritePacketFile = _tfopen(WritePacketFileName, _T("w"));

	_ftprintf(WritePacketFile, _T("#pragma once\n\n"));

	for (DWORD i = 0; i < ProtocolPos; i++)
	{
		// inline 형태로 Write 함수를 선언합니다.
		_ftprintf(WritePacketFile, _T("inline DWORD WRITE_%s(BYTE *buffer, S_%s &parameter)\n{\n"), Protocol[i].Name, Protocol[i].Name);

		// 패킷을 만드는 코드를 만들기 위해서 CStreamSP 클래스를 이용합니다.
		_ftprintf(WritePacketFile, _T("\tCStreamSP Stream;\n"));
		_ftprintf(WritePacketFile, _T("\tStream->SetBuffer(buffer);\n\n"));

		// 해당 프로토콜의 파라미터 개수 만큼 반복합니다.
		for (DWORD j = 0; j < Protocol[i].ParameterCount; j++)
		{
			// 파라미터의 형별 확인
			if (!_tcscmp(Protocol[i].Parameters[j].Type, _T("INT")))
			{
				// WriteINT32 함수를 이용해서 데이터를 입력합니다.
				_ftprintf(WritePacketFile, _T("\tStream->WriteINT32(parameter.%s);\n"), Protocol[i].Parameters[j].Name);
			}
			else if (!_tcscmp(Protocol[i].Parameters[j].Type, _T("DWORD")))
			{
				// WriteDWORD 함수를 이용해서 데이터를 입력합니다.
				_ftprintf(WritePacketFile, _T("\tStream->WriteDWORD(parameter.%s);\n"), Protocol[i].Parameters[j].Name);
			}
			else if (!_tcscmp(Protocol[i].Parameters[j].Type, _T("DWORD_PTR")))
			{
				// WriteDWORD_PTR 함수를 이용해서 데이터를 입력합니다.
				_ftprintf(WritePacketFile, _T("\tStream->WriteDWORD_PTR(parameter.%s);\n"), Protocol[i].Parameters[j].Name);
			}
			else if (!_tcscmp(Protocol[i].Parameters[j].Type, _T("FLOAT")))
			{
				// WriteFLOAT 함수를 이용해서 데이터를 입력합니다.
				_ftprintf(WritePacketFile, _T("\tStream->WriteFLOAT(parameter.%s);\n"), Protocol[i].Parameters[j].Name);
			}
			else if (!_tcscmp(Protocol[i].Parameters[j].Type, _T("INT64")))
			{
				// WriteINT64 함수를 이용해서 데이터를 입력합니다.
				_ftprintf(WritePacketFile, _T("\tStream->WriteINT64(parameter.%s);\n"), Protocol[i].Parameters[j].Name);
			}
			else if (!_tcscmp(Protocol[i].Parameters[j].Type, _T("USHORT")))
			{
				// WriteUSHORT 함수를 이용해서 데이터를 입력합니다.
				_ftprintf(WritePacketFile, _T("\tStream->WriteUSHORT(parameter.%s);\n"), Protocol[i].Parameters[j].Name);
			}
			else if (!_tcscmp(Protocol[i].Parameters[j].Type, _T("SHORT")))
			{
				// WriteSHORT 함수를 이용해서 데이터를 입력합니다.
				_ftprintf(WritePacketFile, _T("\tStream->WriteSHORT(parameter.%s);\n"), Protocol[i].Parameters[j].Name);
			}
			else if (!_tcscmp(Protocol[i].Parameters[j].Type, _T("BOOL")))
			{
				// WriteBOOL 함수를 이용해서 데이터를 입력합니다.
				_ftprintf(WritePacketFile, _T("\tStream->WriteBOOL(parameter.%s);\n"), Protocol[i].Parameters[j].Name);
			}
			else if (!_tcscmp(Protocol[i].Parameters[j].Type, _T("BYTE")))
			{
				// 만약 파라미터의 길이가 0이면
				if (Protocol[i].Parameters[j].Length == 0)
				{
					// WriteByte 함수를 이용해서 데이터를 입력합니다.
					_ftprintf(WritePacketFile, _T("\tStream->WriteByte(parameter.%s);\n"), Protocol[i].Parameters[j].Name);
				}
				else
				{
					// WriteBytes 함수를 이용해서 데이터를 입력합니다.
					_ftprintf(WritePacketFile, _T("\tStream->WriteBytes(parameter.%s, %d);\n"),
						Protocol[i].Parameters[j].Name,
						Protocol[i].Parameters[j].Length);
				}
			}
			else if (!_tcscmp(Protocol[i].Parameters[j].Type, _T("WCHAR")))
			{
				// 만약 파라미터의 길이가 0이면
				if (Protocol[i].Parameters[j].Length == 0)
				{
					// WriteWCHAR 함수를 이용해서 데이터를 입력합니다.
					_ftprintf(WritePacketFile, _T("\tStream->WriteWCHAR(parameter.%s);\n"), Protocol[i].Parameters[j].Name);
				}
				else
				{
					// WriteWCHARs 함수를 이용해서 데이터를 입력합니다.
					_ftprintf(WritePacketFile, _T("\tStream->WriteWCHARs(parameter.%s, %d);\n"),
						Protocol[i].Parameters[j].Name,
						Protocol[i].Parameters[j].Length);
				}
			}
			else
			{
				// WriteBytes 함수를 이용해서 데이터를 입력합니다.
				_ftprintf(WritePacketFile, _T("\tStream->WriteBytes((BYTE*)parameter.%s, %d);\n"),
					Protocol[i].Parameters[j].Name,
					Protocol[i].Parameters[j].Length);
			}
		}

		_ftprintf(WritePacketFile, _T("\n\treturn Stream->GetLength();\n}\n\n"));
	}


	// 같은 함수 이름에 파라미터만 달리해서 파라미터 타입 함수를 제작합니다.
	// Write 함수 파라미터 타입
	// 전체 프로토콜 개수 만큼 반복합니다.
	for (DWORD i = 0; i < ProtocolPos; i++)
	{
		// inline 형태로 Write 함수를 선언합니다.
		_ftprintf(WritePacketFile, _T("inline DWORD WRITE_%s(BYTE *buffer"), Protocol[i].Name);

		// 해당 프로토콜의 파라미터 개수 만큼 반복합니다.
		for (DWORD j = 0; j < Protocol[i].ParameterCount; j++)
		{
			// 파라미터의 길이가 0 보다 클 경우 포인터 변수로 만듭니다.
			if (Protocol[i].Parameters[j].Length > 0)
				_ftprintf(WritePacketFile, _T(", %s *%s"), Protocol[i].Parameters[j].Type, _tcslwr(Protocol[i].Parameters[j].Name));
			else 
				// 파라미터의 길이가 0일 경우 일반 변수로 만듭니다.
				_ftprintf(WritePacketFile, _T(", %s %s"), Protocol[i].Parameters[j].Type, _tcslwr(Protocol[i].Parameters[j].Name));
		}

		_ftprintf(WritePacketFile, _T(")\n{\n"));

		// 패킷을 만드는 코드를 만들기 위해서 CStreamSP 클래스를 이용합니다.
		_ftprintf(WritePacketFile, _T("\tCStreamSP Stream;\n"));
		_ftprintf(WritePacketFile, _T("\tStream->SetBuffer(buffer);\n\n"));

		// 해당 프로토콜의 파라미터 개수 만큼 반복합니다.
		for (DWORD j = 0; j < Protocol[i].ParameterCount; j++)
		{
			// 파라미터의 형별 확인
			if (!_tcscmp(Protocol[i].Parameters[j].Type, _T("INT")))
			{
				// WriteINT32 함수를 이용해서 데이터를 입력합니다.
				_ftprintf(WritePacketFile, _T("\tStream->WriteINT32(%s);\n"), _tcslwr(Protocol[i].Parameters[j].Name));
			}
			else if (!_tcscmp(Protocol[i].Parameters[j].Type, _T("DWORD")))
			{
				// WriteDWORD 함수를 이용해서 데이터를 입력합니다.
				_ftprintf(WritePacketFile, _T("\tStream->WriteDWORD(%s);\n"), _tcslwr(Protocol[i].Parameters[j].Name));
			}
			else if (!_tcscmp(Protocol[i].Parameters[j].Type, _T("DWORD_PTR")))
			{
				// WriteDWORD_PTR 함수를 이용해서 데이터를 입력합니다.
				_ftprintf(WritePacketFile, _T("\tStream->WriteDWORD_PTR(%s);\n"), _tcslwr(Protocol[i].Parameters[j].Name));
			}
			else if (!_tcscmp(Protocol[i].Parameters[j].Type, _T("FLOAT")))
			{
				// WriteFLOAT 함수를 이용해서 데이터를 입력합니다.
				_ftprintf(WritePacketFile, _T("\tStream->WriteFLOAT(%s);\n"), _tcslwr(Protocol[i].Parameters[j].Name));
			}
			else if (!_tcscmp(Protocol[i].Parameters[j].Type, _T("INT64")))
			{
				// WriteINT64 함수를 이용해서 데이터를 입력합니다.
				_ftprintf(WritePacketFile, _T("\tStream->WriteINT64(%s);\n"), _tcslwr(Protocol[i].Parameters[j].Name));
			}
			else if (!_tcscmp(Protocol[i].Parameters[j].Type, _T("USHORT")))
			{
				// WriteUSHORT 함수를 이용해서 데이터를 입력합니다.
				_ftprintf(WritePacketFile, _T("\tStream->WriteUSHORT(%s);\n"), _tcslwr(Protocol[i].Parameters[j].Name));
			}
			else if (!_tcscmp(Protocol[i].Parameters[j].Type, _T("SHORT")))
			{
				// WriteSHORT 함수를 이용해서 데이터를 입력합니다.
				_ftprintf(WritePacketFile, _T("\tStream->WriteSHORT(%s);\n"), _tcslwr(Protocol[i].Parameters[j].Name));
			}
			else if (!_tcscmp(Protocol[i].Parameters[j].Type, _T("BOOL")))
			{
				// WriteBOOL 함수를 이용해서 데이터를 입력합니다.
				_ftprintf(WritePacketFile, _T("\tStream->WriteBOOL(%s);\n"), _tcslwr(Protocol[i].Parameters[j].Name));
			}
			else if (!_tcscmp(Protocol[i].Parameters[j].Type, _T("BYTE")))
			{
				// 만약 파라미터의 길이가 0이면
				if (Protocol[i].Parameters[j].Length == 0)
				{
					// WriteByte 함수를 이용해서 데이터를 입력합니다.
					_ftprintf(WritePacketFile, _T("\tStream->WriteByte(%s);\n"), _tcslwr(Protocol[i].Parameters[j].Name));
				}
				else
				{
					// WriteBytes 함수를 이용해서 데이터를 입력합니다.
					_ftprintf(WritePacketFile, _T("\tStream->WriteBytes(%s, %d);\n"),
						_tcslwr(Protocol[i].Parameters[j].Name),
						Protocol[i].Parameters[j].Length);
				}
			}
			else if (!_tcscmp(Protocol[i].Parameters[j].Type, _T("WCHAR")))
			{
				// 만약 파라미터의 길이가 0이면
				if (Protocol[i].Parameters[j].Length == 0)
				{
					// WriteWCHAR 함수를 이용해서 데이터를 입력합니다.
					_ftprintf(WritePacketFile, _T("\tStream->WriteWCHAR(%s);\n"), _tcslwr(Protocol[i].Parameters[j].Name));
				}
				else
				{
					// WriteWCHARs 함수를 이용해서 데이터를 입력합니다.

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
				// WriteBytes 함수를 이용해서 데이터를 입력합니다.
				_ftprintf(WritePacketFile, _T("\tStream->WriteBytes((BYTE*)%s, %d);\n"),
					Protocol[i].Parameters[j].Name,
					Protocol[i].Parameters[j].Length);
			}
		}

		_ftprintf(WritePacketFile, _T("\n\treturn Stream->GetLength();\n}\n\n"));
	}

	fclose(WritePacketFile);
	// Write 패킷 생성 완료

	delete[] Protocol;

	return 0;

#pragma endregion

}