#pragma once
#include"Common.h"

class PageCache
{
public:
	Span* NewSpan(size_t numpage);//��ȡһ��numpageҳ��span
private:
	SpanList _spanLists[MAX_PAGES];//ʹ��1-128
};

static PageCache pageCacheInst;