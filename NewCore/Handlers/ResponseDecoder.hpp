#pragma once

#include "../Common/MessageContext.hpp"
#include "../Common/MessageType.h"

#include <functional>
#include <memory>
#include <sstream>
#include <string>

class ResponseDecoder
{
public:
	using CallResponseDecoder = std::function<MessagePtr(const std::string &)>;

	static CallResponseDecoder makeCollable()
	{
		return [&](const std::string &response)
		{
			auto decoder = ResponseDecoder();
			return decoder.decodeResponse(response);
		};
	}

private:
	// using MessagePtr = std::shared_ptr<MessageContext>;
	MessagePtr decodeResponse(const std::string &response)
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
