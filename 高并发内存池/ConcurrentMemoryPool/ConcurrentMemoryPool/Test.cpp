#pragma once
#include"ThreadCache.h"
#include<vector>
void UnitThreadCache()
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

int main()
{
	UnitThreadCache();
	return 0;
}