#include"client.hpp"
void Scandir()
{
	const char *ptr = "./";
	boost::filesystem::directory_iterator begin(ptr);//定义一个目录迭代器对象
	boost::filesystem::directory_iterator end;//directory_iterator默认构造函数会构造指向“end"的iterator
	for (; begin != end; ++begin)
	{
		//begin->status() 目录中当前文件的状态信息
		if (boost::filesystem::is_directory(begin->status()))
		{
			std::cout << begin->path().string() << "是一个目录\n";
		}
		else
		{
			std::cout << begin->path().string() << "是一个普通文件\n";
		}
	}
}
void HellowWorld(const httplib::Request &res, httplib::Response &rsp)
{
	printf("收到一个请求\n");
	rsp.set_content("<html><h1>hellow world</h1><html>", "text/html");
	rsp.status = 200;
}
void test1()
{
	/*std::vector<Adapter> list;
	AdapterTool::GetAllAdapter(&list);

	httplib::Server srv;
	srv.Get("/", HellowWorld);
	srv.listen("0.0.0.0", 9000);*/
	//Scandir();
}
void ClientRun()
{
	Sleep(1);
	Client cli;
	cli.Start();
}
void test2()
{
	std::thread cli_thr(ClientRun);
	Server srv;
	srv.start();
}
int main(int argc,char* argv[])
{
	test2();
	return 0;
}