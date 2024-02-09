#include <ClientLib.h>

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

	std::shared_ptr<ClientLib> client = std::make_shared<ClientLib>();
	client->Connect("127.0.0.1", "9000");

	std::thread thread = std::thread(
			[&]()
			{
				while(client->IsConnected())
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