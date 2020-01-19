#pragma once
#include"ThreadCache.h"
#include"CentralCache.h"
void* ThreadCache::Allocte(size_t size)
{
	//Index(size);������󣬻�ȡ�±�
	size_t index = SizeClass::ListIndex(size);
	//��ȡ�ô�С���пռ��������
	FreeList& freeList = _freeLists[index];
	if (!freeList.Empty())
	{
		//��ʾ��ǰ�����������пռ����ʹ��
		//����ָ��ÿռ��ָ�룬���Ѹÿռ��������ɾ��
		return freeList.Pop();
	}
	else
	{
		//������û�У���ֻ�ܴ�centre cache�л�ȡ����ô�С����������Ҫ�ռ��Сȷ��
		//����һ������
		//��Ϊ��ȡ�Ļ����ǵ�����ȡ����ȡ������������������Ҫ��thread������
		return FechFromCentralCache(SizeClass::RoundUp(size));
	}
}
void ThreadCache::Deallocte(void* ptr, size_t size)
{
	size_t index = SizeClass::ListIndex(size);//����size��ȡindex��
	FreeList& freeList = _freeLists[index];
	freeList.Push(ptr);

	//if ()
	//{
	//��������Դ������centralcache��
	//	ReleaseToCentralCache();
	//}
}


//��������thread cache
//void* ThreadCache::FechFromCentralCache(size_t size)
//{
//	//ģ��ȡ�ڴ����Ĵ��룬����Thread cache�߼�
//	size_t num = 20;
//	size_t index = SizeClass::ListIndex(size);
//	char* start = (char*)malloc(num * size);
//	char* cur = start;
//	for (size_t i = 0; i < num - 1; ++i)
//	{
//		char* next = cur + size;
//		NextObj(cur) = next;
//
//		cur = next;
//	}
//	NextObj(cur) = nullptr;
//
//	void* head = NextObj(start);
//	void* tail = cur;
//
//	_freeLists[index].PushRange(head, tail);
//	return start;
//}

void* ThreadCache::FechFromCentralCache(size_t size)
{
	size_t num = SizeClass::NumMoveSize(size);
	void* start = nullptr;
	void* end = nullptr;
	//actual ʵ�ʻ�ȡ���Ķ���
	//start ��end��������Ͳ��� �����Ļ����ȡһ������
	int actualNum = centralCacheInst.FetchRangeObj(start, end, num, size);
	if (actualNum == 1)
	{
		return start;
	}
	else
	{
		size_t index = SizeClass::ListIndex(size);
		FreeList& freelist = _freeLists[index];
		//�����Ļ����ȡ���������pushRange����
		freelist.PushRange(NextObj(start), end);

		return start;
	}
}