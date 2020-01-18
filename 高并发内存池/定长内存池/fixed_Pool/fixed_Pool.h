#pragma once

template<class T, size_t initNum = 100>
class FixedPool
{
public:
	FixedPool()
	{
		//����initNum����С���ڴ�
		_start = (char*)malloc(initNum*sizeof(T));
	}
	T* NEW()//����һ������
	{
		if (_start != _end)
		{
			_start = (char*)malloc(initNum*sizeof(T));
		}
		T* obj = (T*)_start;
		_start += sizeof(T);

		//new�Ķ�λ���ʽ//����T���͵�Ĭ�Ϲ��캯����objָ��Ŀռ���г�ʼ��
		new(obj)T;

		return obj;
	}

	void Delete(T* ptr)
	{
		//�ȵ�������������Ҫ�Ȱѵ�ǰ�ռ��д��ڵ�ָ����ָ��Ķ����ͷŵ�
		ptr->~T();
		*(int*)ptr = freeList;//�Ȱ�freeList��ĵ�ַ��ֵ��*ptr��ǰ�ĸ��ֽ�λ����
		freeList = ptr;//�ڰ�ptrָ��Ľ�㸳ֵ��freeList
	}
private:
	char* _start;
	char* _end;

	T* freeList;
};