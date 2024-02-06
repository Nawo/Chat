#include "Client.h"

bool ClientApp::login(const std::string &userName)
{
	std::string codedMessage = ResponseCoder::makeCollable()(MessageType::Establish, userName, "", "");

	Send(codedMessage);

	return true;
}

bool ClientApp::unlogin()
{
	std::string codedMessage = ResponseCoder::makeCollable()(MessageType::Relinquish, "", "", "");

	Disconnect();

	Send(codedMessage);

	return true;
}

bool ClientApp::sendMessage(const std::string &sender, const std::string &recipient, const std::string &message)
{
	std::string codedMessage = ResponseCoder::makeCollable()(MessageType::Message, sender, recipient, message);

	Send(codedMessage);

	return true;
}

bool ClientApp::sendMessageToAll(const std::string &message)
{
	std::string codedMessage = ResponseCoder::makeCollable()(MessageType::MessageAll, "", "", message);

	Send(codedMessage);

	return true;
}

std::string ClientApp::readMessage()
{
	m_incomingMessages.wait();

	if(!m_incomingMessages.empty())
	{
		return m_incomingMessages.pop_back();
	}
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

	std::shared_ptr<ClientApp> client = std::make_shared<ClientApp>();
	client->Connect("127.0.0.1", "9000");

	std::thread thread = std::thread(
			[&]()
			{
				while(client->IsConnected())
				{
					std::cout << client->readMessage() << std::endl;
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
		client->sendMessageToAll(mess);
		std::cout << "//" + mess + "//" << std::endl;
		mess.clear();
	}

	client->unlogin();
	thread.join();

	return 0;
}