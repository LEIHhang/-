#include"PageCache.h"

Span* PageCache::NewSpan(size_t numpage)
{
	if (!_spanLists[numpage].Empty())
	{
		Span* span = _spanLists[numpage].Begin();
		_spanLists[numpage].PopFront();
		return span;
	}
	for (int i = numpage + 1; i < MAX_PAGES; ++i)
	{
		if (!_spanLists[i].Empty())
		{
			//分裂申请到的大Span
			Span* span = _spanLists[i].Begin();
			_spanLists[i].PopFront();

			//定义切开剩下的span
			Span* splitspan = new Span;
			splitspan->_pageid = span->_pageid + numpage;
			splitspan->_pagesize = span->_pagesize - numpage;

			//将切剩下的新span插入到pagecache里
			_spanLists[splitspan->_pagesize].PushFront(splitspan);

			span->_pagesize = numpage;
			return span;
		}
	}

	//运行到这里说明当前Page cache中没有足够大小的span，找系统申请一个大页Span
	void* ptr = SystemAllocPage(MAX_PAGES - 1);

	Span* bigspan = new Span;
	bigspan->_pageid = (PAGE_ID)ptr >> PAGE_SHIFT;
	bigspan->_pagesize = MAX_PAGES - 1; //申请页的大小是128页
	_spanLists[bigspan->_pagesize].PushFront(bigspan);

	return NewSpan(numpage);

}
