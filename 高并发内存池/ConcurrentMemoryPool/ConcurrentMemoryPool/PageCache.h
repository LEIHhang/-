#pragma once
#include"Common.h"
#include"radix.hpp"
class PageCache
{
public:
	Span* NewSpan(size_t numpage);//��ȡһ��numpageҳ��span
	Span* GetIdToSpan(PAGE_ID id);//����ID��ȡSpan
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
	SpanList _spanLists[MAX_PAGES];//ʹ��1-128
	radix _idRadix;
	std::mutex _mutex;
};
