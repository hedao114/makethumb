#pragma once
#include "C_LightCom.h"
class C_LightManage
{
public:
	C_LightManage(void);
	~C_LightManage(void);

	void openLight(int iNum);

	void openAllLight();

	void closeLight(int iNum);

	void closeAllLight();

	void turnLightUp(int iNum);

	void turnLightUpByNumber(int iNum, int iUpNumber = 5);

	void turnLightDown(int iNum);

	void turnLightDownByNumber(int iNum, int iDownNumber = 5);

	bool getLightState(int iNum);

	int getLightness(int iNum);

private:

	void sendUpOrder(int iNum);

	void sendDownOrder(int iNum);

private:
	bool m_bLights[4];
	int m_iLights[4];
	C_LightCom m_com;
	char* m_pStartAdjust;
	char* m_pEndAdjust;
};

