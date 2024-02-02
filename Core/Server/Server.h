#pragma once

#include "../Common/Common.h"
#include "../Interface/IServer.h"
#include "Session.h"

class Server : public IServer
{
public:
	Server() = delete;
	Server(uint16_t port);
	~Server();
	bool start();
	void stop();

private:
	void acceptConnection();

	asio::io_context m_context;
	std::thread m_thread;
	asio::ip::tcp::acceptor m_acceptator;
	std::map<std::string, std::shared_ptr<Session>> m_activeSessions;
};