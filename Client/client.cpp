#include "../Core/net_chat.h"

#include <atomic>
#include <future>
#include <iostream>
#include <mutex>
#include <thread>

enum class CustomMsgTypes : uint32_t
{
	ServerAccept,
	ServerDeny,
	ServerPing,
	MessageAll,
	ServerMessage,
};

class CustomClient : public chat::net::client_interface<CustomMsgTypes>
{
public:
	void PingServer()
	{
		chat::net::message<CustomMsgTypes> msg;
		msg.header.id = CustomMsgTypes::ServerPing;

		// Caution with this...
		std::chrono::system_clock::time_point timeNow =
				std::chrono::system_clock::now();

		msg << timeNow;
		Send(msg);
	}

	void MessageAll(std::string message)
	{
		chat::net::message<CustomMsgTypes> msg;
		msg.header.id = CustomMsgTypes::MessageAll;
		msg.header.senderName = userName;
		msg << message;
		Send(msg);
	}
};

std::atomic<bool> stopReading(false);
std::string input;

void asyncReadChar()
{
	while(!stopReading)
	{
		std::cin >> input;
	}
}
class Client
{
public:
	Client() : socket_(io_context_)
	{
	}
	void connect(const std::string &host, const std::string &port)
	{
		asio::ip::tcp::resolver resolver(io_context_);
		asio::connect(socket_, resolver.resolve(host, port));
		readData();
	}
	void readData()
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
						readData(); // Initiate another read operation
					}
					else
					{
						std::cerr << "[Client] Read failed: "
								  << errorCode.message() << std::endl;
					}
				});
	}
	void sendData(const std::string &data)
	{
		std::cout << "Sending: " << data << std::endl;
		asio::write(socket_, asio::buffer(data + "\n"));
	}

	void login(const std::string &username)
	{
		sendData("ESTABLISH|" + username + "||");
	}

	void sendMessage(const std::string &sender, const std::string &recipient,
					 const std::string &message)
	{
		sendData("MESSAGE|" + sender + "|" + recipient + "|" + message);
	}

	void logout()
	{
		sendData("RELINQUISH|||");
	}

	void run()
	{
		std::lock_guard<std::mutex> lockGuard(mtx_);
		while(socket_.is_open())
		{
			io_context_.restart();
			io_context_.run();
		}
	}

private:
	asio::io_context io_context_;
	asio::ip::tcp::socket socket_;
	std::string data_;
	std::mutex mtx_;
};

int main()
{
	std::cout << "Podaj nick: ";
	std::cin >> userName;

	CustomClient c;
	c.Connect("127.0.0.1", 60000);

	auto future = std::async(std::launch::async, asyncReadChar);

	bool bQuit = false;
	while(!bQuit)
	{
		if(!input.empty())
		{
			c.MessageAll(input);
			input.clear();
		}

		if(c.IsConnected())
		{
			if(!c.Incoming().empty())
			{
				auto msg = c.Incoming().pop_front().msg;

				switch(msg.header.id)
				{
				case CustomMsgTypes::ServerAccept:
				{
					// Server has responded to a ping request
					std::cout << "Server Accepted Connection\n";
				}
				break;

				case CustomMsgTypes::ServerPing:
				{
					// Server has responded to a ping request
					std::chrono::system_clock::time_point timeNow =
							std::chrono::system_clock::now();
					std::chrono::system_clock::time_point timeThen;
					msg >> timeThen;
					std::cout
							<< "Ping: "
							<< std::chrono::duration<double>(timeNow - timeThen)
									   .count()
							<< "\n";
				}
				break;

				case CustomMsgTypes::ServerMessage:
				{
					// Server has responded to a ping request
					uint32_t clientID;
					msg >> clientID;
					std::cout << "Hello from [" << clientID << "]\n";
				}
				break;

				case CustomMsgTypes::MessageAll:
				{
					std::cout << "[" << msg.header.senderName << "]: ";
					std::string str(msg.body.begin(), msg.body.end());
					std::cout << str << std::endl;
				}
				break;
				}
			}
		}

		else
		{
			std::cout << "Server Down\n";
			bQuit = true;
		}
	}

	stopReading = true;

	future.get();
	// Client c;
	// c.connect("127.0.0.1", "8080");
	// std::thread ioThread([&] { c.run(); });
	// std::string username;
	// std::cout << "Enter your username: ";
	// std::cin >> username;
	// c.login(username);
	// std::string recipient, message;
	// std::cout << std::endl;
	// std::cout << "Enter recipient (or 'exit' to logout): ";
	// std::cin >> recipient;

	// while(true)
	// {
	// 	std::cout << "Enter your message: ";
	// 	std::cin.ignore();
	// 	std::getline(std::cin, message);
	// 	c.sendMessage(username, recipient, message);
	// }
	// ioThread.join();

	return 0;
}