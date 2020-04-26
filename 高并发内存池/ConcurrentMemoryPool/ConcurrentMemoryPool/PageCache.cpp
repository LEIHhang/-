#include"PageCache.h"

Span* PageCache::_NewSpan(size_t numpage)
{
	//��ȡnumpageҳ��span
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
			//��ȡ�����ҳ��
			splitspan->_pageid = span->_pageid + span->_pagesize - numpage;
			splitspan->_pagesize = numpage;
			for (PAGE_ID i = 0; i < numpage; ++i)
			{
				//_idRadix�����spanid��span��ӳ���ϵ��Ŀ��������ĵ���span��Ҫ�ָ�ͺϲ���
				//��ҳ�����ӳ���ϵ
				_idRadix.Delete(splitspan->_pageid + i);
				_idRadix.insert(splitspan->_pageid + i, splitspan);
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
		//����ӳ���ϵ
		_idRadix.insert(bigspan->_pageid + i,bigspan);
	}
	//��PageCache����һ����СΪpagesize��span
	_spanLists[bigspan->_pagesize].PushFront(bigspan);

	return _NewSpan(numpage);
}
Span* PageCache::NewSpan(size_t numpage)
{
	//����PageCacheҲ��Ҫ����
	_mutex.lock();
	Span* span = _NewSpan(numpage);
	_mutex.unlock();
	return span;
}
Span* PageCache::GetIdToSpan(PAGE_ID id)
{
	//std::map<PAGE_ID, Span*> ::iterator it = _idRadix.find(id);
	auto it = _idRadix.find(id);
	if (it != nullptr&& it->value!=nullptr)
	{
		//printf("�ҵ�pageid%d\n", id);
		return it->value;
	}
	else
	{
		//printf("Ҫ����pageid%d\n", id);
		//cout << "��ȡʧ��" << endl;
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
		auto pit = _idRadix.find(prevPageId);
		//ǰ���ҳ������
		if (pit==nullptr||pit->value==nullptr)
		{
			break;
		}

		//˵��ǰһ��Ҳ����ʹ���У����ܺϲ�
		Span* prevSpan = pit->value;
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
			//cout << "��ǰ�ϲ�" << endl;
			//printf("ɾ��pageid:%d\n", prevSpan->_pageid + i);
			_idRadix.Delete(prevSpan->_pageid + i);
			//printf("����pageid:%d\n", prevSpan->_pageid + i);
			_idRadix.insert(prevSpan->_pageid + i,span);
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
		auto nextIt = _idRadix.find(nextPageId);
		if (nextIt == nullptr || nextIt->value == nullptr)
		{
			break;
		}
		Span* nextSpan = nextIt->value;
		if (nextSpan->_usecount != 0)
		{
			break;
		}
		if (span->_pagesize + nextSpan->_pagesize >= MAX_PAGES)
			break;
		span->_pagesize += nextSpan->_pagesize;
		for (PAGE_ID i = 0; i < nextSpan->_pagesize; ++i)
		{
			//cout << "���ϲ�" << endl;
			//printf("ɾ��pageid:%d\n", nextSpan->_pageid + i);
			_idRadix.Delete(nextSpan->_pageid + i);
			//printf("����pageid:%d\n", nextSpan->_pageid + i);
			_idRadix.insert(nextSpan->_pageid + i,span);
		}
		_spanLists[nextSpan->_pagesize].Erase(nextSpan);
		delete nextSpan;
	}

	//���ϲ��õ�span����page cache��
	_spanLists[span->_pagesize].PushFront(span);
}
