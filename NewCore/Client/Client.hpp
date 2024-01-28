#pragma once

#include "../Common/Common.h"
#include "../Common/MessageContext.hpp"
#include "../Interface/IClient.hpp"

class Client : public IClient
{
public:
	Client();
	~Client();
	void connect(const std::string &host, const std::string &port) override;

	void login(const std::string &userName);
	void sendMessage(const std::string &sender, const std::string &recipient,
					 const std::string &message) override;

	void readResponse() override;
	void sendRequest(MessagePtr msg) override;

	void run() override;

private:
	asio::io_context m_context;
	asio::ip::tcp::socket m_socket;
	std::string data_;
	std::mutex m_mutex;
};