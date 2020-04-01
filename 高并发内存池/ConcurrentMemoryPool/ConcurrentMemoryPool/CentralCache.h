#pragma once
#include"Common.h"

class CentralCache
{
public:
	//从中心缓存中获取一定数量的对象给thread cache
	//num:期望获取个数，return value:实际获取个数
	size_t FetchRangeObj(void*& start, void*& end, size_t num, size_t size);

	//将一定数量的对象释放到span跨度
	void ReleaseListToSpans(void* start, size_t size);

	//从spanlist或者从page cache获取一个span
	Span* GetOneSpan(size_t size);
	static CentralCache& GetInstance()
	{
		static CentralCache centralCacheInst;
		return centralCacheInst;
	}
private:
	CentralCache()
	{}
	SpanList _spanlists[NFREE_LIST];
};

//静态区的数据是多个线程共享

//span 跨度：管理页为单位的内存对象，本质是方便做合并，解决内存碎片。


