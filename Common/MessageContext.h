#pragma once

#include "MessageType.h"

struct MessageContext
{
	MessageContext(const std::string &messageType, const std::string &senderName, const std::string &receiverName,
				   const std::string &body)
		: messageType_(static_cast<MessageType>(std::stoi(messageType))), senderName_(senderName),
		  receiverName_(receiverName), body_(body)
	{
	}

	const MessageType &getMessageType() const
	{
		return messageType_;
	}

	const std::string &getSenderName() const
	{
		return senderName_;
	}

	const std::string &getReceiverName() const
	{
		return receiverName_;
	}

	const std::string &getBody() const
	{
		return body_;
	}

private:
	const MessageType messageType_;
	const std::string senderName_;
	const std::string receiverName_;
	const std::string body_;
};
