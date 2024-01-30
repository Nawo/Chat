#include "Client.hpp"

Client::Client() : m_socket(m_context)
{
}

Client::~Client()
{
	disconnect();
}

bool Client::connect(const std::string &host, const std::string &port)
{
	asio::ip::tcp::resolver resolver(m_context);
	asio::connect(m_socket, resolver.resolve(host, port));

	if(!m_socket.is_open())
		return false;

	readResponse();
	run();

	return true;
}

void Client::readResponse()
{
	asio::async_read_until(m_socket, asio::dynamic_buffer(data_), '\n',
						   [this](std::error_code errorCode, std::size_t length)
						   {
							   if(!errorCode)
							   {
								   auto decode = ResponseDecoder::makeCollable()(data_);
								   std::cout << "[" + decode->getSenderName() + "]: " + decode->getBody();
								   data_.erase(0, length); // Clear the processed data
								   readResponse();		   // Initiate another read operation
							   }
							   else
							   {
								   std::cerr << "[ERROR] Read failed: " << errorCode.message() << std::endl;
							   }
						   });
}

bool Client::login(const std::string &userName)
{
	// TODO - handle case when user cannot login to the server
	std::string codedMessage = ResponseCoder::makeCollable()(MessageType::Establish, userName, "", "");

	sendRequest(codedMessage);

	return true;
}

void Client::disconnect()
{
	m_context.stop();

	if(thrContext.joinable())
		thrContext.join();
}

bool Client::sendMessage(const std::string &sender, const std::string &recipient, const std::string &message)
{
	// TODO - handle case when user sent wrong message
	std::string codedMessage = ResponseCoder::makeCollable()(MessageType::Message, sender, recipient, message);

	sendRequest(codedMessage);

	return true;
}

void Client::sendRequest(const std::string &msg)
{
	asio::write(m_socket, asio::buffer(msg + "\n"));
}

void Client::run()
{
	thrContext = std::thread([this]() { m_context.run(); });
}

int main()
{
	std::shared_ptr<Client> client = std::make_shared<Client>();
	client->connect("127.0.0.1", "9000");

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