#include "stdafx.h"
#include "SettingManager.h"
#include <json/json.h>
#include <fstream>
#include "Utility/ImPath.h"
#include "Utility/ImCharset.h"

CSettingManager::CSettingManager()
{
    Load();
}

CSettingManager* CSettingManager::GetInstance()
{
	static CSettingManager* pInstance = new CSettingManager();
	return pInstance;
}

void CSettingManager::Load()
{
    std::wstring strConfFilePath = CImPath::GetConfPath() + L"configs.json";
    std::ifstream inputFile(strConfFilePath);
    if (!inputFile.is_open())
    {
        LOG_ERROR(L"failed to open the basic configure file : %s", strConfFilePath.c_str());
        return;
    }

    std::string jsonString;
    std::string line;
    while (std::getline(inputFile, line))
    {
        jsonString += line;
    }
    inputFile.close();

    Json::Value root;
    Json::CharReaderBuilder builder;
    Json::CharReader* reader = builder.newCharReader();
    std::string errors;
    bool parsingSuccessful = reader->parse(jsonString.c_str(), jsonString.c_str() + jsonString.size(), &root, &errors);
    delete reader;
    if (!parsingSuccessful)
    {
        LOG_ERROR(L"failed to parse the basic configure");
        return;
    }

    if (root.isMember("log_level"))
    {
        m_logLevel = root["log_level"].asInt();
    }

	if (root.isMember("current_mouse_config"))
	{
		std::string mouseConfig = root["current_mouse_config"].asString();
		m_currentMouseConfig = CImCharset::UTF8ToUnicode(mouseConfig.c_str());
	}

	m_mouseConfigList.clear();
	if (root.isMember("mouse_config_list"))
	{
		auto& mouseConfigList = root["mouse_config_list"];
		for (unsigned int i = 0; i < mouseConfigList.size(); i++)
		{
			std::string mouseConfig = mouseConfigList[i].asString();
			m_mouseConfigList.push_back(CImCharset::UTF8ToUnicode(mouseConfig.c_str()));
		}
	}

	LoadMouseConfig(m_currentMouseConfig);
}

void CSettingManager::LoadMouseConfig(const std::wstring& configName)
{
	std::wstring strConfFilePath = CImPath::GetConfPath() + L"mouse\\" + configName + L".json";
	std::ifstream inputFile(strConfFilePath);
	if (!inputFile.is_open())
	{
		LOG_ERROR(L"failed to open the mouse configure file : %s", strConfFilePath.c_str());
		return;
	}

	std::string jsonString;
	std::string line;
	while (std::getline(inputFile, line))
	{
		jsonString += line;
	}
	inputFile.close();

	Json::Value root;
	Json::CharReaderBuilder builder;
	Json::CharReader* reader = builder.newCharReader();
	std::string errors;
	bool parsingSuccessful = reader->parse(jsonString.c_str(), jsonString.c_str() + jsonString.size(), &root, &errors);
	delete reader;
	if (!parsingSuccessful)
	{
		LOG_ERROR(L"failed to parse the mouse configure");
		return;
	}

	if (root.isMember("firstKey"))
	{
		m_mouseConfig.m_firstKey = root["firstKey"].asInt();
	}

	if (root.isMember("secondKey"))
	{
		m_mouseConfig.m_secondKey = root["secondKey"].asInt();
	}

	if (root.isMember("thirdtKey"))
	{
		m_mouseConfig.m_thirdtKey = root["thirdtKey"].asInt();
	}

	if (root.isMember("fourthKey"))
	{
		m_mouseConfig.m_fourthKey = root["fourthKey"].asInt();
	}

	if (root.isMember("fifthKey"))
	{
		m_mouseConfig.m_fifthKey = root["fifthKey"].asInt();
	}

	if (root.isMember("sixthKey"))
	{
		m_mouseConfig.m_sixthKey = root["sixthKey"].asInt();
	}

	if (root.isMember("sleepTime"))
	{
		m_mouseConfig.m_sleepTime = root["sleepTime"].asInt();
	}

	if (root.isMember("currentDpi"))
	{
		m_mouseConfig.m_currentDpi = root["currentDpi"].asInt();
	}
	
	if (root.isMember("dpiSetting"))
	{
		for (unsigned int i = 0; i < ARRAYSIZE(m_mouseConfig.m_dpiSetting) && i < root["dpiSetting"].size(); i++)
		{
			m_mouseConfig.m_dpiSetting[i].m_dpiLevel = root["dpiSetting"][i]["dpiLevel"].asInt();
			m_mouseConfig.m_dpiSetting[i].m_dpiColor = root["dpiSetting"][i]["dpiColor"].asInt();
		}
	}

	if (root.isMember("lightIndex"))
	{
		m_mouseConfig.m_lightIndex = root["lightIndex"].asInt();
	}
	
	if (root.isMember("huibaorate"))
	{
		m_mouseConfig.m_huibaorate = root["huibaorate"].asInt();
	}

	if (root.isMember("lodIndex"))
	{
		m_mouseConfig.m_lodIndex = root["lodIndex"].asInt();
	}

	if (root.isMember("qudouTime"))
	{
		m_mouseConfig.m_qudouTime = root["qudouTime"].asInt();
	}

	if (root.isMember("lineRejust"))
	{
		m_mouseConfig.m_lineRejust = root["lineRejust"].asBool();
	}

	if (root.isMember("motionSync"))
	{
		m_mouseConfig.m_motionSync = root["motionSync"].asBool();
	}

	if (root.isMember("rippleControl"))
	{
		m_mouseConfig.m_rippleControl = root["rippleControl"].asBool();
	}

	// 更新当前加载的配置名称
	m_currentMouseConfig = configName;
	Save(false);
}

void CSettingManager::SaveMouseConfig(const std::wstring& configName)
{
	Json::Value root = Json::objectValue;
	root["firstKey"] = m_mouseConfig.m_firstKey;
	root["secondKey"] = m_mouseConfig.m_secondKey;
	root["thirdtKey"] = m_mouseConfig.m_thirdtKey;
	root["fourthKey"] = m_mouseConfig.m_fourthKey;
	root["fifthKey"] = m_mouseConfig.m_fifthKey;
	root["sixthKey"] = m_mouseConfig.m_sixthKey;

	root["currentDpi"] = m_mouseConfig.m_currentDpi;
	for (auto& item : m_mouseConfig.m_dpiSetting)
	{
		Json::Value dpiSetting;
		dpiSetting["dpiLevel"] = item.m_dpiLevel;
		dpiSetting["dpiColor"] = item.m_dpiColor;
		root["dpiSetting"].append(dpiSetting);
	}

	root["sleepTime"] = m_mouseConfig.m_sleepTime;
	root["lightIndex"] = m_mouseConfig.m_lightIndex;
	root["huibaorate"] = m_mouseConfig.m_huibaorate;
	root["lodIndex"] = m_mouseConfig.m_lodIndex;
	root["qudouTime"] = m_mouseConfig.m_qudouTime;
	root["lineRejust"] = m_mouseConfig.m_lineRejust;
	root["motionSync"] = m_mouseConfig.m_motionSync;
	root["rippleControl"] = m_mouseConfig.m_rippleControl;

	std::wstring saveConfName = configName;
	if (saveConfName.empty())
	{
		saveConfName = m_currentMouseConfig;
	}
	std::wstring strConfFilePath = CImPath::GetConfPath() + L"mouse\\" + saveConfName + L".json";
	std::ofstream outputFile(strConfFilePath);
	if (outputFile.is_open())
	{
		Json::StreamWriterBuilder writer;
		std::string jsonString = Json::writeString(writer, root);
		outputFile << jsonString;
		outputFile.close();
	}
	else
	{
		LOG_ERROR(L"failed to save the mouse configure file : %s", strConfFilePath.c_str());
	}
}

void CSettingManager::Save(bool needSaveMouseConfig)
{
	if (needSaveMouseConfig)
	{
		SaveMouseConfig(m_currentMouseConfig);
	}

    Json::Value root = Json::objectValue;
	root["log_level"] = m_logLevel;
	root["current_mouse_config"] = CImCharset::UnicodeToUTF8(m_currentMouseConfig.c_str());
	for (auto& item : m_mouseConfigList)
	{
		root["mouse_config_list"].append(CImCharset::UnicodeToUTF8(item.c_str()));
	}

    std::wstring strConfFilePath = CImPath::GetConfPath() + L"configs.json";
    std::ofstream outputFile(strConfFilePath);
    if (outputFile.is_open())
    {
        Json::StreamWriterBuilder writer;
        std::string jsonString = Json::writeString(writer, root);
        outputFile << jsonString;
        outputFile.close();
    }
    else
    {
        LOG_ERROR(L"failed to save the basic configure file : %s", strConfFilePath.c_str());
    }
}