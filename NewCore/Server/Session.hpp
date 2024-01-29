#pragma once

#include "../Common/Common.h"
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

private:
	asio::ip::tcp::socket m_socket;
	std::string data_;
	std::string username_;
	std::map<std::string, std::shared_ptr<Session>> &m_activeSessions;
};