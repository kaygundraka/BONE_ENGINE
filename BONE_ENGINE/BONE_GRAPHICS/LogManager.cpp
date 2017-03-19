#include "Common.h"
#include "LogManager.h"
#include "RenderManager.h"
#pragma warning(disable:4996)

namespace BONE_GRAPHICS
{
    void LogManager::InitializeMembers(bool outputLog)
    {
        ThreadSync sync;

        index = 0;
        logMessage = new char[100];
        preMessage = new char[100];
        SaveLog(outputLog);

        logDialog = nullptr;

        Info("LogManager is initialized");
    }

	std::string GetTime()
	{
		std::string Time;

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

    void LogManager::AddLogGui(LogDialog* logDialog)
    {
        ThreadSync sync;

        this->logDialog = logDialog;
    }

	void LogManager::AddLogToFile(int _type, std::string _log)
	{
		ThreadSync sync;

		TiXmlDocument doc;
		doc.LoadFile(logFileName.c_str());

		TiXmlElement* pRoot = doc.FirstChildElement("BONE_LOG_SYSTEM");

		TiXmlElement* pElem = pRoot->FirstChildElement("LOG_INFO");

		if (_type == LOG_INFO)
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
		else if (_type == LOG_WARNING)
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

    void LogManager::Error(const char * format, ...)
    {
        ThreadSync sync;

        va_list args;
        va_start(args, format);

        ShowMessage(LOG_ERROR, format, args);
    }

    void LogManager::Warning(const char * format, ...)
    {
        ThreadSync sync;

        va_list args;
        va_start(args, format);
        ShowMessage(LOG_WARNING, format, args);
    }

    void LogManager::Info(const char * format, ...)
    {
        ThreadSync sync;

        va_list args;
        va_start(args, format);
        ShowMessage(LOG_INFO, format, args);
    }

    void LogManager::Error(const char * format, va_list args)
    {
        ThreadSync sync;

        ShowMessage(LOG_ERROR, format, args);
    }

    void LogManager::Warning(const char * format, va_list args)
    {
        ThreadSync sync;

        ShowMessage(LOG_WARNING, format, args);
    }

    void LogManager::Info(const char * format, va_list args)
    {
        ThreadSync sync;

        ShowMessage(LOG_INFO, format, args);
    }

	void LogManager::ShowMessage(int _type, const char* _log, va_list args)
	{
		ThreadSync sync;

        int result = vsnprintf(buff, LOGSYSTEM_MAX_BUFFER - 1, _log, args);

        buff[LOGSYSTEM_MAX_BUFFER - 1] = '\0';

        freopen("CONOUT$", "wt", stdout);

        if (_type == LOG_INFO)
            std::cout << "Info : " << buff << std::endl;
        else if (_type == LOG_WARNING)
            std::cout << "Warning : " << buff << std::endl;
        else
            std::cout << "Error : " << buff << std::endl;

        if (logDialog != nullptr)
        {
            std::string log = buff;
            log += '\n';
            logDialog->AddLog(log.c_str());
        }

        if (IsSave)
            AddLogToFile(_type, logMessage);
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
			TiXmlElement* pElem = new TiXmlElement("LOG_INFO");
			pRoot->LinkEndChild(pElem);
			
			pElem = new TiXmlElement("LOG_WARNING");
			pRoot->LinkEndChild(pElem);
			
			pElem = new TiXmlElement("LOG_ERROR");
			pRoot->LinkEndChild(pElem);

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