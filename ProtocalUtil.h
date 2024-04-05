#pragma once

#include <vector>

// TLV
#define MAX_VALUE_LENGTH 200
class CProtocalTLV
{
public:
	// type
	unsigned char m_type = 0x00;

	// length
	unsigned char m_length = 0x00;

	// value
	unsigned char m_value[MAX_VALUE_LENGTH];
};

// 数据协议包
class CProtocalPackage
{
public:
	// Report ID
	unsigned char m_reportId = 0x00;

	// 命令
	unsigned char m_commandId = 0x00;

	// tlv列表
	std::vector<CProtocalTLV> m_tlvs;
};


class CProtocalUtil
{
public:
	CProtocalUtil();
	~CProtocalUtil();

public:
	// 发送数据包
	static bool SendPackage(const CProtocalPackage& package);

	// 解析数据包
	static bool ParsePackage(unsigned char* data, int dataLength, CProtocalPackage& package);
};

