#include"client.hpp"
void Scandir()
{
	const char *ptr = "./";
	boost::filesystem::directory_iterator begin(ptr);//����һ��Ŀ¼����������
	boost::filesystem::directory_iterator end;//directory_iteratorĬ�Ϲ��캯���ṹ��ָ��end"��iterator
	for (; begin != end; ++begin)
	{
		//begin->status() Ŀ¼�е�ǰ�ļ���״̬��Ϣ
		if (boost::filesystem::is_directory(begin->status()))
		{
			std::cout << begin->path().string() << "��һ��Ŀ¼\n";
		}
		else
		{
			std::cout << begin->path().string() << "��һ����ͨ�ļ�\n";
		}
	}
}
void HellowWorld(const httplib::Request &res, httplib::Response &rsp)
{
	printf("�յ�һ������\n");
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