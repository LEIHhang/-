#pragma once
#ifdef _WIN32
//windowsƽ̨�ļ�
#include<iostream>
#include<WS2tcpip.h>
#include<Iphlpapi.h>
#include<vector>
#include<stdint.h>
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
				std::cout << adapter._ip_addr << std::endl;
				std::cout << "�������ƣ�" << p_adapters->AdapterName << std::endl;
				std::cout << "������Ϣ��" << p_adapters->Description << std::endl;
				std::cout << "IP��ַ��" << p_adapters->IpAddressList.IpAddress.String << std::endl;
				std::cout << "�������룺" << p_adapters->IpAddressList.IpMask.String << std::endl;
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