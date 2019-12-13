//����ģ�鶨�崦
#pragma once
#include<iostream>
#include<assert.h>
#include<Windows.h>
using namespace std;
//65537
const size_t MAX_SIZE = 64 * 1024; //64k���µĴ�threadcache����0-16ҳ
const size_t NFREE_LIST = MAX_SIZE / 8; //8k�������������󳤶�
const size_t MAX_PAGES = 129;//��������ҳ��
const size_t PAGE_SHIFT = 12; //4kΪҳ��λ��

inline void*& NextObj(void* obj)
{
	//����objָ��ռ�ǰ��/�˸��ֽڵ�ָ��,Ҳ������һ���ڵ��ָ��
	return *(void**)obj;
}
//�������һ����Ź̶���С�����ڴ�ռ������
class FreeList
{
public:
	void Push(void* obj)
	{
		//ͷ��
		NextObj(obj) = _freelist;
		_freelist = obj;
	}

	void PushRange(void* head, void* tail)
	{
		//ͷ��
		//_freelistָ��ĵ�ַ��ֵ��tailָ�����ǰ�ĸ��ֽ��ϣ�����tailָ��ͷһ�����
		NextObj(tail) = _freelist;
		//head��һ��ָ��ͷ����ָ�룬��ֵ����һ��ָ�룬��ô���ָ��Ҳָ��ͷ���
		_freelist = head;
	}
	size_t PopRange(void* start, void* end, size_t num)
	{
		size_t actualNum = 0;
		void* cur = _freelist;
		void*prev = nullptr;
		for (; actualNum < num && cur!=nullptr; ++actualNum)
		{
			prev = cur;
			cur = NextObj(cur);
		}

		start = _freelist;
		end = prev;
		_freelist = cur;

		return actualNum;
	}

	void* Pop()
	{
		//ͷɾ
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
	// ������12%���ҵ�����Ƭ�˷� 
	// [1,128]                  8byte����  freelist[0,16) 
	// [129,1024]               16byte����     freelist[16,72) 
	// [1025,8*1024]            128byte����    freelist[72,128) 
	// [8*1024+1,64*1024]       512byte����  freelist[128,240)

	//���ݶ����С����ָ�������±�
	static size_t _ListIndex(size_t size, int alig_shift)
	{
		//2��align_shift������alignment
		return ((size + ((1 << alig_shift) - 1)) >> alig_shift) -1;
	}

	static inline size_t ListIndex(size_t size)
	{
		assert(size <= MAX_SIZE);
		static int group_array[3] = { 16, 72, 128 };
		if (size <= 128)
			return _ListIndex(size, 3);
		else if (size <= 1024)
		{
			return _ListIndex(size - 128, 4) + group_array[0];
		}
		else if (size <= 8192)
			return _ListIndex(size - 1024, 7) + group_array[1];
		else if (size <= 65536)
		return _ListIndex(size - 8192, 10) + group_array[2];

		return -1;
	}

	static size_t _RoundUp(size_t size, int alignment)
	{
		// (1-7) + 7 = (8-14) -> 8 4 2 1  8-14ת���ɶ����ƶ��ǵ���λΪ1��ǰ��λ������ϣ������Ƕ��뵽8 ����&ȡ����7
		//��17-23��+7 =��24-30��->16 8 4 2 1 
		return (size + alignment - 1)&(~(alignment - 1));
	}
	//���϶��� alignment ������
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
	//���ÿһ��sizeŲ��Num�������ȥ
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

	static size_t NumMovePage(size_t size)
	{
		size_t num = NumMoveSize(size);
		size_t npage = num*size;
		npage >>= 12;
		if (npage == 0)
			npage = 1;
		return npage;
	}
};

//span ��ȣ�����ҳΪ��λ���ڴ���󣬱����Ƿ������ϲ�������ڴ���Ƭ��
#ifdef _WIN32
typedef unsigned int PAGE_ID;
#else
typedef unsigned long long PAGE_ID;
#endif

struct Span
{
	PAGE_ID _pageid;//ҳ��
	int _pagesize;//ҳ������
	FreeList _freelist;//������������
	int _usecount;//�ڴ�����ʹ�ü���

	size_t objsize;//�����С

	Span* _next;
	Span* _prev;
};

//SpanList��һ����Span�������Ĵ�ͷ�ڵ��ѭ������
class SpanList
{
public:
	SpanList()
	{
		_head = new Span;
		_head->_next = _head;
		_head->_prev = _head;
	}

	bool Empty()
	{
		return(_head == _head->_next);
	}
	Span* Begin()
	{
		return _head->_next;
	}

	Span* End()
	{
		return _head;
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

//��ϵͳ����numpageҳ�ڴ�ҵ���������
inline static void* SystemAllocPage(size_t num_page)
{
#ifdef _WIN32
	void* ptr = VirtualAlloc(0, num_page*(1 << PAGE_SHIFT),
		MEM_COMMIT | MEM_RESERVE, PAGE_READWRITE);
#else
	//brk mmap��
#endif
	if (ptr == nullptr)
		throw std::bad_alloc();

	return ptr;
}