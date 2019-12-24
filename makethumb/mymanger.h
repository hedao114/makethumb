#pragma once


#include <QtWidgets/QMainWindow>
#include "C_LightManage.h"
#include <QTimer>
#include <QMessageBox>
#include "DMC2210.h"
#include "MyCamera.h"

using namespace std;

#define STATUS_ERROR            -1
#define IMAGE_NAME_LEN          64

class MyManger : public QMainWindow
{

public:
	MyManger(QWidget *parent = Q_NULLPTR);
	~MyManger();

private:
	C_LightManage m_light_manage;	//LED
	//-----------------
	BOOL	m_bLogic;
	BOOL	m_bSymmetry;
	BOOL	m_bState;
	double	m_fAccel;
	double	m_fDecel;
	long	m_nPulse;
	long	m_nSacc;
	long	m_nSdec;
	long	m_nSpeed;
	long	m_nStart;
	int		m_nActionst;
	int		m_nAxis;
	int		m_nSpeedst;
	int		m_logic;
	QTimer *timer;
	//------------------
	/*ch:状态 | en:Status*/
	BOOL  m_bOpenDevice;                        // ch:是否打开设备 | en:Whether to open device
	BOOL  m_bStartGrabbing;                     // ch:是否开始抓图 | en:Whether to start grabbing
	int   m_nTriggerMode;                       // ch:触发模式 | en:Trigger Mode
	int   m_nTriggerSource;                     // ch:触发源 | en:Trigger Source
	MV_SAVE_IAMGE_TYPE   m_nSaveImageType;      // ch:保存图像格式 | en:Save Image Type
												/*ch:设备相关 | en:Device Related*/
	CMyCamera*      m_pcMyCamera;               // ch:CMyCamera封装了常用接口 | en:CMyCamera packed commonly used interface
	HWND  m_hwndDisplay;                        // ch:显示句柄 | en:Display Handle
	MV_CC_DEVICE_INFO_LIST m_stDevList;         // ch:设备信息列表结构体变量，用来存储设备列表
												//en:Device information list structure variable used to store device list
	unsigned char*  m_pBufForSaveImage;         // ch:用于保存图像的缓存 | en:Buffer to save image
	unsigned int    m_nBufSizeForSaveImage;

	unsigned char*  m_pBufForDriver;            // ch:用于从驱动获取图像的缓存 | en:Buffer to get image from driver
	unsigned int    m_nBufSizeForDriver;

	//	CComboBox m_ctrlDeviceCombo;                // ch:枚举到的设备 | en:Enumerated device
	int      m_nDeviceCombo;

	BOOL    m_bSoftWareTriggerCheck;
	double  m_dExposureEdit;
	double  m_dGainEdit;
	double  m_dFrameRateEdit;
	//------------------
public:
	void ShowErrorMsg(QString csMessage, int nErrorNum);
public slots:
	//灯光操作
	void OpenLight();
	void CloseLight();
	//控制器操作
	bool InitDMC();
	void CloseDMC();
	void DMC_move();
	void ShowState();
	void DMC_X_inside_move(int value);
	void DMC_Y_inside_move(int value);
	void DMC_X_outside_move(int value);
	void DMC_Y_outside_move(int value);
	void StopAll();
	//相机操作
	bool InitCam();
	int OpenDevice(void);                    // ch:打开相机 | en:Open Device
	int CloseDevice(void);                   // ch:关闭设备 | en:Close Device
	void onBtnStartACQ();
	void onBtnStopACQ();
	void onBtnTriggerOnce();
	void OnClickedContinusModeRadio();        // ch:连续模式 | en:Continus Mode
	void OnClickedTriggerModeRadio();         // ch:触发模式 | en:Trigger Mode
	int SetTriggerMode(void);                // ch:设置触发模式 | en:Set Trigger Mode
};
