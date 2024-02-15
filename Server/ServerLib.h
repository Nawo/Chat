#pragma once

#include <Common.h>
#include <IServerLib.h>
#include <MessageContext.h>
#include <MessageType.h>
#include <ResponseCoder.h>
#include <ResponseDecoder.h>
#include <Server.h>
#include <Session.h>
#include <sqlite3.h>

class ServerLib : public IServerLib
{
public:
	ServerLib(std::shared_ptr<IServer> server, ResponseDecoder::CallResponseDecoder responseDecoder,
			  ResponseCoder::CallResponseCoder responseCoder);

	void Update() override;

	void OnClientLogin(std::shared_ptr<MessageContext> &&decodedMessage,
					   std::shared_ptr<Session> &&messageOwner) override;

	void OnClientUnlogin(std::shared_ptr<MessageContext> &&decodedMessage,
						 std::shared_ptr<Session> &&messageOwner) override;

	void OnClientMessage(std::shared_ptr<MessageContext> &&decodedMessage,
						 std::shared_ptr<Session> &&messageOwner) override;

	void OnClientMessageAll(std::shared_ptr<MessageContext> &&decodedMessage,
							std::shared_ptr<Session> &&messageOwner) override;

	void OnShowAvailableUsers(std::shared_ptr<MessageContext> &&decodedMessage,
							  std::shared_ptr<Session> &&messageOwner) override;

	virtual const bool Start() const override;

private:
	std::shared_ptr<IServer> server_;
	ResponseDecoder::CallResponseDecoder responseDecoder_;
	ResponseCoder::CallResponseCoder responseCoder_;
};