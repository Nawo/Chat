#pragma once

#include "../Common/Common.h"

class Client
{
public:
	Client();
	~Client();
	void connect(const std::string &host, const std::string &port);
	void sendMessage(const Message msg);
	void readMessages();

	void run();

private:
	asio::io_context m_context;
	asio::ip::tcp::socket m_socket;
	Message m_data;
	std::mutex m_mutex;
};