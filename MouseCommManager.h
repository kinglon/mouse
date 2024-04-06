#pragma once

class IMouseCommCallback
{
public:
	// ��������
	virtual void RecvDataCallback(unsigned char* data, int dataLength) = 0;
};

class CMouseCommManager
{
protected:
	CMouseCommManager();
	~CMouseCommManager();

public:
	static CMouseCommManager* GetInstance();

	// ��ʼ�������������ӿ�֮ǰ�ȵ���һ��
	void Init(IMouseCommCallback* callback);

	// ��ѯ����Ƿ�����
	bool IsMouseConnected();

	// ��������
	// @param data, ����
	// @param dataLength, ���ݳ���
	// @return ���ͳɹ���ʧ��
	bool SendData(const unsigned char* data, int dataLength);

	// �ڴ����16���Ƶ��ַ�����ʽ���
	std::wstring ToHexChar(const unsigned char* pData, int nDataLength, const wchar_t splitChar=L'\0');

private:
	void ThreadProc();

	// ��������豸�������豸·��
	std::wstring FindMouseDevice();

private:
	bool m_isInit = false;

	IMouseCommCallback* m_callback = nullptr;

	HANDLE m_hDeviceHandle = INVALID_HANDLE_VALUE;
};

