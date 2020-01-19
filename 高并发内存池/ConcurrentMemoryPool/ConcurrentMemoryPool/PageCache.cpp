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
			//�������뵽�Ĵ�Span
			Span* span = _spanLists[i].Begin();
			_spanLists[i].PopFront();

			//�����п�ʣ�µ�span
			Span* splitspan = new Span;
			splitspan->_pageid = span->_pageid + numpage;
			splitspan->_pagesize = span->_pagesize - numpage;

			//����ʣ�µ���span���뵽pagecache��
			_spanLists[splitspan->_pagesize].PushFront(splitspan);

			span->_pagesize = numpage;
			return span;
		}
	}

	//���е�����˵����ǰPage cache��û���㹻��С��span����ϵͳ����һ����ҳSpan
	void* ptr = SystemAllocPage(MAX_PAGES - 1);

	Span* bigspan = new Span;
	bigspan->_pageid = (PAGE_ID)ptr >> PAGE_SHIFT;
	bigspan->_pagesize = MAX_PAGES - 1; //����ҳ�Ĵ�С��128ҳ
	_spanLists[bigspan->_pagesize].PushFront(bigspan);

	return NewSpan(numpage);

}
