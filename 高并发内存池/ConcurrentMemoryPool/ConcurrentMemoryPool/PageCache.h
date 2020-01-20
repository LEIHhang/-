#pragma once
#include"Common.h"

class PageCache
{
public:
	Span* NewSpan(size_t numpage);//获取一个numpage页的span
	Span* GetIdToSpan(PAGE_ID id);//根据ID获取Span
	void ReleaseSpanToPageCache(Span* span);
private:
	SpanList _spanLists[MAX_PAGES];//使用1-128
	std::map<PAGE_ID, Span*> _idSpanMap;
	
};

static PageCache pageCacheInst;