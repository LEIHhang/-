#pragma once
#include"Common.h"

class ThreadCache
{
public:
	//size是申请对象的大小
	void* Allocte(size_t size);
	void Deallocte(void* ptr, size_t size);

	//从中心缓存获取内存,放到size对应的index下标的链表上，获取num个
	void* FechFromCentralCache(size_t size);
private:
	FreeList _freeLists[NFREE_LIST];//每一个结点都是一个链表
};