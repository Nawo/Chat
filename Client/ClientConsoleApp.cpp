#include "../Core/Client/Client.h"
#include "../Core/Handlers/ResponseCoder.h"
#include "../Core/Handlers/ResponseDecoder.h"

class ClientApp : public Client
{
public:
	bool login(const std::string &userName)
	{
		std::string codedMessage = ResponseCoder::makeCollable()(MessageType::Establish, userName, "", "");

		Send(codedMessage);

		return true;
	}

	bool unlogin()
	{
		std::string codedMessage = ResponseCoder::makeCollable()(MessageType::Relinquish, "", "", "");

		Disconnect();

		Send(codedMessage);

		return true;
	}

	bool sendMessage(const std::string &sender, const std::string &recipient, const std::string &message)
	{
		std::string codedMessage = ResponseCoder::makeCollable()(MessageType::Message, sender, recipient, message);

		Send(codedMessage);

		return true;
	}

	bool sendMessageToAll(const std::string &message)
	{
		std::string codedMessage = ResponseCoder::makeCollable()(MessageType::MessageAll, "", "", message);

		Send(codedMessage);

		return true;
	}

	void readMessage()
	{
		while(IsConnected())
		{
			m_incomingMessages.wait();

			if(!m_incomingMessages.empty())
			{
				std::string data = m_incomingMessages.pop_back();

				auto decodedMessage = ResponseDecoder::makeCollable()(data);

				std::cout << "[" + decodedMessage->getSenderName() + "]: " + decodedMessage->getBody();
			}
		}
	}
};

int main()
{
	std::shared_ptr<ClientApp> client = std::make_shared<ClientApp>();
	client->Connect("127.0.0.1", "9000");

	std::thread thread = std::thread([&]() { client->readMessage(); });

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