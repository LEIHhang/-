#pragma once
#include"Common.h"
#include"radix.hpp"
class PageCache
{
public:
	Span* NewSpan(size_t numpage);//获取一个numpage页的span
	Span* GetIdToSpan(PAGE_ID id);//根据ID获取Span
	void ReleaseSpanToPageCache(Span* span);
	Span* _NewSpan(size_t numpage);
	static PageCache& GetInstance()
	{
		static PageCache pageCacheInst;
		return pageCacheInst;
	}
private:
	PageCache()
	{}
	SpanList _spanLists[MAX_PAGES];//使用1-128
	radix _idRadix;
	std::mutex _mutex;
};
