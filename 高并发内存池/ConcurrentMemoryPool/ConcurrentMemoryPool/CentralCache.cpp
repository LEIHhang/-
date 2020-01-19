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

	//���������һ���Ż��������ǰSpan�е�_freelistΪ�գ��Ͱ������뵽spanlist���һ��λ�á�
	//������ͷɾ��Ȼ����β��
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
	//��page cache��ȡ
	Span* span = pageCacheInst.NewSpan(numpage);
	//��span�����гɶ�Ӧ��С�ҵ�span��freelist��
	char* start = (char*)(span->_pageid << 12);
	char* end = start + (span->_pagesize << 12);
	while (start < end)
	{
		char* obj = start;
		start += size;

		span->_freelist.Push(obj);
	}
	//����page cache��ȡ��span���뵽central cache��
	spanlist.PushFront(span);
	
	return span;
}



size_t CentralCache::FetchRangeObj(void*& start, void*& end, size_t num, size_t size)
{
	Span* span = GetOneSpan(size);
	FreeList& freelist = span->_freelist;

	//start��end������β������ڴ˺����ڽ������Thread cache�Ŀռ���start �� end ָ��ͷ��β��
	size_t actualNum = freelist.PopRange(start, end, num);
	span->_usecount += actualNum;
	//�Ż�����������ǰSpan�����freelist����Ϊ�գ������õ�spanlist��ĩβ��
	return actualNum;
}