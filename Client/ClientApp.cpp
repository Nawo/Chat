#include "../Core/Client/Client.h"

class ClientApp : public Client
{
public:
	bool login(const std::string &userName)
	{
		// TODO - handle case when user cannot login to the server
		std::string codedMessage = ResponseCoder::makeCollable()(MessageType::Establish, userName, "", "");

		Send(codedMessage);

		return true;
	}

	bool unlogin()
	{
		std::string codedMessage = ResponseCoder::makeCollable()(MessageType::Relinquish, "", "", "");

		Send(codedMessage);
	}

	bool sendMessage(const std::string &sender, const std::string &recipient, const std::string &message)
	{
		// TODO - handle case when user sent wrong message
		std::string codedMessage = ResponseCoder::makeCollable()(MessageType::Message, sender, recipient, message);

		Send(codedMessage);

		return true;
	}
};

int main()
{
	std::shared_ptr<ClientApp> client = std::make_shared<ClientApp>();
	client->Connect("127.0.0.1", "9000");

	std::string username;
	std::cout << "Enter your username: ";
	std::cin >> username;

	client->login(username);

	std::string mess;
	std::string receiver = "all";
	// std::cout << std::endl;
	// std::cout << "Enter recipient (use "
	// 			 "all"
	// 			 " to broadcast): ";
	// std::cin >> receiver;

	while(true)
	{
		std::cin.ignore();
		std::getline(std::cin, mess);
		client->sendMessage(username, receiver, mess);
	}

	client->unlogin();

	return 0;
}