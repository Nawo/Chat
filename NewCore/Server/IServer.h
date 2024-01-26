#pragma once

#include "Session.h"

class Server
{
public:
	Server();
	void start();
	void stop();

private:
	void acceptConnection();

	asio::io_context m_context;
	asio::ip::tcp::acceptor m_acceptator;
	std::map<std::string, std::shared_ptr<Session>> m_activeSessions;
};