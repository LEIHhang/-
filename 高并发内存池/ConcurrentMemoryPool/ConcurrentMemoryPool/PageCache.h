#pragma once
#include"Common.h"

class PageCache
{
public:
	Span* NewSpan(size_t numpage);//��ȡһ��numpageҳ��span
	Span* GetIdToSpan(PAGE_ID id);//����ID��ȡSpan
	void ReleaseSpanToPageCache(Span* span);
private:
	SpanList _spanLists[MAX_PAGES];//ʹ��1-128
	std::map<PAGE_ID, Span*> _idSpanMap;
	
};

static PageCache pageCacheInst;