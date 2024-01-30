#pragma once

#include "../Common/Common.h"
#include "../Handlers/ResponseDecoder.hpp"
#include "../Interface/IClient.hpp"

class Client : public IClient
{
public:
	Client();
	~Client();
	bool connect(const std::string &host, const std::string &port) override;
	void run() override;

	bool login(const std::string &userName) override;
	void disconnect() override;

	bool sendMessage(const std::string &sender, const std::string &recipient, const std::string &message) override;

	void readResponse() override;

private:
	void sendRequest(MessagePtr msg);

	std::thread thrContext;
	asio::io_context m_context;
	asio::ip::tcp::socket m_socket;
	std::string data_;
	std::mutex m_mutex;
};