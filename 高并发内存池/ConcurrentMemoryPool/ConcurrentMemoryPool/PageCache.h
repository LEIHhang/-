#pragma once
#include"Common.h"

class PageCache
{
public:
	Span* NewSpan(size_t numpage);//获取一个numpage页的span
private:
	SpanList _spanLists[MAX_PAGES];//使用1-128
};

static PageCache pageCacheInst;