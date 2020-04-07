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
	//如果自由链表中数据块个数超过一定长度就要返回给central cache
	void ListTooLong(FreeList& freeList, size_t num,size_t size);
private:
	FreeList _freeLists[NFREE_LIST];//每一个结点都是一个链表类(FreeList)
};

//定义每个线程独享的静态全局变量
_declspec (thread) static ThreadCache* tlsThreadCache = nullptr;
//static ThreadCache tlsThreadCache;