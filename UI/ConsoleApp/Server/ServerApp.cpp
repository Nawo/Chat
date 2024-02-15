#include <ServerFactory.cpp>

int main()
{
	auto serverInvoke = ServerFactory::invokeServerFactory();
	auto server = serverInvoke(9000);

	server->Start();
	std::cout << "[SERVER] started" << std::endl;
	while(true)
	{
		server->Update();
	}
}