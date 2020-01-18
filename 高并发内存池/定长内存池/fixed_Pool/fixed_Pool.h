#pragma once

template<class T, size_t initNum = 100>
class FixedPool
{
public:
	FixedPool()
	{
		//申请initNum个大小的内存
		_start = (char*)malloc(initNum*sizeof(T));
	}
	T* NEW()//申请一个对象
	{
		if (_start != _end)
		{
			_start = (char*)malloc(initNum*sizeof(T));
		}
		T* obj = (T*)_start;
		_start += sizeof(T);

		//new的定位表达式//调用T类型的默认构造函数对obj指向的空间进行初始化
		new(obj)T;

		return obj;
	}

	void Delete(T* ptr)
	{
		//先调用析构函数，要先把当前空间中存在的指针所指向的东西释放掉
		ptr->~T();
		*(int*)ptr = freeList;//先把freeList存的地址赋值给*ptr的前四个字节位置上
		freeList = ptr;//在把ptr指向的结点赋值给freeList
	}
private:
	char* _start;
	char* _end;

	T* freeList;
};