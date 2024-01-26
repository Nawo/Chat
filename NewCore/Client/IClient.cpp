#include "IClient.h"

Client::Client() : m_socket(m_context)
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

void Client::sendMessage(const Message &msg)
{
	asio::async_write(m_socket, asio::buffer(&msg, sizeof(msg)),
					  [this](std::error_code errorCode, std::size_t length)
					  {
						  if(!errorCode)
						  {
						  }
						  else
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