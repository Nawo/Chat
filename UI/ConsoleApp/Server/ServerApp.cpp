#include <ServerLib.h>

int main()
{
	std::shared_ptr<ServerLib> server = std::make_shared<ServerLib>(9000);
	server->Start();
	std::cout << "[SERVER] started" << std::endl;
	while(true)
	{
		server->Update();
	}
}