#include "C_LightCom.h"


C_LightCom::C_LightCom(void)
{
}


C_LightCom::~C_LightCom(void)
{
}

bool C_LightCom::openPort( const char* portNum )
{
	m_com = CreateFileA(portNum, GENERIC_READ | GENERIC_WRITE, 0 ,NULL, OPEN_EXISTING, 0, NULL);
	if (m_com == INVALID_HANDLE_VALUE)
	{
		return false;
	}
	else
	{
		return true;
	}
}

bool C_LightCom::setupPort( DWORD baudrate, BYTE bytesize, BYTE stopbits, DWORD fparity, BYTE parity )
{
	DCB mDcb;
	if (!GetCommState(m_com, &mDcb))
	{
		return false;
	}

	mDcb.BaudRate = baudrate;
	mDcb.fParity = fparity;
	mDcb.Parity = parity;
	mDcb.StopBits = stopbits;
	mDcb.ByteSize = bytesize;

	if (!SetCommState(m_com, &mDcb))
	{
		return false;
	}
	else
	{
		return true;
	}
}

bool C_LightCom::setBufferAreaSize( DWORD inQueue, DWORD outQueue )
{
	if (!SetupComm(m_com, inQueue, outQueue))
	{
		return false;
	}
	else
	{
		return true;
	}
}

bool C_LightCom::setComTimeout( DWORD readinttim, DWORD readToltimM, DWORD readToltimC,DWORD writeToltimM, DWORD writeToltimC )
{
	COMMTIMEOUTS TimeOuts;
	TimeOuts.ReadIntervalTimeout = readinttim;
	TimeOuts.ReadTotalTimeoutMultiplier = readToltimM;
	TimeOuts.ReadTotalTimeoutConstant = readToltimC;
	TimeOuts.WriteTotalTimeoutMultiplier = writeToltimM;
	TimeOuts.WriteTotalTimeoutConstant = writeToltimC;

	if (!SetCommTimeouts(m_com,&TimeOuts))
	{
		return false;
	}
	else
	{
		return true;
	}
}

bool C_LightCom::purgePort( DWORD flags )
{
	//PURGE_TXCLEAR，清空发送缓冲区；PURGE_RXCLEAR，清空接收缓冲区；
	if (!PurgeComm(m_com, flags))
	{
		return false;
	}
	else
	{
		return true;
	}
}

bool C_LightCom::closePort()
{
	this->purgePort(PURGE_TXCLEAR | PURGE_RXCLEAR);

	if (!CloseHandle(m_com))
	{
		return false;
	}
	else
	{
		return true;
	}
}

bool C_LightCom::sendBuff( const char* datas, unsigned int len )
{
	this->purgePort(PURGE_TXCLEAR);

	//int i1 = datas[0];

	DWORD pWrite;
	if (!WriteFile(m_com, datas, sizeof(const char)*len, &pWrite, NULL))
	{
		return false;
	}
	else
	{
		return true;
	}
}

char* C_LightCom::readBuff()
{
	this->purgePort(PURGE_RXCLEAR);

	char *buffl = new char;
	DWORD pRead;
	if (!ReadFile(m_com, &buffl, 1, /*sizeof(buffl)*/ &pRead, NULL))
	{
		return '\0';
	}
	else
	{
		return buffl;
	}
}