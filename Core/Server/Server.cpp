#include "Server.h"

Server::Server(uint16_t port) : m_acceptator(m_context, asio::ip::tcp::endpoint(asio::ip::tcp::v4(), port))
{
}

Server::~Server()
{
	stop();
}

bool Server::start()
{
	try
	{
		acceptConnection();
		m_thread = std::thread([this]() { m_context.run(); });
	}
	catch(std::exception &e)
	{
		std::cerr << "[SERVER] Exception: " << e.what() << "\n";
		return false;
	}

	std::cout << "Server started." << std::endl;
	return true;
}

void Server::stop()
{
	m_context.stop();

	if(m_thread.joinable())
	{
		m_thread.join();
	}

	std::cout << "Server stopped" << std::endl;
}

void Server::acceptConnection()
{
	m_acceptator.async_accept(
			[this](std::error_code errorCode, asio::ip::tcp::socket socket)
			{
				if(!errorCode)
				{
					auto session = std::make_shared<Session>(std::move(socket), m_activeSessions);
					session->start();
				}
				acceptConnection();
			});
}

int main()
{
	std::shared_ptr<Server> server = std::make_shared<Server>(9000);
	server->start();
	while(true)
	{
	}
}