#pragma once

#include "../Common/TSqueue.h"
#include "../Interface/ISession.h"
#include "Session.h"
class IServer
{
public:
	virtual const bool Start() = 0;

	virtual void Stop() = 0;

	virtual tsqueue<std::pair<std::string, std::shared_ptr<Session>>> &GetIncomingMessages() = 0;
	virtual const std::shared_ptr<Session> &getSessionByUsername(const std::string &username) = 0;
	virtual const std::vector<std::shared_ptr<Session>> &getActiveSessions() const = 0;

private:
	virtual void AcceptConnections() = 0;
};