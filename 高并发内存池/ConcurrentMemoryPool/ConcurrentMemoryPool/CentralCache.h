#pragma once
#include"Common.h"

class CentralCache
{
private:
	SpanList _spanlists[NFREE_LIST];
};

//span 跨度：管理页为单位的内存对象，本质是方便做合并，解决内存碎片。


