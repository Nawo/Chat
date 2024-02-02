#pragma once

#include "../Common/Common.h"
#include "../Common/MessageContext.h"
#include "../Common/MessageType.h"

class ResponseCoder
{
public:
	using CallResponseCoder = std::function<std::string(const MessageType &, const std::string &, const std::string &,
														const std::string &)>;

	static CallResponseCoder makeCollable()
	{
		return [&](const MessageType &msgType, const std::string &sender, const std::string &receiver,
				   const std::string &body)
		{
			auto decoder = ResponseCoder();
			return decoder.codeResponse(msgType, sender, receiver, body);
		};
	}

private:
	std::string codeResponse(const MessageType &msgType, const std::string &sender, const std::string &receiver,
							 const std::string &body)
	{
		std::string codedMessage =
				std::to_string(static_cast<int>(msgType)) + "|" + sender + "|" + receiver + "|" + body;

		return codedMessage;
	}
};
