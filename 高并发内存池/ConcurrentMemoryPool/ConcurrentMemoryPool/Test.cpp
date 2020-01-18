#pragma once
#include<vector>
#include"Common.h"
void UnitThreadCache1()
{
	ThreadCache tc;
	vector<void*> v;
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
int main()
{
	UnitPageCache1();
	return 0;
}