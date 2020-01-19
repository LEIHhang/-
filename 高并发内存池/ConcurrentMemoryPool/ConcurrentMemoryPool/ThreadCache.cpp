#pragma once
#include"ThreadCache.h"
#include"CentralCache.h"
void* ThreadCache::Allocte(size_t size)
{
	//Index(size);对齐对象，获取下标
	size_t index = SizeClass::ListIndex(size);
	//获取该大小空闲空间链表对象
	FreeList& freeList = _freeLists[index];
	if (!freeList.Empty())
	{
		//表示当前结点的链表上有空间可以使用
		//返回指向该空间的指针，并把该空间从链表中删除
		return freeList.Pop();
	}
	else
	{
		//链表上没有，就只能从centre cache中获取多个该大小，个数由需要空间大小确认
		//返回一个链表
		//因为获取的话不是单个获取，获取多个，所以这个函数需要在thread内设置
		return FechFromCentralCache(SizeClass::RoundUp(size));
	}
}
void ThreadCache::Deallocte(void* ptr, size_t size)
{
	size_t index = SizeClass::ListIndex(size);//根据size获取index；
	FreeList& freeList = _freeLists[index];
	freeList.Push(ptr);

	//if ()
	//{
	//用来将资源返还给centralcache。
	//	ReleaseToCentralCache();
	//}
}


//独立测试thread cache
//void* ThreadCache::FechFromCentralCache(size_t size)
//{
//	//模拟取内存对象的代码，测试Thread cache逻辑
//	size_t num = 20;
//	size_t index = SizeClass::ListIndex(size);
//	char* start = (char*)malloc(num * size);
//	char* cur = start;
//	for (size_t i = 0; i < num - 1; ++i)
//	{
//		char* next = cur + size;
//		NextObj(cur) = next;
//
//		cur = next;
//	}
//	NextObj(cur) = nullptr;
//
//	void* head = NextObj(start);
//	void* tail = cur;
//
//	_freeLists[index].PushRange(head, tail);
//	return start;
//}

void* ThreadCache::FechFromCentralCache(size_t size)
{
	size_t num = SizeClass::NumMoveSize(size);
	void* start = nullptr;
	void* end = nullptr;
	//actual 实际获取到的对象
	//start 和end都是输出型参数 从中心缓存获取一个链表
	int actualNum = centralCacheInst.FetchRangeObj(start, end, num, size);
	if (actualNum == 1)
	{
		return start;
	}
	else
	{
		size_t index = SizeClass::ListIndex(size);
		FreeList& freelist = _freeLists[index];
		//从中心缓存获取多个对象用pushRange插入
		freelist.PushRange(NextObj(start), end);

		return start;
	}
}