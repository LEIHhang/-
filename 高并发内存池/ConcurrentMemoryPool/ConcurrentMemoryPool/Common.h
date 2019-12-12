//����ģ�鶨�崦
#pragma once
#include<iostream>
#include<assert.h>
using namespace std;

const size_t MAX_SIZE = 64 * 1024; //64k���µĴ��ڴ�����룬���ϵĴ�ϵͳ����
const size_t NFREE_LIST = MAX_SIZE / 8; //8k�������������󳤶�
//�������һ����Ź̶���С�����ڴ�ռ������

inline void*& NextObj(void* obj)
{
	//����objָ��ռ�ǰ��/�˸��ֽڵ�ָ��,Ҳ������һ���ڵ��ָ��
	return *(void**)obj;
}
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
		NextObj(tail) = _freelist;
		_freelist = head;
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
	//���ݶ����С����ָ�������±�
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
	void* _freelist;//������������
	int _usecount;//�ڴ�����ʹ�ü���

	size_t objsize;//�����С

	Span* _next;
	Span* _prev;
};

//SpanList��һ����Span������������
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