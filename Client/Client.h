#pragma once

#include "../Core/Client/Client.h"
#include "../Core/Handlers/ResponseCoder.h"
#include "../Core/Handlers/ResponseDecoder.h"
#include "../lib/sqlite3/sqlite3.h"

class ClientApp : public Client
{
public:
	bool login(const std::string &userName);

	bool unlogin();

	bool sendMessage(const std::string &sender, const std::string &recipient, const std::string &message);

	bool sendMessageToAll(const std::string &message);

	std::string readMessage();
};