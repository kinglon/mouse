#include "stdafx.h"
#include "MouseCommManager.h"
#include <hidsdi.h>
#include <thread>
#include <setupapi.h>
#include <devguid.h>
#include <winioctl.h>
#include "SettingManager.h"

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

	CIcrCriticalSection cs(m_cs.GetCS());
	m_sendDatas.push(std::string((const char*)data, dataLength));
	cs.Leave();

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
		m_hDeviceHandle = CreateFile(devicePath.c_str(), GENERIC_READ | GENERIC_WRITE, FILE_SHARE_READ | FILE_SHARE_WRITE, \
			NULL, OPEN_EXISTING, FILE_FLAG_OVERLAPPED, NULL);
		if (m_hDeviceHandle == INVALID_HANDLE_VALUE)
		{
			LOG_ERROR(L"failed to open the mouse device, error is %d", GetLastError());
			std::this_thread::sleep_for(std::chrono::seconds(3));
			continue;
		}

		while (true)
		{
			if (!WriteData())
			{
				break;
			}

			if (!ReadData())
			{
				break;
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

		if (DevAttributes.VendorID == 0x1111 && DevAttributes.ProductID == 0xf && currentDevicePath.find(L"col05") != -1)
		{
			devicePath = currentDevicePath;
			break;
		}
	} while (true);

	SetupDiDestroyDeviceInfoList(DevInfo);
	return devicePath;
}

bool CMouseCommManager::ReadData()
{	
	OVERLAPPED overlapped;
	memset(&overlapped, 0, sizeof(OVERLAPPED));
	overlapped.hEvent = CreateEvent(NULL, TRUE, FALSE, NULL);

	unsigned char buffer[64];
	if (!ReadFile(m_hDeviceHandle, buffer, sizeof(buffer), nullptr, &overlapped))
	{
		if (GetLastError() != ERROR_IO_PENDING)
		{
			LOG_ERROR(L"failed to read data when calling ReadFile, error is %d", GetLastError());
			CloseHandle(overlapped.hEvent);
			CloseHandle(m_hDeviceHandle);
			m_hDeviceHandle = INVALID_HANDLE_VALUE;
			std::this_thread::sleep_for(std::chrono::seconds(3));
			return false;
		}
	}

	if (WaitForSingleObject(overlapped.hEvent, 100) != WAIT_OBJECT_0)
	{
		CancelIoEx(m_hDeviceHandle, &overlapped);
		CloseHandle(overlapped.hEvent);
		return true;
	}
	CloseHandle(overlapped.hEvent);

	DWORD bytesRead = 0;
	if (!GetOverlappedResult(m_hDeviceHandle, &overlapped, &bytesRead, false))
	{
		LOG_ERROR(L"failed to read data when calling GetOverlappedResult, error is %d", GetLastError());		
		return true;		
	}

	if (bytesRead == 0)
	{
		LOG_ERROR(L"recv a data with nothing");
	}
	else if (bytesRead > 0 && m_callback)
	{
		if (CSettingManager::GetInstance()->m_logLevel >= (int)ELogLevel::LOG_LEVEL_DEBUG)
		{
			std::wstring dataHex = ToHexChar(buffer, bytesRead);
			LOG_DEBUG(L"recv data: %s", dataHex.c_str());
		}
		m_callback->RecvDataCallback(buffer, bytesRead);
	}

	return true;
}

bool CMouseCommManager::WriteData()
{
	std::vector<std::string> sendDatas;
	CIcrCriticalSection cs(m_cs.GetCS());
	while (!m_sendDatas.empty())
	{
		sendDatas.push_back(m_sendDatas.front());
		m_sendDatas.pop();
	}
	cs.Leave();

	for (auto& data : sendDatas)
	{
		if (CSettingManager::GetInstance()->m_logLevel >= (int)ELogLevel::LOG_LEVEL_DEBUG)
		{
			std::wstring dataHex = ToHexChar((const unsigned char*)data.c_str(), data.length());
			LOG_DEBUG(L"send data: %s", dataHex.c_str());
		}

		OVERLAPPED overlapped;
		memset(&overlapped, 0, sizeof(OVERLAPPED));
		overlapped.hEvent = CreateEvent(NULL, TRUE, FALSE, NULL);		
		if (!WriteFile(m_hDeviceHandle, data.c_str(), data.length(), nullptr, &overlapped))
		{
			if (GetLastError() != ERROR_IO_PENDING)
			{
				LOG_ERROR(L"failed to send data, error is %d", GetLastError());
				CloseHandle(overlapped.hEvent);
				CloseHandle(m_hDeviceHandle);
				m_hDeviceHandle = INVALID_HANDLE_VALUE;
				std::this_thread::sleep_for(std::chrono::seconds(3));
				return false;
			}
		}

		if (WaitForSingleObject(overlapped.hEvent, 1000) != WAIT_OBJECT_0)
		{
			LOG_ERROR(L"failed to send data, error is timeout");
			CancelIoEx(m_hDeviceHandle, &overlapped);
			CloseHandle(overlapped.hEvent);
			return true;
		}
		CloseHandle(overlapped.hEvent);
	}

	return true;
}