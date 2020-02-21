#include"tool.hpp"
#include"httplib.h"
void HellowWorld(const httplib::Request &res, httplib::Response &rsp)
{
	printf("收到一个请求\n");
	rsp.set_content("<html><h1>hellow world</h1><html>", "text/html");
	rsp.status = 200;
}
int main(int argc,char* argv[])
{
	std::vector<Adapter> list;
	AdapterTool::GetAllAdapter(&list);

	httplib::Server srv;
	srv.Get("/", HellowWorld);
	srv.listen("0.0.0.0", 9000);
	return 0;
}