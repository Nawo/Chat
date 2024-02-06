#pragma once

#include "../Interface/IServer.h"
#include "Session.h"

class Server : public IServer
{
public:
	Server() = delete;

	Server(uint16_t port) : m_acceptator(m_context, asio::ip::tcp::endpoint(asio::ip::tcp::v4(), port))
	{
	}
	~Server()
	{
		Stop();
	}

	const bool Start() override
	{
		try
		{
			AcceptConnections();
			m_thread = std::thread([this]() { m_context.run(); });
		}
		catch(std::exception &e)
		{
			std::cerr << "[ERROR] " << e.what() << "\n";
			return false;
		}

		return true;
	}

	void Stop() override
	{
		m_context.stop();

		if(m_thread.joinable())
		{
			m_thread.join();
		}
	}
	void AcceptConnections() override
	{
		m_acceptator.async_accept(
				[this](std::error_code errorCode, asio::ip::tcp::socket socket)
				{
					if(!errorCode)
					{
						auto session = std::make_shared<Session>(std::move(socket), m_activeSessions);
						session->Start();
					}
					AcceptConnections();
				});
	}

private:
	asio::io_context m_context;
	std::thread m_thread;
	asio::ip::tcp::acceptor m_acceptator;
	std::map<std::string, std::shared_ptr<Session>> m_activeSessions;
};