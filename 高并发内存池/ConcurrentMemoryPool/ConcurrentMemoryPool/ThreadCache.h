#pragma once
#include"Common.h"

class ThreadCache
{
public:
	//size���������Ĵ�С
	void* Allocte(size_t size);
	void Deallocte(void* ptr, size_t size);

	//�����Ļ����ȡ�ڴ�,�ŵ�index�±�������ϣ���ȡnum��
	void* FechFromCentralCache(size_t index,size_t num);
private:
	FreeList _freeLists[NFREE_LIST];//ÿһ����㶼��һ������
};