#pragma once
#include"CentralCache.h"
#include"PageCache.h"
Span* CentralCache::GetOneSpan(size_t size)
{
	size_t index = SizeClass::ListIndex(size);
	//SpanList类中私有成员指向第一个Span对象，Span类中有指向下一个Span类型的指针
	//Begin和End只是封装了接口
	SpanList& spanlist = _spanlists[index];
	Span* it = spanlist.Begin();

	//这里可以做一个优化，如果当前Span中的_freelist为空，就把他插入到spanlist最后一个位置。
	//可以先头删，然后再尾插
	while (it != spanlist.End())
	{
		if (!it->_freelist.Empty())
		{
			return it;
		}
		else
		{
			it = it->_next;
		}
	}
	size_t numpage = SizeClass::NumMovePage(size);
	//从page cache获取
	Span* span = pageCacheInst.NewSpan(numpage);
	//把span对象切成对应大小挂到span的freelist中
	char* start = (char*)(span->_pageid << 12);
	char* end = start + (span->_pagesize << 12);
	while (start < end)
	{
		char* obj = start;
		start += size;

		span->_freelist.Push(obj);
	}
	//设置span里自由链表啊对象大小
	span->_objsize = size;
	//将从page cache获取的span插入到central cache上
	spanlist.PushFront(span);
	
	return span;
}


size_t CentralCache::FetchRangeObj(void*& start, void*& end, size_t num, size_t size)
{

	size_t index = SizeClass::ListIndex(size);
	SpanList& spanlist = _spanlists[index];
	spanlist.Lock();

	//GetOneSpan从central cache和Page获取需要的Span并返回
	Span* span = GetOneSpan(size);
	//获取 返回的Span上的自由链表
	FreeList& freelist = span->_freelist;

	//start和end是输出形参数，在此函数内将分配给Thread cache的空间用start 和 end 指向头和尾。
	//获取自由链表上的内存块，并用start 和 end指向头尾。
	size_t actualNum = freelist.PopRange(start, end, num);
	span->_usecount += actualNum;
	//优化待处理：当当前Span对象的freelist链表为空，则将其置到spanlist最末尾。

	spanlist.Unlock();
	return actualNum;
}

void CentralCache::ReleaseListToSpans(void* start,size_t size)
{
	size_t index = SizeClass::ListIndex(size);
	SpanList& spanlist = _spanlists[index];
	spanlist.Lock();

	while (start)
	{
		void* next = NextObj(start);
		//计算页号
		PAGE_ID id = (PAGE_ID)start >> PAGE_SHIFT;
		Span* span = pageCacheInst.GetIdToSpan(id);
		span->_freelist.Push(start);
		span->_usecount--;
		//表示当前span切出去的对象全部返回，可以将SPan还给page cache，进行合并，减少内存碎片。
		if (span->_usecount == 0)
		{
			size_t index = SizeClass::ListIndex(span->_objsize);
			_spanlists[index].Erase(span);
			span->_freelist.Clear();
			pageCacheInst.ReleaseSpanToPageCache(span);
		}
		start = next;
	}

	spanlist.Unlock();
}