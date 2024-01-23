#include "../Core/net_chat.h"

#include <atomic>
#include <future>
#include <iostream>

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
		for(auto &a : message)
		{
			msg << a;
		}
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

int main()
{
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
					// uint32_t clientID;
					// msg >> clientID;
					// std::cout << "[" << clientID << "]: ";
					for(auto &a : msg.body)
					{
						std::cout << a;
					}
					std::cout << std::endl;
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

	return 0;
}