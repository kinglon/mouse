#pragma once

class IMouseCommCallback
{
public:
	// 接收数据
	virtual void RecvDataCallback(unsigned char* data, int dataLength) = 0;
};

class CMouseCommManager
{
protected:
	CMouseCommManager();
	~CMouseCommManager();

public:
	static CMouseCommManager* GetInstance();

	// 初始化，调用其他接口之前先调用一次
	void Init(IMouseCommCallback* callback);

	// 查询鼠标是否连接
	bool IsMouseConnected();

	// 发送数据
	// @param data, 数据
	// @param dataLength, 数据长度
	// @return 发送成功或失败
	bool SendData(const unsigned char* data, int dataLength);

	// 内存块以16进制的字符串形式输出
	std::wstring ToHexChar(const unsigned char* pData, int nDataLength, const wchar_t splitChar=L'\0');

private:
	void ThreadProc();

	// 查找鼠标设备，返回设备路径
	std::wstring FindMouseDevice();

private:
	bool m_isInit = false;

	IMouseCommCallback* m_callback = nullptr;

	HANDLE m_hDeviceHandle = INVALID_HANDLE_VALUE;
};

