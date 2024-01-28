#include "Client.hpp"

#include "../Handlers/ResponseDecoder.cpp"

Client::Client() : m_socket(m_context)
{
}

Client::~Client()
{
}

void Client::connect(const std::string &host, const std::string &port)
{
	asio::ip::tcp::resolver resolver(m_context);
	asio::connect(m_socket, resolver.resolve(host, port));
	readResponse();
}

void Client::readResponse()
{
	asio::async_read_until(
			m_socket, asio::dynamic_buffer(data_), '\n',
			[this](std::error_code errorCode, std::size_t length)
			{
				if(!errorCode)
				{
					std::cout << "[Client] Data received: " << length
							  << " bytes." << std::endl;
					std::string message(data_.substr(0, length));
					std::cout << "[Client] Message received: " << message
							  << std::endl;
					data_.erase(0, length); // Clear the processed data
					readResponse();			// Initiate another read operation
				}
				else
				{
					std::cerr << "[Client] Read failed: " << errorCode.message()
							  << std::endl;
				}
			});
}

void Client::login(const std::string &userName)
{
	auto decode = ResponseDecoder::makeCollable();

	std::string test("1|" + userName + "||");

	sendRequest(decode(test));
}

void Client::sendMessage(const std::string &sender,
						 const std::string &recipient,
						 const std::string &message)
{
	auto decode = ResponseDecoder::makeCollable();

	std::string test("3|" + sender + "|" + recipient + "|" + message);

	sendRequest(decode(test));
}

void Client::sendRequest(MessagePtr msg)
{
	std::string request(std::to_string(static_cast<int>(msg->messageType_))
						+ "|" + msg->senderName_ + "|" + msg->receiverName_
						+ "|" + msg->body_);
	std::cout << "Sending" << request << "\n";
	asio::write(m_socket, asio::buffer(request + "\n"));
}

void Client::run()
{
	std::lock_guard<std::mutex> lockGuard(m_mutex);
	while(m_socket.is_open())
	{
		m_context.restart();
		m_context.run();
	}
}

int main()
{
	std::shared_ptr<Client> client = std::make_shared<Client>();
	client->connect("127.0.0.1", "9000");
	std::thread ioThread(
			[&] { client->run(); }); // Run io_context in a separate thread

	std::string username;
	std::cout << "Enter your username: ";
	std::cin >> username;

	client->login(username);

	std::string receiver, mess;
	std::cout << std::endl;
	std::cout << "Enter recipient (or 'exit' to logout): ";
	std::cin >> receiver;

	while(true)
	{
		std::cout << "Enter your message: ";
		std::cin.ignore();
		std::getline(std::cin, mess);
		client->sendMessage(username, receiver, mess);
	}

	ioThread.join();

	return 0;
}