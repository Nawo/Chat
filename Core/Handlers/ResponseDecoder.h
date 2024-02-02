#pragma once

#include "../Common/Common.h"
#include "../Common/MessageContext.h"
#include "../Common/MessageType.h"

class ResponseDecoder
{
public:
	using CallResponseDecoder = std::function<std::shared_ptr<MessageContext>(const std::string &)>;

	static CallResponseDecoder makeCollable()
	{
		return [&](const std::string &response)
		{
			auto decoder = ResponseDecoder();
			return decoder.decodeResponse(response);
		};
	}

private:
	std::shared_ptr<MessageContext> decodeResponse(const std::string &response)
	{
		std::istringstream iss(response);
		std::string type;
		std::string sender;
		std::string receiver;
		std::string body;

		std::getline(iss, type, '|');
		std::getline(iss, sender, '|');
		std::getline(iss, receiver, '|');
		std::getline(iss, body, '\\');

		return std::make_shared<MessageContext>(type, sender, receiver, body);
	}
};
