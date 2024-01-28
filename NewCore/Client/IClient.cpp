#include "IClient.h"

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
	readMessages();
}

void Client::readMessages()
{
	asio::async_read(
			m_socket, asio::buffer(&m_data, sizeof(m_data)),
			[this](std::error_code errorCode, std::size_t length)
			{
				if(!errorCode)
				{
					std::cout << "[" + m_data.sender + "]: " + m_data.message;
					m_data.clear();
					readMessages(); // Jezeli coś odbierze i przetworzy,
									// niech nasluchuje dalej
				}
				else
				{
					std::cerr << "[Client] Read failed: " << errorCode.message()
							  << std::endl;
				}
			});
}

void Client::sendMessage(const Message msg)
{
	asio::async_write(m_socket, asio::buffer(&msg, sizeof(msg)),
					  [this](std::error_code errorCode, std::size_t length)
					  {
						  if(errorCode)
						  {
							  std::cerr << "[Client] send failed: "
										<< errorCode.message() << std::endl;
						  }
					  });
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

std::atomic<bool> stopReading(false);
std::string input;

void asyncReadChar()
{
	while(!stopReading)
	{
		std::cin >> input;
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
	Message msg;
	msg.type = "login";
	msg.sender = username;
	client->sendMessage(msg);
	msg.clear();

	auto future = std::async(std::launch::async, asyncReadChar);
	bool bQuit = false;
	while(!bQuit)
	{
		std::string mess;
		std::cin >> mess;
		if(input == "quit")
		{
			bQuit = true;
			break;
		}
		Message message;
		message.type = "MessageAll";
		message.message = mess;
		message.sender = username;
		client->sendMessage(message);
		input.clear();
	}

	stopReading = true;

	future.get();
	ioThread.join();

	return 0;
}