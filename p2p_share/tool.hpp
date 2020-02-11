#pragma once
#ifdef _WIN32
//windows平台文件
#include<iostream>
#include<WS2tcpip.h>
#include<Iphlpapi.h>
#include<vector>
#include<stdint.h>
#pragma comment(lib,"Iphlpapi.lib")//包含静态库
#else
//linux平台文件
#endif
class Adapter
{
public:
	uint32_t _ip_addr;
	uint32_t _mask_addr;
};

class AdapterTool
{
public:
#ifdef _WIN32
	//设置成静态的可以在结构体外直接调用
	static bool GetAllAdapter(std::vector<Adapter> *list)
	{
		//IP_ADAPTER_INFO是一个保存网卡信息的结构体
		//PIP_ADAPTER_INFO是指向这个结构体的指针
		PIP_ADAPTER_INFO p_adapters = new IP_ADAPTER_INFO();
		uint64_t all_adapters_size = sizeof(IP_ADAPTER_INFO);
		//GetAdaptersInfo是获取当前主机网卡信息接口，第二个参数返回所有网卡信息大小之和。
		int ret = GetAdaptersInfo(p_adapters, (PULONG)&all_adapters_size);
		//如果返回值为ERROR_BUFFER_OVERFLOW说明大小设置不对，造成缓冲区溢出
		if (ret == ERROR_BUFFER_OVERFLOW)
		{
			//先释放原来申请空间
			delete p_adapters;
			//再重新开辟空间
			p_adapters = (PIP_ADAPTER_INFO)new BYTE(all_adapters_size);
			//重新获取网卡信息
			GetAdaptersInfo(p_adapters, (PULONG)&all_adapters_size);
		}

		while (p_adapters)
		{
			std::cout << "网卡名称：" << p_adapters->AdapterName << std::endl;
			std::cout << "描述信息：" << p_adapters->Description << std::endl;
			std::cout << "IP地址：" << p_adapters->IpAddressList.IpAddress.String << std::endl;
			std::cout << "子网掩码：" << p_adapters->IpAddressList.IpMask.String << std::endl;

			p_adapters = p_adapters->Next;
		}
		delete p_adapters;
		return true;
	}
#else
	bool GetAllAdapter(std::vector<Adapter> *list);
#endif
};