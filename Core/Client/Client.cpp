#include "Client.hpp"

Client::Client() : m_socket(m_context)
{
}

Client::~Client()
{
	Disconnect();
}

const bool Client::Run()
{
	m_thread = std::thread([this]() { m_context.run(); });
	return true;
}

const bool Client::Connect(const std::string &host, const std::string &port)
{
	try
	{
		asio::ip::tcp::resolver resolver(m_context);
		asio::connect(m_socket, resolver.resolve(host, port));

		if(!IsConnected())
		{
			return false;
		}

		Run();
		Read();
	}
	catch(std::exception &e)
	{
		std::cerr << "[ERROR] " << e.what() << "\n";
		return false;
	}
	return true;
}

const bool Client::Disconnect()
{
	std::string codedMessage = ResponseCoder::makeCollable()(MessageType::Relinquish, "", "", "");

	Send(codedMessage);

	if(IsConnected())
	{
		asio::post(m_context, [this]() { m_socket.close(); });
	}

	m_context.stop();

	if(m_thread.joinable())
	{
		m_thread.join();
	}

	return true;
}

const bool Client::IsConnected() const
{
	return m_socket.is_open();
}

const std::string Client::Read()
{
	asio::async_read_until(m_socket, asio::dynamic_buffer(m_data), '\n',
						   [this](std::error_code errorCode, std::size_t length)
						   {
							   if(!errorCode)
							   {
								   auto decode = ResponseDecoder::makeCollable()(m_data);
								   std::cout << "[" + decode->getSenderName() + "]: " + decode->getBody();
								   m_data.erase(0, length); // Clear the processed data
								   Read();					// Initiate another read operation
							   }
							   else
							   {
								   std::cerr << "[ERROR] Read failed: " << errorCode.message() << std::endl;
							   }
						   });
	return "";
}

bool Client::login(const std::string &userName)
{
	// TODO - handle case when user cannot login to the server
	std::string codedMessage = ResponseCoder::makeCollable()(MessageType::Establish, userName, "", "");

	Send(codedMessage);

	return true;
}

bool Client::sendMessage(const std::string &sender, const std::string &recipient, const std::string &message)
{
	// TODO - handle case when user sent wrong message
	std::string codedMessage = ResponseCoder::makeCollable()(MessageType::Message, sender, recipient, message);

	Send(codedMessage);

	return true;
}

const bool Client::Send(const std::string &msg)
{
	asio::write(m_socket, asio::buffer(msg + "\n"));
	return true;
}

int main()
{
	std::shared_ptr<Client> client = std::make_shared<Client>();
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

	return 0;
}