#pragma once

#include "common/Common.hpp"
#include "common/Message.hpp"
#include "handlers/ResponseDecoder.hpp"

#include <string>
#include <thread>

class Client
{
public:
	Client();
	void connect(const std::string &host, const std::string &port);
	void login(const std::string &username);
	void sendMessage(const std::string &sender, const std::string &recipient,
					 const std::string &message);
	void checkDataAvailability();
	void logout();
	void run();

private:
	asio::io_context io_context_;
	asio::ip::tcp::socket socket_;
	std::string data_;
	std::mutex mtx_;

	void readData();
	void sendData(const std::string &data);
};