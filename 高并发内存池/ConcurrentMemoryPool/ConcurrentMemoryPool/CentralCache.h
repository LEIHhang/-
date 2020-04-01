#pragma once
#include"Common.h"

class CentralCache
{
public:
	//�����Ļ����л�ȡһ�������Ķ����thread cache
	//num:������ȡ������return value:ʵ�ʻ�ȡ����
	size_t FetchRangeObj(void*& start, void*& end, size_t num, size_t size);

	//��һ�������Ķ����ͷŵ�span���
	void ReleaseListToSpans(void* start, size_t size);

	//��spanlist���ߴ�page cache��ȡһ��span
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

//��̬���������Ƕ���̹߳���

//span ��ȣ�����ҳΪ��λ���ڴ���󣬱����Ƿ������ϲ�������ڴ���Ƭ��


