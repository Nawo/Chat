#pragma once

#include <iostream>
#include <memory>
#include <optional>
#include <string>

namespace chat::connection
{
	enum class MessageType : int
	{
		Establish = 1,
		Relinquish = 2,
		Message = 3
	};

	struct Message
	{
		Message(const MessageType messageType, const std::string &senderName,
				const std::string &receiverName, const std::string &body)
			: messageType_(messageType), senderName_(senderName),
			  receiverName_(receiverName), body_(body)
		{
		}

		MessageType messageType_;
		std::string senderName_;
		std::string receiverName_;
		std::string body_;
	};

	using MessagePtr = std::shared_ptr<Message>;
} // namespace chat::connection
