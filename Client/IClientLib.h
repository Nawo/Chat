#pragma once

#include <Common.h>
#include <MessageContext.h>
#include <MessageType.h>

class IClientLib
{
public:
	virtual bool login(const std::string &userName) = 0;

	virtual bool unlogin() = 0;
	virtual bool printUsers() = 0;

	virtual bool sendMessage(const std::string &sender, const std::string &recipient, const std::string &message) = 0;

	virtual bool sendMessageToAll(const std::string &message) = 0;

	virtual bool connect(const std::string &host, const std::string &port) const = 0;

	virtual bool isConnected() const = 0;

	virtual MessageContext readMessage() = 0;
};