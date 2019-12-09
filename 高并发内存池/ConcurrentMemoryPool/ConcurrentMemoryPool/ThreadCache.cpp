#pragma once
#include"ThreadCache.h"
void* ThreadCache::FechFromCentralCache(size_t index, size_t num)
{
	//
}
void* ThreadCache::Allocte(size_t size)
{
	size_t index;//Index(size);获取结点从链表下标
	FreeList& freeList = _freeLists[index];
	if (!freeList.Empty())
	{
		//表示当前结点的链表上有空间可以使用
		return freeList.Pop();
	}
	else
	{
		//链表上没有，就只能从centre cache中申请
		//将num-1个挂起来，一个返回
		size_t num = 20;//对象小需要设置多一些，对象大需要设置小一些
		return FechFromCentralCache(index, num);
	}
}
void ThreadCache::Deallocte(void* ptr, size_t size)
{
	size_t index;//根据size获取index；
	FreeList& freeList = _freeLists[index];
	freeList.Push(ptr);

	//if ()
	//{
	//用来将资源返还给centralcache。
	//	ReleaseToCentralCache();
	//}
}