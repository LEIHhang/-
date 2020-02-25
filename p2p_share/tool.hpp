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
				std::cout << adapter._ip_addr << std::endl;
				char s[40];
				_itoa_s(adapter._ip_addr, s, 2);
				printf("变量i的二进制数为：%s\n", s);
				std::cout << "网卡名称：" << p_adapters->AdapterName << std::endl;
				std::cout << "描述信息：" << p_adapters->Description << std::endl;
				std::cout << "IP地址：" << p_adapters->IpAddressList.IpAddress.String << std::endl;
				std::cout << "子网掩码：" << p_adapters->IpAddressList.IpMask.String << std::endl;
				std::cout << std::endl;
			}
			//PIP_ADAPTER_INFO tmp = p_adapters;
			p_adapters = p_adapters->Next;
		}
		delete p_adapters;
		return true;
	}
#else
	bool GetAllAdapter(std::vector<Adapter> *list);
#endif
};

class FileTool
{
public:
	static bool Write(const std::string& file_name,const std::string& body,const uint32_t offset = 0)
	{
		std::ofstream  ofs(file_name);
		if (ofs.is_open() == false)
		{
			std::cout << "打开["<<file_name.c_str()<<"]文件打开失败" << std::endl;
			return false;
		}
		ofs.seekp(offset, std::ios::beg);
		ofs.write(&body[0], body.size());
		if (ofs.good() == false)
		{
			std::cerr << "向文件写入数据失败" << std::endl;
			return false;
		}
		ofs.close();
		return true;
	}
};