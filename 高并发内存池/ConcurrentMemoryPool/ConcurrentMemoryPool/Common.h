//公共模块定义处
#pragma once
#include<iostream>
#include<assert.h>
using namespace std;

const size_t MAX_SIZE = 64 * 1024; //64k
const size_t NFREE_LIST = MAX_SIZE / 8; //8k
//这个类是一个存放固定大小对象内存空间的链表

inline void*& NextObj(void* obj)
{
	//返回obj指向空间前四/八个字节的指针,也就是下一个节点的指针
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

	void PushRange(void* head, void* tail)
	{
		NextObj(tail) = _freelist;
		_freelist = head;
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

class SizeClass
{
public:
	//根据对象大小计算指针数组下标
	static size_t ListIndex(size_t size)
	{
		if (size % 8 == 0)
		{
			return size / 8 - 1;
		}
		else
			return size / 8;
	}
};

//span 跨度：管理页为单位的内存对象，本质是方便做合并，解决内存碎片。
#ifdef _WIN32
typedef unsigned int PAGE_ID;
#else
typedef unsigned long long PAGE_ID;
#endif

struct Span
{
	PAGE_ID _pageid;//页号
	int _pagesize;//页的数量
	void* _freelist;//对象自由链表
	int _usecount;//内存块对象使用计数

	size_t objsize;//对象大小

	Span* _next;
	Span* _prev;
};

//SpanList是一个将Span链起来的链表
class SpanList
{
public:
	SpanList()
	{
		_head = new Span;
		_head->_next = _head;
		_head->_prev = _head;
	}

	void PushFront(Span* newpos)
	{
		Insert(_head->_next, newpos);
	}

	void PopFront()
	{
		Erase(_head->_next);
	}

	void PushBack(Span* newpos)
	{
		Insert(_head, newpos);
	}

	void PopBack()
	{
		Erase(_head->_prev);
	}

	void Insert(Span* pos, Span* newspan)
	{
		Span* prev = pos->_prev;
		//prev newspan pos
		prev->_next = newspan;
		newspan->_next = pos;
		newspan->_prev = prev;
		pos->_prev = newspan;
	}

	void Erase(Span* pos)
	{
		assert(pos != _head);
		Span* prev = pos->_prev;
		Span* next = pos->_next;
		prev->_next = next;
		next->_prev = prev;
	}
private:
	Span* _head;
};