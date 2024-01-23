#include "../Core/net_chat.h"

#include <iostream>

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

int main()
{
	CustomServer server(60000);
	server.Start();

	while(1)
	{
		server.Update(-1, true);
	}

	return 0;
}