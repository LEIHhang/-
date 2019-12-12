//公共模块定义处
#pragma once
#include<iostream>
#include<assert.h>
using namespace std;

const size_t MAX_SIZE = 64 * 1024; //64k以下的从内存池申请，以上的从系统申请
const size_t NFREE_LIST = MAX_SIZE / 8; //8k，表明链表的最大长度
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

	static size_t _RoundUp(size_t size, int alignment)
	{
		// (1-7) + 7 = (8-14) -> 8 4 2 1  8-14转换成二进制都是第四位为1，前三位自由组合，让他们对齐到8 所以&取反的7
		//（17-23）+7 =（24-30）->16 8 4 2 1 
		return (size + alignment - 1)&(~(alignment - 1));
	}
	//向上对齐 alignment 对齐数
	static inline size_t RoundUp(size_t size)
	{
		assert(size <= MAX_SIZE);
		if (size <= 128)
			return _RoundUp(size, 8);
		else if (size <= 1024)
			return _RoundUp(size, 16);
		else if (size <= 8192)
			return _RoundUp(size, 128);
		else if (size <= 65536)
			return _RoundUp(size, 1024);

		return -1;
	}
	//针对每一个size挪动Num个对象过去
	static size_t NumMoveSize(size_t size)
	{
		if (size == 0)
			return 0;
		int num = MAX_SIZE / size;

		if (num < 2)
			num = 2;

		if (num > 512)
			num = 512;
		return num;
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