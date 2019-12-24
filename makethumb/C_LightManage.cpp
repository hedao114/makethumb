#include "C_LightManage.h"
#include "math.h"

C_LightManage::C_LightManage(void)
{
	m_com.openPort("COM1");
	m_com.setupPort(9600, 8, ONESTOPBIT, FALSE, NOPARITY);
	m_com.setBufferAreaSize(1024, 1024);

	for (int i = 0;i<4;i++)
	{
		m_bLights[i] = false;
		m_iLights[i] = 255;
	}
	m_pStartAdjust = "TSTAADJ";
	m_pEndAdjust = "Tendadj";
}


C_LightManage::~C_LightManage(void)
{
	m_com.closePort();
}

void C_LightManage::openLight( int iNum )
{
	if(m_bLights[iNum] == false)
	{
		m_bLights[iNum] = true;
	}
	int sum = 0;
	for (int i = 0;i<4;i++)
	{
		if(m_bLights[i] == true)
		{
			sum += pow(2.0, i);
		}
	}

	char cOpen = 128 + sum;
	//char test = 0x82;
	m_com.sendBuff(&cOpen, 1);
}

void C_LightManage::closeLight( int iNum )
{
	if(m_bLights[iNum] == true)
	{
		m_bLights[iNum] = false;
	}
	int sum = 0;
	for (int i = 0;i<4;i++)
	{
		if(m_bLights[i] == true)
		{
			sum += pow(2.0, i);
		}
	}

	char cClose = 128 + sum;
	m_com.sendBuff(&cClose, 1);
}

void C_LightManage::turnLightUp( int iNum )
{

	if(m_iLights[iNum] == 255)
		return;
	m_com.sendBuff(m_pStartAdjust,8);
	
	sendUpOrder(iNum);
	
	
	m_com.sendBuff(m_pEndAdjust, 8);
	
}

void C_LightManage::turnLightDown( int iNum )
{
	if(m_iLights[iNum] == 0)
		return;
	m_com.sendBuff(m_pStartAdjust,7);
	//char* ccc = m_com.readBuff();
	sendDownOrder(iNum);
	//ccc = m_com.readBuff();
	m_com.sendBuff(m_pEndAdjust, 7);
	//ccc = m_com.readBuff();
}

void C_LightManage::openAllLight()
{
	char cOpen = 128 + 15;
	m_com.sendBuff(&cOpen, 1);
}

void C_LightManage::closeAllLight()
{
	char cClose = 128 + 0;
	m_com.sendBuff(&cClose, 1);
}

void C_LightManage::turnLightUpByNumber( int iNum, int iUpNumber /*= 5*/ )
{
	if(m_iLights[iNum] + iUpNumber <= 255)
	{
		m_com.sendBuff(m_pStartAdjust,7);
		for (int i = 0;i<iUpNumber;i++)
		{
			sendUpOrder(iNum);
		}
		m_com.sendBuff(m_pEndAdjust, 7);
	}
}

void C_LightManage::turnLightDownByNumber( int iNum, int iDownNumber /*= 5*/ )
{
	if(m_iLights[iNum] - iDownNumber >= 0)
	{
		m_com.sendBuff(m_pStartAdjust,7);
		for (int i = 0;i<iDownNumber;i++)
		{
			sendDownOrder(iNum);
		}
		m_com.sendBuff(m_pEndAdjust, 7);
	}
}

bool C_LightManage::getLightState( int iNum )
{
	return m_bLights[iNum];
}

int C_LightManage::getLightness( int iNum )
{
	return m_iLights[iNum];
}

void C_LightManage::sendUpOrder( int iNum )
{
	if(iNum == 0)
		m_com.sendBuff("TADJLIZ", 7);
	else if(iNum == 1)
		m_com.sendBuff("TADJLIB", 7);
	else if(iNum == 2)
		m_com.sendBuff("TADJLID", 7);
	else
		m_com.sendBuff("TADJLIA", 7);
	m_iLights[iNum]++;
}

void C_LightManage::sendDownOrder( int iNum )
{
	if(iNum == 0)
		m_com.sendBuff("TADJLIc", 7);
	else if(iNum == 1)
		m_com.sendBuff("TADJLIb", 7);
	else if(iNum == 2)
		m_com.sendBuff("TADJLId", 7);
	else
		m_com.sendBuff("TADJLIa", 7);
	m_iLights[iNum]--;
}
