#include"PageCache.h"

Span* PageCache::_NewSpan(size_t numpage)
{
	//_spanLists[numpage].Lock();
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
			splitspan->_pageid = span->_pageid + span->_pagesize - numpage;
			splitspan->_pagesize = numpage;
			for (PAGE_ID i = 0; i < numpage; ++i)
			{
				//切页后更改映射关系
				_idRadix.radix_tree_delete(splitspan->_pageid + i);
				_idRadix.radix_tree_insert(splitspan->_pageid + i, splitspan);
			}

			span->_pagesize -= numpage;

			//将切剩下的新span插入到pagecache里
			_spanLists[span->_pagesize].PushFront(span);

			return splitspan;
		}
	}

	//运行到这里说明当前Page cache中没有足够大小的span，找系统申请一个大页Span
	void* ptr = SystemAllocPage(MAX_PAGES - 1);

	Span* bigspan = new Span;
	bigspan->_pageid = (PAGE_ID)ptr >> PAGE_SHIFT;
	bigspan->_pagesize = MAX_PAGES - 1; //申请页的大小是128页

	for (PAGE_ID i = 0; i < bigspan->_pagesize; ++i)
	{
		//_idRadix.insert(std::make_pair(bigspan->_pageid + i, bigspan));
		_idRadix.radix_tree_insert(bigspan->_pageid + i,bigspan);
	}
	_spanLists[bigspan->_pagesize].PushFront(bigspan);

	return _NewSpan(numpage);
}
Span* PageCache::NewSpan(size_t numpage)
{
	_mutex.lock();
	Span* span = _NewSpan(numpage);
	_mutex.unlock();
	return span;
}
Span* PageCache::GetIdToSpan(PAGE_ID id)
{
	//std::map<PAGE_ID, Span*> ::iterator it = _idRadix.find(id);
	auto it = _idRadix.radix_tree_find(id);
	if (it != NULL)
	{
		return it;
	}
	else
	{
		return nullptr;
	}
}

void PageCache::ReleaseSpanToPageCache(Span* span)
{
	//页的合并
	while (1)
	{
		//获取前一个页的span，先得到前一个页的id
		PAGE_ID prevPageId = span->_pageid - 1;
		//span可能挂载page cache或者central cache中，但是只要存在就会在map中
		//根据id获取span
		auto pit = _idRadix.radix_tree_find(prevPageId);
		//前面的页不存在
		if (_idRadix.radix_tree_find(prevPageId )==NULL)
		{
			break;
		}

		//说明前一个也还在使用中，不能合并
		Span* prevSpan = pit;
		if (prevSpan->_usecount != 0)
		{
			break;
		}

		if (span->_pagesize + prevSpan->_pagesize >= MAX_PAGES)
			break;
		//合并
		span->_pageid = prevSpan->_pageid;
		span->_pagesize += prevSpan->_pagesize;
		for (PAGE_ID i = 0; i < prevSpan->_pagesize; ++i)
		{
			//映射到新的span上
			_idRadix.radix_tree_insert(prevSpan->_pageid + i,span);
		}
		//要先在page cache中删除该span，否则直接删除会有野指针。
		_spanLists[prevSpan->_pagesize].Erase(prevSpan);
		//每一个span都是在NewSpan函数中new出来的，所以要delete
		
		delete prevSpan;
	}
	//向后合并
	while (1)
	{
		PAGE_ID nextPageId = span->_pageid + span->_pagesize;
		auto nextIt = _idRadix.radix_tree_find(nextPageId);
		if (nextIt == NULL)
		{
			break;
		}
		Span* nextSpan = nextIt;
		if (nextSpan->_usecount != 0)
		{
			break;
		}
		if (span->_pagesize + nextSpan->_pagesize >= MAX_PAGES)
			break;
		span->_pagesize += nextSpan->_pagesize;
		for (PAGE_ID i = 0; i < nextSpan->_pagesize; ++i)
		{
			_idRadix.radix_tree_insert(nextSpan->_pageid + i,span);
		}
		_spanLists[nextSpan->_pagesize].Erase(nextSpan);
		delete nextSpan;
	}

	//将合并好的span挂载page cache上
	_spanLists[span->_pagesize].PushFront(span);
}
