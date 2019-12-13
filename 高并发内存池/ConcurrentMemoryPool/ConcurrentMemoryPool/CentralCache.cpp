#pragma once
#include"CentralCache.h"
#include"PageCache.h"
Span* CentralCache::GetOneSpan(size_t size)
{
	size_t index = SizeClass::ListIndex(size);
	//SpanList����˽�г�Աָ���һ��Span����Span������ָ����һ��Span���͵�ָ��
	//Begin��Endֻ�Ƿ�װ�˽ӿ�
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
	//��page cache��ȡ
	return span;
}



size_t CentralCache::FetchRangeObj(void*& start, void*& end, size_t num, size_t size)
{
	Span* span = GetOneSpan(size);
	FreeList& freelist = span->_freelist;

	size_t actualNum = freelist.PopRange(start, end, num);
	span->_usecount += actualNum;
	//�Ż�����������ǰSpan�����freelist����Ϊ�գ������õ�spanlist��ĩβ��
	return actualNum;
}