#ifndef LOGINFO_H
#define LOGINFO_H
#pragma once
#include "stdafx.h"
#define BUFFSIZE 8192
class logInfo
{
public:
	logInfo(void);
	~logInfo(void);

	//��־�ļ�
	FILE* m_pfLogFile;
	char m_logFileName[1000];
	char m_cInfo[BUFFSIZE];
	char systime[20];
	int g_logType = 3; //0:�����; 1:������ļ�; 2:�������Ļ; 3:������ļ�����Ļ; 

	int  SetLogPath(const char *pLogPath);
	int  WriteLogInfo(const char *pInfo);
	void flushLog(void);
	void GetSysTime(void);
	void printLog(const char *format, ...);
	void setLogType(int logType);
};
#endif // LOGINFO_H
