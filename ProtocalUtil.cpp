#include "stdafx.h"
#include "ProtocalUtil.h"
#include "MouseCommManager.h"

// ���ݰ��̶����С
#define PACKAGE_FIXED_SIZE  6

// ����Э�����С
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
	// ����ռ�	
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

	// ���̶���
	int offset = 0;
	data[offset++] = package.m_reportId;
	data[offset++] = package.m_commandId;
	data[offset++] = (unsigned char)package.m_tlvs.size();
	data[offset++] = (unsigned char)tlvDataLength;

	// ���TLV����	
	for (const auto& tlv : package.m_tlvs)
	{
		data[offset] = tlv.m_type;
		data[offset+1] = tlv.m_length;
		memcpy(&data[offset+2], tlv.m_value, tlv.m_length - 2);
		offset += tlv.m_length;
	}

	// ����CRC�����
	UINT16 crc = CRC16CCITT(data, totalDataLength - 2);
	data[offset++] = (crc >> 8) & 0xff;
	data[offset++] = crc & 0xff;

	// ����
	bool success = CMouseCommManager::GetInstance()->SendData(data, OUTPUT_REPORT_LENGTH);
	delete[] data;

	return success;
}

bool CProtocalUtil::ParsePackage(const unsigned char* data2, int dataLength2, CProtocalPackage& package)
{	
	static int dataMaxLength = 6400; // ��󳤶�
	static int dataLength = 0; // ��ǰ���ݳ���
	static unsigned char* data = new unsigned char[dataMaxLength];

	// ����Ҫ��һ���ֽ� 0x5A
	if (data2 == nullptr || dataLength2 < 1)
	{
		LOG_ERROR(L"invalid param");
		dataLength = 0;
		return false;
	}

	// ���ݴ����������������ݻ�����
	if (dataLength + dataLength2 >= dataMaxLength)
	{
		LOG_ERROR(L"the data length is too long");
		dataLength = 0;
		return false;
	}

	// �������ݣ�Ҫ���ǲ�����
	if (dataLength == 0)
	{
		memcpy(data, data2, dataLength2);
		dataLength = dataLength2;
	}
	else
	{
		memcpy(&data[dataLength], &data2[1], dataLength2-1);  // ��һ���ֽ�0x5A��Ҫ
		dataLength += dataLength2 - 1;
	}
	
	if (dataLength < PACKAGE_FIXED_SIZE)
	{
		LOG_ERROR(L"the data length is too short");
		dataLength = 0;
		return false;
	}

	// �����̶���
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

	// crcУ��
	unsigned short crc = CRC16CCITT((unsigned char*)data, realDataLength - 2);
	unsigned short myCrc = (data[realDataLength - 2] << 8) + data[realDataLength - 1];
	if (crc != myCrc)
	{
		LOG_ERROR(L"crc is wrong, 0x%02X != 0x%02X", crc, myCrc);
		dataLength = 0;
		return false;
	}

	// ����tlv	
	const unsigned char* tlvData = &data[offset];
	offset = 0; // ÿ��TLV�Ŀ�ͷλ��
	for (int i = 0; i < tlvCount; i++)
	{
		CProtocalTLV tlv;
		if (offset + 2 > tlvDataLength)
		{
			LOG_ERROR(L"the tlv data length is less");
			dataLength = 0;
			return false;
		}
		tlv.m_type = tlvData[offset];
		tlv.m_length = tlvData[offset+1];
		
		if (offset + tlv.m_length > tlvDataLength)
		{
			LOG_ERROR(L"the tlv data length is less");
			dataLength = 0;
			return false;
		}
		memcpy(tlv.m_value, tlvData + offset + 2, tlv.m_length-2);
		offset += tlv.m_length;

		package.m_tlvs.push_back(tlv);
	}

	dataLength = 0;
	return true;
}

std::string CProtocalUtil::HexChar2Bytes(const std::string& hexChars)
{
	std::string result;
	for (size_t i = 0; i < hexChars.length(); i += 2) 
	{
		std::string byteString = hexChars.substr(i, 2);
		char ch = (char)std::stoul(byteString, nullptr, 16);
		result.push_back(ch);
	}
	return result;
}