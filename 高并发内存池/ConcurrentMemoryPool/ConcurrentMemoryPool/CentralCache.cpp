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
	Span* span = pageCacheInst.NewSpan(numpage);
	//从page cache获取
	return span;
}



size_t CentralCache::FetchRangeObj(void*& start, void*& end, size_t num, size_t size)
{
	Span* span = GetOneSpan(size);
	FreeList& freelist = span->_freelist;

	size_t actualNum = freelist.PopRange(start, end, num);
	span->_usecount += actualNum;
	//优化待处理：当当前Span对象的freelist链表为空，则将其置到spanlist最末尾。
	return actualNum;
}