#pragma once
#ifdef _WIN32
//windowsƽ̨�ļ�
#include<iostream>
#include<WS2tcpip.h>
#include<Iphlpapi.h>
#include<vector>
#include<stdint.h>
#include <cstdlib>
#include<fstream>
#include<sstream>
#include<boost\filesystem.hpp>
#pragma comment(lib,"Iphlpapi.lib")//������̬��
#pragma comment(lib,"ws2_32.lib")
#else
//linuxƽ̨�ļ�
#endif
//����������Ϣ
class Adapter
{
public:
	//ip��ַ
	uint32_t _ip_addr;
	//��������
	uint32_t _mask_addr;
};

class AdapterTool
{
public:
#ifdef _WIN32
	//���óɾ�̬�Ŀ����ڽṹ����ֱ�ӵ���
	//����ȡ��������Ϣ������listָ���������
	static bool GetAllAdapter(std::vector<Adapter> *list)
	{
		//IP_ADAPTER_INFO��һ������������Ϣ�Ľṹ��
		//PIP_ADAPTER_INFO��ָ������ṹ���ָ��
		PIP_ADAPTER_INFO p_adapters = new IP_ADAPTER_INFO();
		uint64_t all_adapters_size = sizeof(IP_ADAPTER_INFO);
		//GetAdaptersInfo�ǻ�ȡ��ǰ�������������������Ϣ�Ľӿڣ��ڶ���������������������Ϣ��С֮�͡�
		int ret = GetAdaptersInfo(p_adapters, (PULONG)&all_adapters_size);
		//�������ֵΪERROR_BUFFER_OVERFLOW˵����С���ò��ԣ���ɻ��������
		if (ret == ERROR_BUFFER_OVERFLOW)
		{
			//���ͷ�ԭ������ռ�
			delete p_adapters;
			//�����¿��ٿռ䣬���ֽڿ���
			p_adapters = (PIP_ADAPTER_INFO)new BYTE[all_adapters_size];
			//���»�ȡ������Ϣ
			GetAdaptersInfo(p_adapters, (PULONG)&all_adapters_size);
		}
		
		while (p_adapters)
		{
			Adapter adapter;
			//�������ʮ���ơ� ��> ��������������
			inet_pton(AF_INET, p_adapters->IpAddressList.IpAddress.String, &adapter._ip_addr);
			inet_pton(AF_INET, p_adapters->IpAddressList.IpMask.String, &adapter._mask_addr);

			if (adapter._ip_addr != 0)//ȥ��û�����õ�����
			{
				list->push_back(adapter);
				//std::cout << adapter._ip_addr << std::endl;
				char s[40];
				_itoa_s(adapter._ip_addr, s, 2);
				/*printf("����i�Ķ�������Ϊ��%s\n", s);
				std::cout << "�������ƣ�" << p_adapters->AdapterName << std::endl;
				std::cout << "������Ϣ��" << p_adapters->Description << std::endl;
				std::cout << "IP��ַ��" << p_adapters->IpAddressList.IpAddress.String << std::endl;
				std::cout << "�������룺" << p_adapters->IpAddressList.IpMask.String << std::endl;
				std::cout << std::endl;*/
			}
			//PIP_ADAPTER_INFO tmp = p_adapters;
			p_adapters = p_adapters->Next;
		}
		delete p_adapters;
		list->resize(1);
		return true;
	}
#else
	bool GetAllAdapter(std::vector<Adapter> *list);
#endif
};

class FileTool
{
public:
	static int64_t GetFileSize(const std::string& file_name)
	{
		return boost::filesystem::file_size(file_name);
	}
	static bool Write(const std::string& file_name,const std::string& body,const uint32_t offset = 0)
	{
		/*std::ofstream  ofs(file_name);
		std::cout << file_name << std::endl;
		if (ofs.is_open() == false)
		{
			std::cout << "��["<<file_name.c_str()<<"]�ļ���ʧ��" << std::endl;
			return false;
		}
		ofs.seekp(offset, std::ios::beg);
		ofs.write(&body[0], body.size());
		std::cout << body << std::endl;
		if (ofs.good() == false)
		{
			std::cerr << "���ļ�д������ʧ��" << std::endl;
			ofs.close();
			return false;
		}
		std::cout << "�ļ�д��ɹ�\n";
		ofs.close();
		return true;*/
		FILE* fp = NULL;
		fopen_s(&fp, file_name.c_str(), "ab+");
		if (fp == NULL)
		{
			std::cout << "�ļ���ȡʧ��\n";
			fclose(fp);
			return false;
		}
		fseek(fp, offset, SEEK_SET);
		int ret = fwrite(&body[0], 1, body.size(), fp);
		if (ret != body.size())
		{
			std::cerr << "���ļ���ȡ����ʧ��\n";
			fclose(fp);
			return false;
		}
		fclose(fp);
		return true;
	}
	//���ļ����ݶ�ȡ����Ӧ�����У�Ҳ���ǰѴ����ϵ����ݶ�ȡ���ڴ���
	static bool Read(const std::string& file_name, std::string* body)
	{
		//std::ifstream ifs(file_name);
		//if (!ifs.is_open())
		//{
		//	std::cerr << "�ļ���ʧ��\n";
		//	ifs.close();
		//	return false;
		//}
		//int64_t filesize = boost::filesystem::file_size(file_name);
		//body->resize(filesize);
		//ifs.read(&(*body)[0], filesize);
		//if (!ifs.good())
		//{
		//	std::cerr << "�ļ�д�����\n";
		//	ifs.close();
		//	return false;
		//}
		//ifs.close();
		//return true;
		int64_t file_size = boost::filesystem::file_size(file_name);
		body->resize(file_size);
		FILE* fp = NULL;
		fopen_s(&fp, file_name.c_str(), "rb+");
		if (fp == NULL)
		{
			std::cout << "�ļ���ȡʧ��\n";
			fclose(fp);
			return false;
		}
		fseek(fp, 0, SEEK_SET);
		int64_t ret = fread(&(*body)[0], 1, file_size, fp);
		if (ret != file_size)
		{
			std::cerr << "���ļ���ȡ����ʧ��\n";
			fclose(fp);
			return false;
		}
		fclose(fp);
		return true;
	}
	static bool RangeRead(const std::string file_name, std::string* body, int64_t len, long offset)
	{
		body->resize(len);
		FILE* fp = NULL;
		fopen_s(&fp, file_name.c_str(), "rb+");
		if (fp == NULL)
		{
			std::cout << "�ֿ��ļ���ȡʧ��\n";
			fclose(fp);
			return false;
		}
		fseek(fp, offset, SEEK_SET);
		int64_t ret = fread(&(*body)[0], 1, len, fp);
		std::cout << "�ֶ��ļ���ȡ�ɹ�\n";
		if (ret != len)
		{
			std::cerr << "���ļ���ȡ����ʧ��\n";
			fclose(fp);
			return false;
		}
		fclose(fp);
		return true;
	}
};

class StringTool
{
public:
	static uint64_t String2Dig(const std::string& num)
	{
		std::stringstream tmp;
		tmp << num;
		uint64_t len;
		tmp >> len;
		return len;
	}
};

class HeaderTool
{
public:
	static bool GetRange(const std::string& range_str, int64_t* range_start, int64_t* range_end)
	{
		//������������bytes =  0 - 99
		int16_t equal_sign = range_str.find('=');
		int16_t sub_sign = range_str.find('-');
		std::cout << range_str << std::endl;
		std::cout << "equal_sign:" << equal_sign << std::endl;
		std::cout << "sub_sign:" << sub_sign << std::endl;

		*range_start = std::atol(range_str.substr(equal_sign + 1, sub_sign - equal_sign - 1).c_str());
		std::cout << "range_str.substr(equal_sign + 1, sub_sign - equal_sign - 1):" << range_str.substr(equal_sign + 1, sub_sign - equal_sign - 1) << std::endl;
		*range_end = std::atol(range_str.substr(sub_sign + 1).c_str());
		std::cout << "range_str.substr(sub_sign + 1):" << range_str.substr(sub_sign + 1) << std::endl;
		return true;
	}
};