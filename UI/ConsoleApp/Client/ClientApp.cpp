#include <ClientFactory.cpp>

void printError(std::string e)
{
	std::cout << e << std::endl;
}

int main()
{
	sqlite3 *db;
	char *zErrMsg = 0;
	int rc;

	rc = sqlite3_open("test.db", &db);

	if(rc)
	{
		fprintf(stderr, "Can't open database: %s\n", sqlite3_errmsg(db));
		return (0);
	}
	else
	{
		fprintf(stderr, "Opened database successfully\n");
	}
	sqlite3_close(db);

	auto clientFactory = ClientFactory::invokeClientFactory(printError);
	auto client = clientFactory();

	client->connect("127.0.0.1", "9000");

	std::thread thread = std::thread(
			[&]()
			{
				while(client->isConnected())
				{
					auto msg = client->readMessage();
					std::cout << "[" + msg.getSenderName() + "] " + msg.getBody() << std::endl;
				};
			});

	std::string username;
	std::cout << "Enter your username: ";
	std::cin >> username;

	client->login(username);

	std::string mess;

	while(true)
	{
		std::cin.ignore();
		std::getline(std::cin, mess);
		if(mess == "1")
		{
			client->printUsers();
		}
		else
		{
			client->sendMessageToAll(mess);
		}

		mess.clear();
	}

	client->unlogin();
	thread.join();

	return 0;
}