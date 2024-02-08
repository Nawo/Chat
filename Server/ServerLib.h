#pragma once

#include "../Core/Handlers/ResponseCoder.h"
#include "../Core/Handlers/ResponseDecoder.h"
#include "../Core/Server/Server.h"
#include "../lib/sqlite3/sqlite3.h"

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