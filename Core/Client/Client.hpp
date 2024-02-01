#pragma once

#include "../Common/Common.h"
#include "../Handlers/ResponseCoder.hpp"
#include "../Handlers/ResponseDecoder.hpp"
#include "../Interface/IClient.hpp"

class Client : public IClient<std::string>
{
public:
	Client();
	~Client();

	const bool Run() override;

	const bool Connect(const std::string &host, const std::string &port) override;

	const bool Disconnect() override;

	const bool IsConnected() const override;

	const bool Send(const std::string &msg) override;

	const std::string Read() override;

	bool login(const std::string &userName);

	bool sendMessage(const std::string &sender, const std::string &recipient, const std::string &message);

private:
	std::thread m_thread;
	asio::io_context m_context;
	asio::ip::tcp::socket m_socket;
	std::string m_data;
};