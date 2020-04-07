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
			//�������뵽�Ĵ�Span
			Span* span = _spanLists[i].Begin();
			_spanLists[i].PopFront();

			//�����п�ʣ�µ�span
			Span* splitspan = new Span;
			splitspan->_pageid = span->_pageid + span->_pagesize - numpage;
			splitspan->_pagesize = numpage;
			for (PAGE_ID i = 0; i < numpage; ++i)
			{
				//��ҳ�����ӳ���ϵ
				_idRadix.radix_tree_delete(splitspan->_pageid + i);
				_idRadix.radix_tree_insert(splitspan->_pageid + i, splitspan);
			}

			span->_pagesize -= numpage;

			//����ʣ�µ���span���뵽pagecache��
			_spanLists[span->_pagesize].PushFront(span);

			return splitspan;
		}
	}

	//���е�����˵����ǰPage cache��û���㹻��С��span����ϵͳ����һ����ҳSpan
	void* ptr = SystemAllocPage(MAX_PAGES - 1);

	Span* bigspan = new Span;
	bigspan->_pageid = (PAGE_ID)ptr >> PAGE_SHIFT;
	bigspan->_pagesize = MAX_PAGES - 1; //����ҳ�Ĵ�С��128ҳ

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
	//ҳ�ĺϲ�
	while (1)
	{
		//��ȡǰһ��ҳ��span���ȵõ�ǰһ��ҳ��id
		PAGE_ID prevPageId = span->_pageid - 1;
		//span���ܹ���page cache����central cache�У�����ֻҪ���ھͻ���map��
		//����id��ȡspan
		auto pit = _idRadix.radix_tree_find(prevPageId);
		//ǰ���ҳ������
		if (_idRadix.radix_tree_find(prevPageId )==NULL)
		{
			break;
		}

		//˵��ǰһ��Ҳ����ʹ���У����ܺϲ�
		Span* prevSpan = pit;
		if (prevSpan->_usecount != 0)
		{
			break;
		}

		if (span->_pagesize + prevSpan->_pagesize >= MAX_PAGES)
			break;
		//�ϲ�
		span->_pageid = prevSpan->_pageid;
		span->_pagesize += prevSpan->_pagesize;
		for (PAGE_ID i = 0; i < prevSpan->_pagesize; ++i)
		{
			//ӳ�䵽�µ�span��
			_idRadix.radix_tree_insert(prevSpan->_pageid + i,span);
		}
		//Ҫ����page cache��ɾ����span������ֱ��ɾ������Ұָ�롣
		_spanLists[prevSpan->_pagesize].Erase(prevSpan);
		//ÿһ��span������NewSpan������new�����ģ�����Ҫdelete
		
		delete prevSpan;
	}
	//���ϲ�
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

	//���ϲ��õ�span����page cache��
	_spanLists[span->_pagesize].PushFront(span);
}
