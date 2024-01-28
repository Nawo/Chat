#include "Client.hpp"

#include <iostream>
#include <thread>

Client::Client() : socket_(io_context_)
{
}

void Client::connect(const std::string &host, const std::string &port)
{
	asio::ip::tcp::resolver resolver(io_context_);
	asio::connect(socket_, resolver.resolve(host, port));
	readData();
}

void Client::readData()
{
	asio::async_read_until(
			socket_, asio::dynamic_buffer(data_), '\n',
			[this](std::error_code errorCode, std::size_t length)
			{
				if(!errorCode)
				{
					std::cout << "[Client] Data received: " << length
							  << " bytes." << std::endl;
					std::string message(data_.substr(0, length));
					std::cout << "[Client] Message received: " << message
							  << std::endl;
					data_.erase(0, length); // Clear the processed data
					readData();				// Initiate another read operation
				}
				else
				{
					std::cerr << "[Client] Read failed: " << errorCode.message()
							  << std::endl;
				}
			});
}

void Client::sendData(const std::string &data)
{
	std::cout << "Sending: " << data << std::endl;
	asio::write(socket_, asio::buffer(data + "\n"));
}

void Client::login(const std::string &username)
{
	sendData("ESTABLISH|" + username + "||");
}

void Client::sendMessage(const std::string &sender,
						 const std::string &recipient,
						 const std::string &message)
{
	sendData("MESSAGE|" + sender + "|" + recipient + "|" + message);
}

void Client::logout()
{
	sendData("RELINQUISH|||");
}

void Client::run()
{
	std::lock_guard<std::mutex> lockGuard(mtx_);
	while(socket_.is_open())
	{
		io_context_.restart();
		io_context_.run();
	}
}