#include "IServer.h"

Server::Server()
	: m_acceptator(m_context,
				   asio::ip::tcp::endpoint(asio::ip::tcp::v4(), 8080))
{
}

void Server::acceptConnection()
{
	m_acceptator.async_accept(
			[this](std::error_code errorCode, asio::ip::tcp::socket socket)
			{
				if(!errorCode)
				{
					auto session = std::make_shared<Session>(std::move(socket),
															 m_activeSessions);
					session->start();
				}
				acceptConnection();
			});
}

void Server::start()
{
	std::cout << "Server started on port 8080" << std::endl;
	acceptConnection();
	m_context.run();
}

void Server::stop()
{
	m_context.stop();
	std::cout << "Server stopped" << std::endl;
}