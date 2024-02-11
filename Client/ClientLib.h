#pragma once

#include <Client.h>
#include <Common.h>
#include <IClientLib.h>
#include <MessageContext.h>
#include <MessageType.h>
#include <ResponseCoder.h>
#include <ResponseDecoder.h>
#include <sqlite3.h>

class ClientLib : public IClientLib
{
public:
	ClientLib(std::shared_ptr<IClient<std::string>> client, ResponseDecoder::CallResponseDecoder responseDecoder,
			  ResponseCoder::CallResponseCoder responseCoder);

	bool login(const std::string &userName) override;

	bool unlogin() override;
	bool printUsers() override;

	bool sendMessage(const std::string &sender, const std::string &recipient, const std::string &message) override;

	bool sendMessageToAll(const std::string &message) override;

	bool connect(const std::string &host, const std::string &port) const override;

	bool isConnected() const override;

	MessageContext readMessage() override;

private:
	std::shared_ptr<IClient<std::string>> client_;

	ResponseDecoder::CallResponseDecoder responseDecoder_;
	ResponseCoder::CallResponseCoder responseCoder_;
};
