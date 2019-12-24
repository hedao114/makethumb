#include "mymanger.h"

#pragma comment(lib,"DMC2210.lib")
#pragma comment(lib,"MvCameraControl.lib")

MyManger::MyManger(QWidget *parent){
	
}

MyManger::~MyManger() {
	
}

// ch:显示错误信息 | en:Show error message
void MyManger::ShowErrorMsg(QString csMessage, int nErrorNum)
{
	QMessageBox::information(this, "提示", csMessage, QMessageBox::Ok | QMessageBox::Cancel, QMessageBox::Cancel);
}

void MyManger::OpenLight()
{
	m_light_manage.openAllLight();
}

void MyManger::CloseLight()
{
	m_light_manage.closeAllLight();
}

bool MyManger::InitDMC()
{

	m_bLogic = FALSE;
	m_bSymmetry = FALSE;
	m_fAccel = 0.1;
	m_fDecel = 0.1;
	m_nPulse = 6400;
	m_nSacc = 100;
	m_nSdec = 100;
	m_nSpeed = 3200;
	m_nStart = 1600;
	m_nActionst = 0;
	m_nAxis = 0;
	m_nSpeedst = 0;
	m_logic = 1;

	if (d2210_board_init() <= 0)//控制卡的初始化操作
	{
//		QMessageBox::information(this, "err", "d2210_board_init err~", QMessageBox::Ok | QMessageBox::Cancel, QMessageBox::Cancel);
		return false;
	}

	timer = new QTimer(this);
	connect(timer, SIGNAL(timeout()), this, SLOT(ShowState()));
	timer->start(100);
	return true;
}

void MyManger::CloseDMC()
{
	d2210_board_close();
}

void MyManger::ShowState()
{
	QString string;
	long position = d2210_get_position(m_nAxis);
	string.sprintf("当前位置：%ld", position);

	DWORD status = d2210_check_done(m_nAxis);
	string.sprintf("状态值：%04x", status);

	if (status == 0)
	{
		m_bState = false;
	}
	if (status == 1)
	{
		m_bState = true;
	}

}

void MyManger::DMC_move()
{
	m_bState = false;
	if ((d2210_check_done(m_nAxis)) & 0x02 == 1) //已经在运动中
		return;
	//设定脉冲模式及逻辑方向（此处脉冲模式固定为P+D方向：脉冲+方向）	
	d2210_set_pulse_outmode(m_nAxis, m_bLogic ? 1 : 1);
	//设定S形截面的特性
	d2210_set_s_profile(m_nAxis, m_nStart, m_nSpeed, m_fAccel, m_fDecel, m_nSacc, m_nSdec);
	//设定T形截面的特性
	d2210_set_profile(m_nAxis, m_nStart, m_nSpeed, m_fAccel, m_fDecel);

	if (m_nActionst == 0) {//点动(位置模式)
						   //暂使用相对运动函数，若需要绝对，替换相对函数为绝对函数即可，其它可不变
		m_nSpeedst == 2 ?//S形驱动
			d2210_s_pmove(m_nAxis, m_nPulse*m_logic, 0) :
			d2210_ex_t_pmove(m_nAxis, m_nPulse*m_logic, 0);
		//T形加速，当起始速度与运动速度相等时，即为匀速
	}
	else {//持续驱动(速度模式)
		int	dir = (-m_nPulse*m_logic > 0) ? 0 : 1;//以点动长度，即得运动方向
		m_nSpeedst == 2 ?//S形驱动
			d2210_s_vmove(m_nAxis, dir) :
			d2210_t_vmove(m_nAxis, dir);
		//T形加速，当起始速度与运动速度相等时，即为匀速
	}

	while (1)
	{
		if (m_bState)
		{
			break;
		}
		Sleep(300);
	}
}

void MyManger::DMC_X_inside_move(int value)
{
	m_bLogic = FALSE;
	m_nPulse = value;
	m_nAxis = 0;
	DMC_move();
}

void MyManger::DMC_Y_inside_move(int value)
{

	m_bLogic = FALSE;
	m_nPulse = value;
	m_nAxis = 1;
	DMC_move();
}
void MyManger::DMC_X_outside_move(int value)
{
	m_bLogic = TRUE;
	m_nPulse = value;
	m_nAxis = 0;
	DMC_move();
}
void MyManger::DMC_Y_outside_move(int value)
{
	m_bLogic = TRUE;
	m_nPulse = value;
	m_nAxis = 1;
	DMC_move();
}

void MyManger::StopAll()
{
	d2210_emg_stop();
}


bool MyManger::InitCam()
{
	QString strMsg;//

// ch:清除设备列表框中的信息 | en:Clear Device List Information
//	m_ctrlDeviceCombo.ResetContent();
//	ui.comboBox->clear();

	// ch:初始化设备信息列表 | en:Device Information List Initialization
	memset(&m_stDevList, 0, sizeof(MV_CC_DEVICE_INFO_LIST));

	// ch:枚举子网内所有设备 | en:Enumerate all devices within subnet
	int nRet = CMyCamera::EnumDevices(&m_stDevList);
	if (MV_OK != nRet)
	{
		return false;
	}

	// ch:将值加入到信息列表框中并显示出来 | en:Add value to the information list box and display
	unsigned int i;
	char Temp[500];
	int nIp1, nIp2, nIp3, nIp4;
	for (i = 0; i < m_stDevList.nDeviceNum; i++)
	{
		MV_CC_DEVICE_INFO* pDeviceInfo = m_stDevList.pDeviceInfo[i];
		if (NULL == pDeviceInfo)
		{
			continue;
		}
		if (pDeviceInfo->nTLayerType == MV_GIGE_DEVICE)
		{
			nIp1 = ((m_stDevList.pDeviceInfo[i]->SpecialInfo.stGigEInfo.nCurrentIp & 0xff000000) >> 24);
			nIp2 = ((m_stDevList.pDeviceInfo[i]->SpecialInfo.stGigEInfo.nCurrentIp & 0x00ff0000) >> 16);
			nIp3 = ((m_stDevList.pDeviceInfo[i]->SpecialInfo.stGigEInfo.nCurrentIp & 0x0000ff00) >> 8);
			nIp4 = (m_stDevList.pDeviceInfo[i]->SpecialInfo.stGigEInfo.nCurrentIp & 0x000000ff);

			wchar_t* pUserName = NULL;
			if (strcmp("", (LPCSTR)(pDeviceInfo->SpecialInfo.stGigEInfo.chUserDefinedName)) != 0)
			{
				DWORD dwLenUserName = MultiByteToWideChar(CP_ACP, 0, (LPCSTR)(pDeviceInfo->SpecialInfo.stGigEInfo.chUserDefinedName), -1, NULL, 0);
				pUserName = new wchar_t[dwLenUserName];
				MultiByteToWideChar(CP_ACP, 0, (LPCSTR)(pDeviceInfo->SpecialInfo.stGigEInfo.chUserDefinedName), -1, pUserName, dwLenUserName);
			}
			else
			{
				char strUserName[256] = { 0 };
				sprintf_s(strUserName, "%s %s (%s)", pDeviceInfo->SpecialInfo.stGigEInfo.chManufacturerName,
					pDeviceInfo->SpecialInfo.stGigEInfo.chModelName,
					pDeviceInfo->SpecialInfo.stGigEInfo.chSerialNumber);
				DWORD dwLenUserName = MultiByteToWideChar(CP_ACP, 0, (LPCSTR)(strUserName), -1, NULL, 0);
				pUserName = new wchar_t[dwLenUserName];
				MultiByteToWideChar(CP_ACP, 0, (LPCSTR)(strUserName), -1, pUserName, dwLenUserName);
			}
			//char Temp[500];
			//memset(Temp,0,500);
			//sprintf(Temp, "[%d]GigE:    %s  (%d.%d.%d.%d)", i,(char*)pUserName, nIp1, nIp2, nIp3, nIp4);
			//strMsg = Temp;
			strMsg.sprintf(("[%d]GigE:    %s  (%d.%d.%d.%d)"), i, pUserName, nIp1, nIp2, nIp3, nIp4);
			if (NULL != pUserName)
			{
				delete(pUserName);
				pUserName = NULL;
			}

		}
		else if (pDeviceInfo->nTLayerType == MV_USB_DEVICE)
		{
			wchar_t* pUserName = NULL;

			if (strcmp("", (char*)pDeviceInfo->SpecialInfo.stUsb3VInfo.chUserDefinedName) != 0)
			{
				DWORD dwLenUserName = MultiByteToWideChar(CP_ACP, 0, (LPCSTR)(pDeviceInfo->SpecialInfo.stUsb3VInfo.chUserDefinedName), -1, NULL, 0);
				pUserName = new wchar_t[dwLenUserName];
				MultiByteToWideChar(CP_ACP, 0, (LPCSTR)(pDeviceInfo->SpecialInfo.stUsb3VInfo.chUserDefinedName), -1, pUserName, dwLenUserName);
			}
			else
			{
				char strUserName[256];
				sprintf_s(strUserName, "%s %s (%s)", pDeviceInfo->SpecialInfo.stUsb3VInfo.chManufacturerName,
					pDeviceInfo->SpecialInfo.stUsb3VInfo.chModelName,
					pDeviceInfo->SpecialInfo.stUsb3VInfo.chSerialNumber);
				DWORD dwLenUserName = MultiByteToWideChar(CP_ACP, 0, (LPCSTR)(strUserName), -1, NULL, 0);
				pUserName = new wchar_t[dwLenUserName];
				MultiByteToWideChar(CP_ACP, 0, (LPCSTR)(strUserName), -1, pUserName, dwLenUserName);
			}
			//memset(Temp, 0, 500);
			//sprintf(Temp, "[%d]UsbV3:  %s", i, (char*)pUserName);
			//strMsg = Temp;
			strMsg.sprintf(("[%d]UsbV3:  %s"), i, pUserName);
			if (NULL != pUserName)
			{
				delete(pUserName);
				pUserName = NULL;
			}

		}
		else
		{
			//ShowErrorMsg(TEXT("Unknown device enumerated"), 0);;
			//QMessageBox::information(this, "提示", "Unknown device enumerated!", QMessageBox::Ok | QMessageBox::Cancel, QMessageBox::Cancel);
		}
//		ui.comboBox->addItem(strMsg);
	}

	if (0 == m_stDevList.nDeviceNum)
	{
		//		ShowErrorMsg(TEXT("No device"), 0);
		//QMessageBox::information(this, "提示", "No device!", QMessageBox::Ok | QMessageBox::Cancel, QMessageBox::Cancel);
		return false;
	}

	return true;
}


// ch:打开设备 | en:Open Device
int MyManger::OpenDevice(void)
{
	if (TRUE == m_bOpenDevice)
	{
		return STATUS_ERROR;
	}
	//	UpdateData(TRUE);
	if (TRUE == m_bOpenDevice)
	{
		return STATUS_ERROR;
	}
	int nIndex = m_nDeviceCombo;
	if ((nIndex < 0) | (nIndex >= MV_MAX_DEVICE_NUM))
	{
		//ShowErrorMsg(TEXT("Please select device"), 0);
		return STATUS_ERROR;
	}

	// ch:由设备信息创建设备实例 | en:Device instance created by device information
	if (NULL == m_stDevList.pDeviceInfo[nIndex])
	{
		//ShowErrorMsg(TEXT("Device does not exist"), 0);
		return STATUS_ERROR;
	}

	if (NULL != m_pcMyCamera)
	{
		return STATUS_ERROR;
	}

	m_pcMyCamera = new CMyCamera;
	if (NULL == m_pcMyCamera)
	{
		return STATUS_ERROR;
	}

	int nRet = m_pcMyCamera->Open(m_stDevList.pDeviceInfo[nIndex]);
	if (MV_OK != nRet)
	{
		delete m_pcMyCamera;
		m_pcMyCamera = NULL;
		return nRet;
	}

	// ch:探测网络最佳包大小(只对GigE相机有效) | en:Detection network optimal package size(It only works for the GigE camera)
	if (m_stDevList.pDeviceInfo[nIndex]->nTLayerType == MV_GIGE_DEVICE)
	{
		int nPacketSize = m_pcMyCamera->GetOptimalPacketSize();
		if (nPacketSize > 0)
		{
			nRet = m_pcMyCamera->SetIntValue("GevSCPSPacketSize", nPacketSize);
			if (nRet != MV_OK)
			{
				//ShowErrorMsg(TEXT("Warning: Set Packet Size fail!"), nRet);
			}
		}
		else
		{
			//ShowErrorMsg(TEXT("Warning: Get Packet Size fail!"), nPacketSize);
		}
	}

	m_bOpenDevice = TRUE;

	return MV_OK;
}

// ch:关闭设备 | en:Close Device
int MyManger::CloseDevice(void)
{
	if (m_pcMyCamera)
	{
		m_pcMyCamera->Close();
		delete m_pcMyCamera;
		m_pcMyCamera = NULL;
	}

	m_bOpenDevice = FALSE;
	m_bStartGrabbing = FALSE;

	if (m_pBufForDriver)
	{
		free(m_pBufForDriver);
		m_pBufForDriver = NULL;
	}
	m_nBufSizeForDriver = 0;

	if (m_pBufForSaveImage)
	{
		free(m_pBufForSaveImage);
		m_pBufForSaveImage = NULL;
	}
	m_nBufSizeForSaveImage = 0;

	return MV_OK;
}

void MyManger::onBtnStartACQ()
{
	if (FALSE == m_bOpenDevice || TRUE == m_bStartGrabbing)
	{
		return;
	}

	int nRet = MV_OK;
	if (NULL != m_pcMyCamera)
	{
		nRet = m_pcMyCamera->StartGrabbing();
		if (nRet == MV_OK)
		{
			nRet = m_pcMyCamera->Display(m_hwndDisplay);
		}
	}
	else
	{
		return;
	}

	if (MV_OK != nRet)
	{
		return;
	}
	m_bStartGrabbing = TRUE;
	//	EnableControls(TRUE);
}
void MyManger::onBtnStopACQ()
{
	if (FALSE == m_bOpenDevice || FALSE == m_bStartGrabbing)
	{
		return;
	}

	int nRet = MV_OK;
	if (NULL != m_pcMyCamera)
	{
		nRet = m_pcMyCamera->StopGrabbing();
	}
	else
	{
		return;
	}

	if (MV_OK != nRet)
	{
		return;
	}
	m_bStartGrabbing = FALSE;
	//	EnableControls(TRUE);
}
void MyManger::onBtnTriggerOnce()
{
	if (TRUE != m_bStartGrabbing)
	{
		return;
	}

	int nRet = m_pcMyCamera->CommandExecute("TriggerSoftware");
}


void MyManger::OnClickedContinusModeRadio()
{
	//((CButton *)GetDlgItem(IDC_CONTINUS_MODE_RADIO))->SetCheck(TRUE);
	//((CButton *)GetDlgItem(IDC_TRIGGER_MODE_RADIO))->SetCheck(FALSE);
	//ui.radioButton->setChecked(TRUE);
	//ui.radioButton_2->setChecked(TRUE);
	m_nTriggerMode = MV_TRIGGER_MODE_OFF;
	int nRet = SetTriggerMode();
	if (MV_OK != nRet)
	{
		return;
	}
	//GetDlgItem(IDC_SOFTWARE_ONCE_BUTTON)->EnableWindow(FALSE);
	//ui.pushButton_triggerOnce->setEnabled(FALSE);
}

void MyManger::OnClickedTriggerModeRadio()
{
	//	UpdateData(TRUE);
	//	((CButton *)GetDlgItem(IDC_CONTINUS_MODE_RADIO))->SetCheck(FALSE);
	//	((CButton *)GetDlgItem(IDC_TRIGGER_MODE_RADIO))->SetCheck(TRUE);

	m_nTriggerMode = MV_TRIGGER_MODE_ON;
	int nRet = SetTriggerMode();
	if (MV_OK != nRet)
	{
		ShowErrorMsg(TEXT("Set Trigger Mode Fail"), nRet);
		return;
	}

	if (m_bStartGrabbing == TRUE)
	{
		if (TRUE == m_bSoftWareTriggerCheck)
		{
			//GetDlgItem(IDC_SOFTWARE_ONCE_BUTTON)->EnableWindow(TRUE);
			//ui.pushButton_triggerOnce->setEnabled(TRUE);
		}
	}
}

// ch:设置触发模式 | en:Set Trigger Mode
int MyManger::SetTriggerMode(void)
{
	int nRet = m_pcMyCamera->SetEnumValue("TriggerMode", m_nTriggerMode);
	if (MV_OK != nRet)
	{
		return nRet;
	}

	return MV_OK;
}