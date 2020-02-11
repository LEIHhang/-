#include"tool.hpp"

int main()
{
	std::vector<Adapter> list;
	AdapterTool::GetAllAdapter(&list);
	return 0;
}