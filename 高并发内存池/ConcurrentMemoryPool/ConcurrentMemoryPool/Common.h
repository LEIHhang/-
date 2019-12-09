//公共模块定义处
#pragma once
#include<iostream>
using namespace std;

const size_t MAX_SIZE = 64 * 1024;
const size_t NFREE_LIST = MAX_SIZE / 8;
//这个类是一个存放固定大小对象内存空间的链表

inline void*& NextObj(void* obj)
{
	//返回obj指向空间前四/八个字节的指针
	return *(void**)obj;
}
class FreeList
{
public:
	void Push(void* obj)
	{
		//头插
		NextObj(obj) = _freelist;
		_freelist = obj;
	}

	void* Pop()
	{
		//头删
		void* obj = _freelist;
		_freelist = NextObj(obj);
		return obj;
	}
	bool Empty()
	{
		return (_freelist == nullptr);
	}
private:
	void* _freelist= nullptr;
};