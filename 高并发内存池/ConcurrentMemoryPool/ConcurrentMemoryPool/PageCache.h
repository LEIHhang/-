#pragma once
#include"Common.h"

class PageCache
{
public:
	Span* NewSpan(size_t numpage);
private:
	SpanList _spanLists[129];//สนำร1-128
};

static PageCache pageCacheInst;