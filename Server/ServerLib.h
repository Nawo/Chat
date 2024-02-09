#pragma once

#include <Common.h>
#include <MessageContext.h>
#include <MessageType.h>
#include <ResponseCoder.h>
#include <ResponseDecoder.h>
#include <Server.h>
#include <sqlite3.h>

class ServerLib : public Server
{
public:
	using Server::Server;

	void Update();

	void OnClientLogin(std::shared_ptr<MessageContext> &&decodedMessage, std::shared_ptr<Session> &&messageOwner);

	void OnClientUnlogin(std::shared_ptr<MessageContext> &&decodedMessage, std::shared_ptr<Session> &&messageOwner);

	void OnClientMessage(std::shared_ptr<MessageContext> &&decodedMessage, std::shared_ptr<Session> &&messageOwner);

	void OnClientMessageAll(std::shared_ptr<MessageContext> &&decodedMessage, std::shared_ptr<Session> &&messageOwner);

	void OnShowAvailableUsers(std::shared_ptr<MessageContext> &&decodedMessage,
							  std::shared_ptr<Session> &&messageOwner);
};