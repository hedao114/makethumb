#pragma once
#include "tchar.h"
#include "windows.h"
#include "stdio.h"

class C_LightCom
{
public:
	C_LightCom(void);
	~C_LightCom(void);

	bool openPort(const char* portNum);

	bool setupPort(DWORD baudrate, BYTE bytesize, BYTE stopbits, DWORD fparity, BYTE parity);

	bool setBufferAreaSize(DWORD inQueue, DWORD outQueue);

	bool setComTimeout(DWORD readinttim, DWORD readToltimM, DWORD readToltimC,DWORD writeToltimM, DWORD writeToltimC);

	bool purgePort(DWORD flags);

	bool closePort();

	bool sendBuff(const char* datas, unsigned int len);

	char* readBuff();

private:

	HANDLE m_com;
};

