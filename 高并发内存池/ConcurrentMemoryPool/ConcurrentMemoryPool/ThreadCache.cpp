#pragma once
#include"ThreadCache.h"

void* ThreadCache::Allocte(size_t size)
{
	//Index(size);��ȡ���������±�
	size_t index = SizeClass::ListIndex(size);
	FreeList& freeList = _freeLists[index];
	if (!freeList.Empty())
	{
		//��ʾ��ǰ�����������пռ����ʹ��
		return freeList.Pop();
	}
	else
	{
		//������û�У���ֻ�ܴ�centre cache������
		//��num-1����������һ������
		size_t num = 20;//����С��Ҫ���ö�һЩ���������Ҫ����СһЩ
		return FechFromCentralCache(index, num);
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

void* ThreadCache::FechFromCentralCache(size_t index, size_t num)
{
	//ģ��ȡ�ڴ����Ĵ��룬����Thread cache�߼�
	size_t size = (index + 1) * 8;
	char* start = (char*)malloc(num * size);
	char* cur = start;
	for (size_t i = 0; i < num - 1; ++i)
	{
		char* next = cur + size;
		NextObj(cur) = next;

		cur = next;
	}
	NextObj(cur) = nullptr;

	void* head = NextObj(start);
	void* tail = cur;

	_freeLists[index].PushRange(head, tail);
	return start;
}