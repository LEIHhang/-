#pragma once
#include"ThreadCache.h"
void* ThreadCache::FechFromCentralCache(size_t index, size_t num)
{
	//
}
void* ThreadCache::Allocte(size_t size)
{
	size_t index;//Index(size);��ȡ���������±�
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
	size_t index;//����size��ȡindex��
	FreeList& freeList = _freeLists[index];
	freeList.Push(ptr);

	//if ()
	//{
	//��������Դ������centralcache��
	//	ReleaseToCentralCache();
	//}
}