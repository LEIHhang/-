#pragma once
#ifdef _WIN32
//windows平台文件
#include<iostream>
#include<WS2tcpip.h>
#include<Iphlpapi.h>
#include<vector>
#include<stdint.h>
#include <cstdlib>
#include<fstream>
#include<sstream>
#include<boost\filesystem.hpp>
#pragma comment(lib,"Iphlpapi.lib")//包含静态库
#pragma comment(lib,"ws2_32.lib")
#else
//linux平台文件
#endif
//定义网卡信息
class Adapter
{
public:
	//ip地址
	uint32_t _ip_addr;
	//子网掩码
	uint32_t _mask_addr;
};

class AdapterTool
{
public:
#ifdef _WIN32
	//设置成静态的可以在结构体外直接调用
	//将获取的网卡信息保存在list指向的数组中
	static bool GetAllAdapter(std::vector<Adapter> *list)
	{
		//IP_ADAPTER_INFO是一个保存网卡信息的结构体
		//PIP_ADAPTER_INFO是指向这个结构体的指针
		PIP_ADAPTER_INFO p_adapters = new IP_ADAPTER_INFO();
		uint64_t all_adapters_size = sizeof(IP_ADAPTER_INFO);
		//GetAdaptersInfo是获取当前计算机所有主机网卡信息的接口，第二个参数返回所有网卡信息大小之和。
		int ret = GetAdaptersInfo(p_adapters, (PULONG)&all_adapters_size);
		//如果返回值为ERROR_BUFFER_OVERFLOW说明大小设置不对，造成缓冲区溢出
		if (ret == ERROR_BUFFER_OVERFLOW)
		{
			//先释放原来申请空间
			delete p_adapters;
			//再重新开辟空间，按字节开辟
			p_adapters = (PIP_ADAPTER_INFO)new BYTE[all_adapters_size];
			//重新获取网卡信息
			GetAdaptersInfo(p_adapters, (PULONG)&all_adapters_size);
		}
		
		while (p_adapters)
		{
			Adapter adapter;
			//将“点分十进制” －> “二进制整数”
			inet_pton(AF_INET, p_adapters->IpAddressList.IpAddress.String, &adapter._ip_addr);
			inet_pton(AF_INET, p_adapters->IpAddressList.IpMask.String, &adapter._mask_addr);

			if (adapter._ip_addr != 0)//去掉没有启用的网卡
			{
				list->push_back(adapter);
				//std::cout << adapter._ip_addr << std::endl;
				char s[40];
				_itoa_s(adapter._ip_addr, s, 2);
				/*printf("变量i的二进制数为：%s\n", s);
				std::cout << "网卡名称：" << p_adapters->AdapterName << std::endl;
				std::cout << "描述信息：" << p_adapters->Description << std::endl;
				std::cout << "IP地址：" << p_adapters->IpAddressList.IpAddress.String << std::endl;
				std::cout << "子网掩码：" << p_adapters->IpAddressList.IpMask.String << std::endl;
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
			std::cout << "打开["<<file_name.c_str()<<"]文件打开失败" << std::endl;
			return false;
		}
		ofs.seekp(offset, std::ios::beg);
		ofs.write(&body[0], body.size());
		std::cout << body << std::endl;
		if (ofs.good() == false)
		{
			std::cerr << "向文件写入数据失败" << std::endl;
			ofs.close();
			return false;
		}
		std::cout << "文件写入成功\n";
		ofs.close();
		return true;*/
		FILE* fp = NULL;
		fopen_s(&fp, file_name.c_str(), "ab+");
		if (fp == NULL)
		{
			std::cout << "文件读取失败\n";
			fclose(fp);
			return false;
		}
		fseek(fp, offset, SEEK_SET);
		int ret = fwrite(&body[0], 1, body.size(), fp);
		if (ret != body.size())
		{
			std::cerr << "从文件读取数据失败\n";
			fclose(fp);
			return false;
		}
		fclose(fp);
		return true;
	}
	//把文件内容读取到响应正文中，也就是把磁盘上的数据读取到内存上
	static bool Read(const std::string& file_name, std::string* body)
	{
		//std::ifstream ifs(file_name);
		//if (!ifs.is_open())
		//{
		//	std::cerr << "文件打开失败\n";
		//	ifs.close();
		//	return false;
		//}
		//int64_t filesize = boost::filesystem::file_size(file_name);
		//body->resize(filesize);
		//ifs.read(&(*body)[0], filesize);
		//if (!ifs.good())
		//{
		//	std::cerr << "文件写入错误\n";
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
			std::cout << "文件读取失败\n";
			fclose(fp);
			return false;
		}
		fseek(fp, 0, SEEK_SET);
		int64_t ret = fread(&(*body)[0], 1, file_size, fp);
		if (ret != file_size)
		{
			std::cerr << "从文件读取数据失败\n";
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
			std::cout << "分块文件读取失败\n";
			fclose(fp);
			return false;
		}
		fseek(fp, offset, SEEK_SET);
		int64_t ret = fread(&(*body)[0], 1, len, fp);
		std::cout << "分段文件读取成功\n";
		if (ret != len)
		{
			std::cerr << "从文件读取数据失败\n";
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
		//处理这种数据bytes =  0 - 99
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