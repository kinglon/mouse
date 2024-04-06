#include "stdafx.h"
#include "MouseCommManager.h"
#include <thread>
#include <setupapi.h>
#include <devguid.h>
#include <winioctl.h>
#include "SettingManager.h"

extern "C" {
#include <hidsdi.h> 
};

#pragma comment(lib,"setupapi.lib")
#pragma comment(lib,"Hid.lib")


CMouseCommManager::CMouseCommManager()
{
}


CMouseCommManager::~CMouseCommManager()
{
}


CMouseCommManager* CMouseCommManager::GetInstance()
{
	static CMouseCommManager* instance = new CMouseCommManager();
	return instance;
}

void CMouseCommManager::Init(IMouseCommCallback* callback)
{
	if (m_isInit)
	{
		return;
	}

	m_callback = callback;
	new std::thread(&CMouseCommManager::ThreadProc, this);

	m_isInit = true;
}

bool CMouseCommManager::IsMouseConnected()
{
	return m_hDeviceHandle != INVALID_HANDLE_VALUE;
}

std::wstring CMouseCommManager::ToHexChar(const unsigned char* pData, int nDataLength, const wchar_t splitChar)
{
	std::wstring strResult;
	for (int i = 0; i < nDataLength; i++)
	{
		if (splitChar != L'\0' && i != 0) 
		{
			strResult += splitChar;
		}

		unsigned char ch = pData[i];
		wchar_t szBuf[10];
		memset(szBuf, 0, sizeof(szBuf));
		swprintf_s(szBuf, L"%02x", (int)ch);
		strResult += szBuf;
	}

	return strResult;
}

bool CMouseCommManager::SendData(const unsigned char* data, int dataLength)
{
	if (m_hDeviceHandle == INVALID_HANDLE_VALUE)
	{
		LOG_ERROR(L"failed to send data, the mouse device not open");
		return false;
	}

	if (CSettingManager::GetInstance()->m_logLevel >= (int)ELogLevel::LOG_LEVEL_DEBUG)
	{
		std::wstring dataHex = ToHexChar(data, dataLength);
		LOG_DEBUG(L"send data: %s", dataHex.c_str());
	}

	DWORD bytesWritten;
	if (!WriteFile(m_hDeviceHandle, data, dataLength, &bytesWritten, NULL))
	{
		LOG_ERROR(L"failed to send data, error is %d", GetLastError());
		return false;
	}

	return true;
}

void CMouseCommManager::ThreadProc()
{
	LOG_INFO(L"the thread of mouse communication is running");

	while (true)
	{
		// 查找设备
		std::wstring devicePath = FindMouseDevice();
		if (devicePath.empty())
		{
			LOG_DEBUG(L"the mouse device is not found");
			std::this_thread::sleep_for(std::chrono::seconds(3));
			continue;
		}
		LOG_INFO(L"found the mouse device of %s", devicePath.c_str());

		// 打开设备
		m_hDeviceHandle = CreateFile(devicePath.c_str(), GENERIC_READ | GENERIC_WRITE, 0, NULL, OPEN_EXISTING, 0, NULL);
		if (m_hDeviceHandle == INVALID_HANDLE_VALUE)
		{
			LOG_ERROR(L"failed to open the mouse device, error is %d", GetLastError());
			std::this_thread::sleep_for(std::chrono::seconds(3));
			continue;
		}

		// 读取数据
		unsigned char buffer[10240];
		DWORD bytesRead = 0;
		while (true)
		{
			if (!ReadFile(m_hDeviceHandle, buffer, sizeof(buffer), &bytesRead, NULL))
			{
				LOG_ERROR(L"failed to read data, error is %d", GetLastError());
				CloseHandle(m_hDeviceHandle);
				m_hDeviceHandle = INVALID_HANDLE_VALUE;
				break;
			}

			if (bytesRead > 0 && m_callback)
			{
				if (CSettingManager::GetInstance()->m_logLevel >= (int)ELogLevel::LOG_LEVEL_DEBUG)
				{
					std::wstring dataHex = ToHexChar(buffer, bytesRead);
					LOG_DEBUG(L"recv data: %s", dataHex.c_str());
				}
				m_callback->RecvDataCallback(buffer, bytesRead);
			}
			std::this_thread::sleep_for(std::chrono::milliseconds(1));
		}
	}
}

std::wstring CMouseCommManager::FindMouseDevice()
{
	std::wstring devicePath;

	GUID HidGuid;
	HidD_GetHidGuid(&HidGuid);

	// Get Device Information for all present devices
	HDEVINFO DevInfo = SetupDiGetClassDevs(&HidGuid, NULL, NULL, (DIGCF_PRESENT | DIGCF_DEVICEINTERFACE));
	if (DevInfo == INVALID_HANDLE_VALUE)
	{
		LOG_ERROR(L"failed to call SetupDiGetClassDevs, error is %d", GetLastError());
		return devicePath;
	}

	/* Scan all Devices */
	int Index = -1;
	do
	{
		Index++;

		/* Device Interface Element of a Device Information set */
		SP_DEVICE_INTERFACE_DATA  DevData;
		DevData.cbSize = sizeof(DevData);
		BOOL ok = SetupDiEnumDeviceInterfaces(DevInfo, 0, &HidGuid, Index, &DevData);
		if (!ok)
		{
			break;
		}

		/* Get Device Interface Details - Get Length */
		ULONG Length = 0;
		ok = SetupDiGetDeviceInterfaceDetail(DevInfo, &DevData, NULL, 0, &Length, NULL);
		if (!ok && GetLastError() != ERROR_INSUFFICIENT_BUFFER)
		{
			LOG_ERROR(L"failed to call SetupDiGetDeviceInterfaceDetail, error is %d", GetLastError());
			continue;
		}

		/* Get Device Interface Details */
		PSP_DEVICE_INTERFACE_DETAIL_DATA DevDetail = (PSP_DEVICE_INTERFACE_DETAIL_DATA)malloc(Length);
		DevDetail->cbSize = sizeof(SP_DEVICE_INTERFACE_DETAIL_DATA);
		ok = SetupDiGetDeviceInterfaceDetail(DevInfo, &DevData, DevDetail, Length, NULL, NULL);
		if (!ok)
		{
			LOG_ERROR(L"failed to call SetupDiGetDeviceInterfaceDetail, error is %d", GetLastError());
			free(DevDetail);
			DevDetail = NULL;
			continue;
		}
		std::wstring currentDevicePath = DevDetail->DevicePath;
		LOG_DEBUG(L"find a hid device: %s", currentDevicePath.c_str());
		free(DevDetail);
		DevDetail = NULL;

		/* Create File for Device Read/Write */
		HANDLE DevHandle = CreateFile(currentDevicePath.c_str(),
			0,
			FILE_SHARE_READ | FILE_SHARE_WRITE,
			(LPSECURITY_ATTRIBUTES)NULL,
			OPEN_EXISTING,
			0,
			NULL
			);
		if (DevHandle == INVALID_HANDLE_VALUE)
		{
			// LOG_ERROR(L"failed to open device of %s, error is %d", currentDevicePath.c_str(), GetLastError());
			continue;
		}

		HIDD_ATTRIBUTES DevAttributes;
		DevAttributes.Size = sizeof(DevAttributes);
		ok = HidD_GetAttributes(DevHandle, &DevAttributes);
		CloseHandle(DevHandle);
		DevHandle = NULL;
		if (!ok)
		{
			// LOG_ERROR(L"failed to get attribute of device of %s, error is %d", currentDevicePath.c_str(), GetLastError());
			continue;
		}

		if (DevAttributes.VendorID == 0x1111 && DevAttributes.ProductID == 0xf)
		{
			devicePath = currentDevicePath;
			break;
		}
	} while (true);

	SetupDiDestroyDeviceInfoList(DevInfo);
	return devicePath;
}