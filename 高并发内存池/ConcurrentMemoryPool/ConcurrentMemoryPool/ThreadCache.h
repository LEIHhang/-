#pragma once
#include"Common.h"

class ThreadCache
{
public:
	//size���������Ĵ�С
	void* Allocte(size_t size);
	void Deallocte(void* ptr, size_t size);

	//�����Ļ����ȡ�ڴ�,�ŵ�size��Ӧ��index�±�������ϣ���ȡnum��
	void* FechFromCentralCache(size_t size);
	//����������������ݿ��������һ�����Ⱦ�Ҫ���ظ�central cache
	void ListTooLong(FreeList& freeList, size_t num,size_t size);
private:
	FreeList _freeLists[NFREE_LIST];//ÿһ����㶼��һ��������(FreeList)
};

//����ÿ���̶߳���ľ�̬ȫ�ֱ���
_declspec (thread) static ThreadCache* tlsThreadCache = nullptr;
//static ThreadCache tlsThreadCache;