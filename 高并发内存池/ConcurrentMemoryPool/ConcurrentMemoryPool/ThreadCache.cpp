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
		//表示在thread cache中当前结点的链表上有空间可以使用
		//返回一个该大小空间，并把该空间从链表中删除
		return freeList.Pop();
	}
	else
	{
		//thread cache中没有 就只能向central cache中获取
		//为了减少和centralcache的交互次数，一次取多个该大小，
		//个数由根据申请的空间大小确认
		//将从central获取的的空间进行切块放入当前freelist中，并返回一个给用户
		//RoundUp负责将元素大小对齐
		return FechFromCentralCache(SizeClass::RoundUp(size));
	}
}
void ThreadCache::Deallocte(void* ptr, size_t size)
{
	size_t index = SizeClass::ListIndex(size);//根据size获取index；
	FreeList& freeList = _freeLists[index];
	freeList.Push(ptr);

	//对象个数满足一定条件|内存大小过大 就会释放给cetral cache
	size_t num = SizeClass::NumMoveSize(size);
	if (freeList.Num()>= num)
	{
		//自由链表过长舍弃num个数据块
		ListTooLong(freeList, num, size);
	}
}

void ThreadCache::ListTooLong(FreeList& freeList, size_t num,size_t size)
{
	void* start = nullptr, *end = nullptr;
	freeList.PopRange(start, end, num);
	NextObj(end) = nullptr;
	CentralCache::GetInstance().ReleaseListToSpans(start,size);

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

//向centralcache获取空间
void* ThreadCache::FechFromCentralCache(size_t size)
{
	size_t num = SizeClass::NumMoveSize(size);
	void* start = nullptr;
	void* end = nullptr;
	//actual 实际获取到的对象
	//start 和end都是输出型参数 从中心缓存获取一个链表
	//ThreadCache访问CentralCache的接口处
	int actualNum = CentralCache::GetInstance().FetchRangeObj(start, end, num, size);
	if (actualNum == 1)
	{
		return start;
	}
	else
	{
		//成功获取到多个空间，返回一个，将剩余的挂载到thread cache上
		size_t index = SizeClass::ListIndex(size);
		FreeList& freelist = _freeLists[index];
		//从中心缓存获取多个对象用pushRange插入，
		freelist.PushRange(NextObj(start), end,actualNum-1);

		return start;
	}
}