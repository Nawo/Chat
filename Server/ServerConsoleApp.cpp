#include "../Core/Server/Server.h"

int main()
{
	std::shared_ptr<Server> server = std::make_shared<Server>(9000);
	server->Start();
	while(true)
	{
	}
}