//����ģ�鶨�崦
#pragma once
#include<iostream>
using namespace std;

const size_t MAX_SIZE = 64 * 1024;
const size_t NFREE_LIST = MAX_SIZE / 8;
//�������һ����Ź̶���С�����ڴ�ռ������

inline void*& NextObj(void* obj)
{
	//����objָ��ռ�ǰ��/�˸��ֽڵ�ָ��
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