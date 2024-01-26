#pragma once

#include "../Common/Common.h"

class Session : public std::enable_shared_from_this<Session>
{
public:
	explicit Session(asio::ip::tcp::socket socket,
					 std::map<std::string, std::shared_ptr<Session>> &sessions);
	void start();

private:
	void send(const Message &data);
	void receive();

	std::string m_username;
	asio::ip::tcp::socket m_socket;
	Message m_data;
	std::map<std::string, std::shared_ptr<Session>> &m_activeSessions;
};