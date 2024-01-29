#pragma once

// forward declaration
struct MessageContext;
using MessagePtr = std::shared_ptr<MessageContext>;

class IClient
{
public:
	virtual void connect(const std::string &host, const std::string &port) = 0;

	virtual void login(const std::string &userName) = 0;
	virtual void sendMessage(const std::string &sender,
							 const std::string &recipient,
							 const std::string &message) = 0;

	virtual void readResponse() = 0;
	virtual void sendRequest(MessagePtr msg) = 0;

	virtual void run() = 0;
};
using ClientPtr = std::shared_ptr<IClient>;
