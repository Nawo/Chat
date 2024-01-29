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
	auto decode = ResponseDecoder::makeCollable();

	std::string msg(std::to_string(static_cast<int>(MessageType::Establish)) + "|" + userName + "||");

	sendRequest(decode(msg));

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

	auto decode = ResponseDecoder::makeCollable();

	std::string test(std::to_string(static_cast<int>(MessageType::Message)) + "|" + sender + "|" + recipient + "|"
					 + message);

	sendRequest(decode(test));

	return true;
}

void Client::sendRequest(MessagePtr msg)
{
	std::string request(std::to_string(static_cast<int>(msg->getMessageType())) + "|" + msg->getSenderName() + "|"
						+ msg->getReceiverName() + "|" + msg->getBody());
	asio::write(m_socket, asio::buffer(request + "\n"));
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