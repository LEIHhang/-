#pragma once
#include<vector>
#include"Common.h"
#include"ThreadCache.h"
#include"Concurrent.h"
void UnitThreadCache1()
{
	ThreadCache tc;
	std::vector<void*> v;
	for (size_t i = 0; i < 25; ++i)
	{
		v.push_back(tc.Allocte(7));
	}
	for (size_t i = 0; i < v.size(); ++i)
	{
		printf("[%d]--%p\n", i, v[i]);
	}
}
void UnitThreadCache2()
{
	cout << SizeClass::RoundUp(7) << endl;
	cout << SizeClass::RoundUp(9) << endl;
	cout << SizeClass::RoundUp(10) << endl;
	cout << SizeClass::RoundUp(17) << endl;
	cout << SizeClass::RoundUp(18) << endl;
	cout << SizeClass::RoundUp(25) << endl;

	cout << SizeClass::ListIndex(7) << endl;
	cout << SizeClass::ListIndex(9) << endl;
	cout << SizeClass::ListIndex(10) << endl;
	cout << SizeClass::ListIndex(17) << endl;
	cout << SizeClass::ListIndex(18) << endl;
	cout << SizeClass::ListIndex(25) << endl;
	cout << SizeClass::ListIndex(129) << endl;


}

void UnitPageCache1()
{
	void* ptr = SystemAllocPage(MAX_PAGES - 1);
	PAGE_ID id = (PAGE_ID)ptr >> PAGE_SHIFT;
	cout << id << endl;
}
void UnitThread1()
{

}
void func1(int n)
{
	std::vector<void*> v;
	size_t size = 7;
	for (size_t i = 0; i < SizeClass::NumMoveSize(size) + 1; ++i)
	{
		v.push_back(ConcurrentMalloc(size));
	}

	for (size_t i = 0; i < v.size(); ++i)
	{
		printf("[%d]->%p\n", i, v[i]);
	}
	for (auto ptr : v)
	{
		ConcurrentFree(ptr);
	}
	v.clear();
}

void func2(int n)
{
	std::vector<void*> v;
	size_t size = 7;
	for (size_t i = 0; i < SizeClass::NumMoveSize(size) + 1; ++i)
	{
		v.push_back(ConcurrentMalloc(size));
	}

	for (size_t i = 0; i < v.size(); ++i)
	{
		printf("[%d]->%p\n", i, v[i]);
	}
	for (auto ptr : v)
	{
		ConcurrentFree(ptr);
	}
	v.clear();
}
int main()
{
	//UnitPageCache1();
	std::thread t1(func1, 100);
	std::thread t2(func2, 100);
	cout << "主线程等待" << endl;
	t1.join();
	t2.join();
	return 0;
}