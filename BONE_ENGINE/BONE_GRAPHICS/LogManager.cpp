#include "Common.h"
#include "LogManager.h"
#include "RenderManager.h"

namespace BONE_GRAPHICS
{
	string GetTime()
	{
		string Time;

		time_t timer;
		struct tm *t;
		timer = time(nullptr);
		t = localtime(&timer);
		char TempStr[20];
		itoa(t->tm_year + 1900, TempStr, 10);
		Time = "";
		Time += TempStr;
		itoa(t->tm_mon + 1, TempStr, 10);
		Time += TempStr;
		itoa(t->tm_mday, TempStr, 10);
		Time += TempStr;
		itoa(t->tm_hour, TempStr, 10);
		Time += TempStr;
		itoa(t->tm_min, TempStr, 10);
		Time += TempStr;
		itoa(t->tm_sec, TempStr, 10);
		Time += TempStr;

		return Time;
	}

	void LogManager::AddLogToFile(int _type, string _log)
	{
		ThreadSync sync;

		TiXmlDocument doc;
		doc.LoadFile(logFileName.c_str());

		// 루트 노드 접근
		TiXmlElement* pRoot = doc.FirstChildElement("BONE_LOG_SYSTEM");

		TiXmlElement* pElem = pRoot->FirstChildElement("LOG_SET");

		if (_type == LOG_SET)
		{
			TiXmlElement* pLog = new TiXmlElement("Log");
			_log = "TIME[" + GetTime() + "] : " + _log;
			pLog->LinkEndChild(new TiXmlText(_log.c_str()));
			pElem->LinkEndChild(pLog);
		}
		else if (_type == LOG_ERROR)
		{
			pElem = pElem->NextSiblingElement();

			TiXmlElement* pLog = new TiXmlElement("Log");
			_log = "TIME[" + GetTime() + "] : " + _log;
			pLog->LinkEndChild(new TiXmlText(_log.c_str()));
			pElem->LinkEndChild(pLog);
		}
		else if (_type == LOG_VALUE)
		{
			pElem = pElem->NextSiblingElement();
			pElem = pElem->NextSiblingElement();

			TiXmlElement* pLog = new TiXmlElement("Log");
			_log = "TIME[" + GetTime() + "] : " + _log;
			pLog->LinkEndChild(new TiXmlText(_log.c_str()));
			pElem->LinkEndChild(pLog);
		}
		else if (_type == LOG_MESSAGE)
		{
			pElem = pElem->NextSiblingElement();
			pElem = pElem->NextSiblingElement();
			pElem = pElem->NextSiblingElement();

			TiXmlElement* pLog = new TiXmlElement("Log");
			_log = "TIME[" + GetTime() + "] : " + _log;
			pLog->LinkEndChild(new TiXmlText(_log.c_str()));
			pElem->LinkEndChild(pLog);
		}

		doc.SaveFile(logFileName.c_str());
	}

	void LogManager::InitializeMembers(bool outputLog)
	{
		ThreadSync sync;

		index = 0;
		logMessage = new char[100];
		preMessage = new char[100];
		SaveLog(outputLog);

		ShowMessage(LOG_MESSAGE, "LogManager is initialized");
	}

	void LogManager::ShowMessage(int _type, char* _log)
	{
		ThreadSync sync;

		if (strlen(_log) < MAX_PATH)
		{
			freopen("CONOUT$", "wt", stdout);
			//CONOUT$ - console 창
			//wt - 텍스트 쓰기 모드
			//stdout - 출력될 파일 포인터(모니터로 지정)

			sprintf(logMessage, "%d : ", index++);

			char* TempStr;
			TempStr = new char[100 - strlen(logMessage)];

			strcpy(TempStr, _log);

			strcat(logMessage, TempStr);

			if (strcmp(logMessage, preMessage))
			{
				sprintf(preMessage, "%d : ", index);
				strcat(preMessage, TempStr);

				std::cout << logMessage << std::endl;
			}
			else
				index--;

			if (IsSave)
			{
				AddLogToFile(_type, logMessage);
			}

			delete[] TempStr;
		}
		else
			std::cout << "Message Size - OVERFLOW" << std::endl;
	}

	void LogManager::ShowMessage(int _type, char* _log, float _value)
	{
		ThreadSync sync;

		if (strlen(_log) < MAX_PATH)
		{
			freopen("CONOUT$", "wt", stdout);
			//CONOUT$ - console 창
			//wt - 텍스트 쓰기 모드
			//stdout - 출력될 파일 포인터(모니터로 지정)

			sprintf(logMessage, "%d : ", index++);

			char* TempStr = new char[100 - strlen(logMessage)];

			sprintf(TempStr, _log, _value);

			strcat(logMessage, TempStr);

			if (strcmp(logMessage, preMessage))
			{
				sprintf(preMessage, "%d : ", index);
				strcat(preMessage, TempStr);

				std::cout << logMessage << std::endl;
			}
			else
				index--;

			if (IsSave)
			{
				AddLogToFile(_type, logMessage);
			}

			delete[] TempStr;
		}
		else
			std::cout << "Message Size - OVERFLOW" << std::endl;
	}

	void LogManager::ShowMessage(int _type, char* _log, char* _value)
	{
		ThreadSync sync;

		if (strlen(_log) < MAX_PATH)
		{
			freopen("CONOUT$", "wt", stdout);
			//CONOUT$ - console 창
			//wt - 텍스트 쓰기 모드
			//stdout - 출력될 파일 포인터(모니터로 지정)

			sprintf(logMessage, "%d : ", index++);

			char* TempStr = new char[100 - strlen(logMessage)];

			sprintf(TempStr, _log, _value);
			strcat(logMessage, TempStr);

			if (strcmp(logMessage, preMessage))
			{
				sprintf(preMessage, "%d : ", index);
				strcat(preMessage, TempStr);

				std::cout << logMessage << std::endl;
			}
			else
				index--;

			if (IsSave)
			{
				AddLogToFile(_type, logMessage);
			}

			delete[] TempStr;
		}
		else
			std::cout << "Message Size - OVERFLOW" << std::endl;
	}

	void LogManager::SaveLog(bool _bFlag)
	{
		ThreadSync sync;

		IsSave = _bFlag;

		if (IsSave)
		{
			logFileName = "log_" + GetTime();
			logFileName += ".xml";

			// XML 형식 선언
			TiXmlDocument doc;
			TiXmlDeclaration* pDecl = new TiXmlDeclaration("1.0", "euc-kr", "");
			doc.LinkEndChild(pDecl);

			// Root 노드 추가
			TiXmlElement* pRoot = new TiXmlElement("ETU_ENGINE_LOG_SYSTEM");
			doc.LinkEndChild(pRoot);

			// Comment(주석) 문장 추가.
			TiXmlComment* pComment = new TiXmlComment();
			pComment->SetValue("Settings for Log");
			pRoot->LinkEndChild(pComment);

			// Static Mesh Information
			TiXmlElement* pElem = new TiXmlElement("LOG_SET");
			pRoot->LinkEndChild(pElem);
			//delete pElem;

			pElem = new TiXmlElement("LOG_ERROR");
			pRoot->LinkEndChild(pElem);
			//delete pElem;

			pElem = new TiXmlElement("LOG_VALUE");
			pRoot->LinkEndChild(pElem);
			//delete pElem;

			pElem = new TiXmlElement("LOG_MESSAGE");
			pRoot->LinkEndChild(pElem);

			// XML 파일 형태로 저장.
			doc.SaveFile(logFileName.c_str());
		}
	}

	void LogManager::ReleaseMembers()
	{
		ThreadSync sync;

		FreeConsole();

		delete preMessage;
		delete logMessage;
		std::cout << "inputManager is clear." << std::endl;
	}
}