#pragma once

#include "../Common/Common.h"
#include "../Interface/IServer.hpp"
#include "Session.hpp"

class Server : public IServer
{
public:
	Server();
	~Server();
	void start();
	void stop();

private:
	void acceptConnection();

	asio::io_context m_context;
	asio::ip::tcp::acceptor m_acceptator;
	std::map<std::string, std::shared_ptr<Session>> m_activeSessions;
};