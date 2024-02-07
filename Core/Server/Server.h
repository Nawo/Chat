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
						std::lock_guard<std::mutex> lg(m_mutex);
						m_activeSessions.push_back(std::make_shared<Session>(std::move(socket), m_incomingMessages));
						m_activeSessions.front()->Start();
					}
					AcceptConnections();
				});
	}

	std::shared_ptr<Session> getSessionByUsername(const std::string &username)
	{
		std::lock_guard<std::mutex> lg(m_mutex);
		auto it = std::find_if(m_activeSessions.begin(), m_activeSessions.end(),
							   [username](auto it) -> bool { return it->getSessionOwner() == username; });
		if(it == m_activeSessions.end())
		{
			return nullptr;
		}

		return *it;
	}

	std::vector<std::shared_ptr<Session>> getActiveSessions() const
	{
		return m_activeSessions;
	}

protected:
	tsqueue<std::pair<std::string, std::shared_ptr<Session>>> m_incomingMessages;
	std::vector<std::shared_ptr<Session>> m_activeSessions;

private:
	std::mutex m_mutex;
	asio::io_context m_context;
	std::thread m_thread;
	asio::ip::tcp::acceptor m_acceptator;
};