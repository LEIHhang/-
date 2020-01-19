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
	//将从page cache获取的span插入到central cache上
	spanlist.PushFront(span);
	
	return span;
}



size_t CentralCache::FetchRangeObj(void*& start, void*& end, size_t num, size_t size)
{
	Span* span = GetOneSpan(size);
	FreeList& freelist = span->_freelist;

	//start和end是输出形参数，在此函数内将分配给Thread cache的空间用start 和 end 指向头和尾。
	size_t actualNum = freelist.PopRange(start, end, num);
	span->_usecount += actualNum;
	//优化待处理：当当前Span对象的freelist链表为空，则将其置到spanlist最末尾。
	return actualNum;
}