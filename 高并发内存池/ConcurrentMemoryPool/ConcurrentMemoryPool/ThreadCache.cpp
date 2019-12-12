#pragma once
#include"ThreadCache.h"
#include"CentralCache.h"
void* ThreadCache::Allocte(size_t size)
{
	//Index(size);获取结点从链表下标
	size_t index = SizeClass::ListIndex(size);
	FreeList& freeList = _freeLists[index];
	if (!freeList.Empty())
	{
		//表示当前结点的链表上有空间可以使用
		//返回这个删除空间的指针
		return freeList.Pop();
	}
	else
	{
		//链表上没有，就只能从centre cache中申请
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
	int actualNum = centralCacheInst.FetchRangeObj(start, end, num, size);
	if (actualNum == 1)
	{
		return start;
	}
	else
	{
		size_t index = SizeClass::ListIndex(size);
		FreeList& freelist = _freeLists[index];
		freelist.PushRange(NextObj(start), end);
		return start;
	}
}