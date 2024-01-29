#pragma once

// forward declaration
struct MessageContext;
using MessagePtr = std::shared_ptr<MessageContext>;

class IClient
{
public:
	/**
	 * @returns true, if successfull connect, false if not
	 *
	 * @note This function have to be called to connect with server,
	 * as host we give IP (e.g. 123.456.789.1), and port which server listening
	 */
	virtual bool connect(const std::string &host, const std::string &port) = 0;
	virtual void run() = 0;

	virtual bool login(const std::string &userName) = 0;
	virtual void disconnect() = 0;

	virtual bool sendMessage(const std::string &sender, const std::string &recipient, const std::string &message) = 0;

	virtual void readResponse() = 0;
};
using ClientPtr = std::shared_ptr<IClient>;
