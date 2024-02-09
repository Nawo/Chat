#pragma once

#include <Client.h>
#include <Common.h>
#include <MessageContext.h>
#include <MessageType.h>
#include <ResponseCoder.h>
#include <ResponseDecoder.h>
#include <sqlite3.h>

class ClientLib : public Client
{
public:
	bool login(const std::string &userName);

	bool unlogin();
	bool printUsers();

	bool sendMessage(const std::string &sender, const std::string &recipient, const std::string &message);

	bool sendMessageToAll(const std::string &message);

	MessageContext readMessage();
};