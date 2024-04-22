#include "stdafx.h"
#include "ProtocalUtil.h"
#include "MouseCommManager.h"

// 数据包固定域大小
#define PACKAGE_FIXED_SIZE  6

// 数据协议包大小
#define OUTPUT_REPORT_LENGTH	64


UINT16 CProtocalUtil::CRC16CCITT(UCHAR * pszBuf, UINT unLength)
{

	UINT32 i, j;
	UINT16 CrcReg = 0xFFFF;
	UINT16 CurVal;

	for (i = 0; i < unLength; i++)
	{
		CurVal = pszBuf[i] << 8;

		for (j = 0; j < 8; j++)
		{
			if ((short)(CrcReg ^ CurVal) < 0)
				CrcReg = (CrcReg << 1) ^ 0x1021;
			else
				CrcReg <<= 1;
			CurVal <<= 1;
		}
	}

	return CrcReg;
}


bool CProtocalUtil::SendPackage(const CProtocalPackage& package)
{
	// 分配空间	
	int tlvDataLength = 0;
	for (const auto& tlv : package.m_tlvs)
	{
		tlvDataLength += (int)tlv.m_length;
	}	

	int totalDataLength = PACKAGE_FIXED_SIZE + tlvDataLength;
	if (totalDataLength > OUTPUT_REPORT_LENGTH)
	{
		LOG_ERROR(L"the size of the package is greater than %d", OUTPUT_REPORT_LENGTH);
		return false;
	}
	unsigned char* data = new unsigned char[OUTPUT_REPORT_LENGTH];
	memset(data, 0, OUTPUT_REPORT_LENGTH);

	// 填充固定域
	int offset = 0;
	data[offset++] = package.m_reportId;
	data[offset++] = package.m_commandId;
	data[offset++] = (unsigned char)package.m_tlvs.size();
	data[offset++] = (unsigned char)tlvDataLength;

	// 填充TLV数据	
	for (const auto& tlv : package.m_tlvs)
	{
		data[offset] = tlv.m_type;
		data[offset+1] = tlv.m_length;
		memcpy(&data[offset+2], tlv.m_value, tlv.m_length - 2);
		offset += tlv.m_length;
	}

	// 计算CRC并填充
	UINT16 crc = CRC16CCITT(data, totalDataLength - 2);
	data[offset++] = (crc >> 8) & 0xff;
	data[offset++] = crc & 0xff;

	// 发送
	bool success = CMouseCommManager::GetInstance()->SendData(data, OUTPUT_REPORT_LENGTH);
	delete[] data;

	return success;
}

bool CProtocalUtil::ParsePackage(const unsigned char* data2, int dataLength2, CProtocalPackage& package)
{	
	static int dataMaxLength = 6400; // 最大长度
	static int dataLength = 0; // 当前数据长度
	static unsigned char* data = new unsigned char[dataMaxLength];

	// 至少要有一个字节 0x5A
	if (data2 == nullptr || dataLength2 < 1)
	{
		LOG_ERROR(L"invalid param");
		dataLength = 0;
		return false;
	}

	// 数据传输有误，重置下数据缓冲区
	if (dataLength + dataLength2 >= dataMaxLength)
	{
		LOG_ERROR(L"the data length is too long");
		dataLength = 0;
		return false;
	}

	// 拷贝数据，要考虑拆包情况
	if (dataLength == 0)
	{
		memcpy(data, data2, dataLength2);
		dataLength = dataLength2;
	}
	else
	{
		memcpy(&data[dataLength], &data2[1], dataLength2-1);  // 第一个字节0x5A不要
		dataLength += dataLength2 - 1;
	}
	
	if (dataLength < PACKAGE_FIXED_SIZE)
	{
		LOG_ERROR(L"the data length is too short");
		dataLength = 0;
		return false;
	}

	// 解析固定域
	int offset = 0;
	package.m_reportId = data[offset++];
	package.m_commandId = data[offset++];
	int tlvCount = (int)data[offset++];
	unsigned char tlvDataLength = data[offset++];
	int realDataLength = tlvDataLength + PACKAGE_FIXED_SIZE;
	if (realDataLength > dataLength)
	{
		LOG_INFO(L"total length is %d, recv length is %d", realDataLength, dataLength);
		return false;
	}

	// crc校验
	unsigned short crc = CRC16CCITT((unsigned char*)data, realDataLength - 2);
	unsigned short myCrc = (data[realDataLength - 2] << 8) + data[realDataLength - 1];
	if (crc != myCrc)
	{
		LOG_ERROR(L"crc is wrong, 0x%02X != 0x%02X", crc, myCrc);
		dataLength = 0;
		return false;
	}

	// 解析tlv	
	const unsigned char* tlvData = &data[offset];
	offset = 0;
	for (int i = 0; i < tlvCount; i++)
	{
		CProtocalTLV tlv;
		if (offset + 2 > tlvDataLength)
		{
			LOG_ERROR(L"the tlv data length is less");
			dataLength = 0;
			return false;
		}
		tlv.m_type = tlvData[offset++];
		tlv.m_length = tlvData[offset++];
		
		if (offset + tlv.m_length > tlvDataLength)
		{
			LOG_ERROR(L"the tlv data length is less");
			dataLength = 0;
			return false;
		}
		memcpy(tlv.m_value, tlvData + offset, tlv.m_length);
		offset += tlv.m_length;

		package.m_tlvs.push_back(tlv);
	}

	dataLength = 0;
	return true;
}