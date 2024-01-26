#include "../Core/net_chat.h"

#include <iostream>
#include <map>
#include <memory>
#include <string>
#include <vector>

enum class CustomMsgTypes : uint32_t
{
	ServerAccept,
	ServerDeny,
	ServerPing,
	MessageAll,
	ServerMessage,
};

class CustomServer : public chat::net::server_interface<CustomMsgTypes>
{
public:
	CustomServer(uint16_t nPort)
		: chat::net::server_interface<CustomMsgTypes>(nPort)
	{
	}

protected:
	virtual bool OnClientConnect(
			std::shared_ptr<chat::net::connection<CustomMsgTypes>> client)
	{
		chat::net::message<CustomMsgTypes> msg;
		msg.header.id = CustomMsgTypes::ServerAccept;
		client->Send(msg);
		return true;
	}

	// Called when a client appears to have disconnected
	virtual void OnClientDisconnect(
			std::shared_ptr<chat::net::connection<CustomMsgTypes>> client)
	{
		std::cout << "Removing client [" << client->GetID() << "]\n";
	}

	// Called when a message arrives
	virtual void
	OnMessage(std::shared_ptr<chat::net::connection<CustomMsgTypes>> client,
			  chat::net::message<CustomMsgTypes> &msg)
	{
		switch(msg.header.id)
		{
		case CustomMsgTypes::ServerPing:
		{
			std::cout << "[" << client->GetID() << "]: Server Ping\n";

			// Simply bounce message back to client
			client->Send(msg);
		}
		break;

		case CustomMsgTypes::MessageAll:
		{
			std::cout << "[" << client->GetID() << "]: ";
			for(const auto &a : msg.body)
			{
				std::cout << a;
			}
			std::cout << std::endl;

			// Construct a new message and send it to all clients

			MessageAllClients(msg, client);
		}
		break;
		}
	}
};

class Session : public std::enable_shared_from_this<Session>
{
public:
	Session(asio::ip::tcp::socket socket,
			std::map<std::string, std::shared_ptr<Session>> &sessions)
		: socket_(std::move(socket)), activeSessions_(sessions)
	{
	}
	void start()
	{
		receive();
	}

private:
	void send(const std::string &data)
	{
		std::cout << "About to send to " << username_ << ": " << data
				  << std::endl;

		auto self(shared_from_this());
		asio::async_write(socket_, asio::buffer(data + "\n"),
						  [this, self, data](std::error_code errorCode,
											 std::size_t length)
						  {
							  if(errorCode)
							  {
								  std::cerr << "Error sending to " << username_
											<< ": " << errorCode.message()
											<< std::endl;
							  }
							  else
							  {
								  std::cout << "Sent " << length << " bytes to "
											<< username_ << ": " << data
											<< std::endl;
							  }
						  });
	}
	void receive()
	{
		auto self(shared_from_this());
		asio::async_read_until(
				socket_, asio::dynamic_buffer(data_), '\n',
				[this, self](std::error_code errorCode, std::size_t length)
				{
					if(!errorCode)
					{
						std::string line(data_.substr(0, length - 1));
						data_.erase(0, length);

						std::string type, sender, recipient, content;
						std::istringstream iss(line);
						std::getline(iss, type, '|');
						std::getline(iss, sender, '|');
						std::getline(iss, recipient, '|');
						std::getline(iss, content, '|');

						if(type == "ESTABLISH")
						{
							std::cout << "successfully established session "
										 "with " + sender
									  << std::endl;
							username_ = sender;
							activeSessions_[username_] = self;
						}
						else if(type == "RELINQUISH")
						{
							std::cout << "successfully closed session with "
												 + sender
									  << std::endl;
							activeSessions_.erase(username_);
						}
						else if(type == "MESSAGE")
						{
							std::cout << "Current active sessions: ";
							for(const auto &pair : activeSessions_)
							{
								std::cout << pair.first << " ";
							}
							std::cout << std::endl;

							auto recipientSession =
									activeSessions_.find(recipient);
							if(recipientSession != activeSessions_.end())
							{
								std::cout << "got message from " + sender
													 + ", forwarding "
													   "message to "
													 + recipient
										  << std::endl;
								std::string forwardMessage = "MESSAGE|" + sender
															 + "|" + recipient
															 + "|" + content;
								recipientSession->second->send(forwardMessage);
							}
							else
								std::cout << "got message from " + sender
													 + ", but recipient: "
													 + recipient + " is offline"
										  << std::endl;
						}
						else
							std::cout << "received unknown protocol "
										 "message, start retrying... "
									  << std::endl;
						receive();
					}
					else
					{
						std::cout << "connection lost" << std::endl;
						activeSessions_.erase(username_);
					}
				});
	}

	std::map<std::string, std::shared_ptr<Session>> &activeSessions_;
	asio::ip::tcp::socket socket_;
	std::string data_;
	std::string username_;
};

class Server
{
public:
	Server()
		: acceptor_(ioContext_,
					asio::ip::tcp::endpoint(asio::ip::tcp::v4(), 8080))
	{
	}

	void start()
	{
		std::cout << "Server started on port 8080" << std::endl;
		acceptConnection();
		ioContext_.run();
	}
	void stop()
	{
		ioContext_.stop();
		std::cout << "Server stopped" << std::endl;
	}

private:
	void acceptConnection()
	{
		acceptor_.async_accept(
				[this](std::error_code errorCode, asio::ip::tcp::socket socket)
				{
					if(!errorCode)
					{
						auto session = std::make_shared<Session>(
								std::move(socket), activeSessions_);
						session->start();
					}
					acceptConnection();
				});
	}

	asio::io_context ioContext_;
	asio::ip::tcp::acceptor acceptor_;
	std::map<std::string, std::shared_ptr<Session>> activeSessions_;
};

int main()
{
	CustomServer server(60000);
	server.Start();

	while(1)
	{
		server.Update(-1, true);
	}

	// while(true)
	// {
	// 	auto server_ = std::make_shared<Server>();
	// 	server_->start();
	// }

	return 0;
}