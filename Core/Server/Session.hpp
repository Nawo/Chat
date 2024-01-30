#pragma once

#include "../Common/Common.h"
#include "../Handlers/ResponseCoder.hpp"
#include "../Handlers/ResponseDecoder.hpp"
#include "../Interface/ISession.hpp"

class Session : public ISession, public std::enable_shared_from_this<Session>
{
public:
	explicit Session(asio::ip::tcp::socket socket, std::map<std::string, std::shared_ptr<Session>> &sessions);
	void start() override;

private:
	void request(const std::string &request) override;
	void response() override;

	std::string data_;

	asio::ip::tcp::socket m_socket;									   // Handles client socket
	std::string username_;											   // Handles client username
	std::map<std::string, std::shared_ptr<Session>> &m_activeSessions; // Reference to all server's sessions
};