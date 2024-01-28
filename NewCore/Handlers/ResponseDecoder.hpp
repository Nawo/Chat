#pragma once

#include "../common/MessageContext.hpp"

#include <functional>
#include <iostream>
#include <memory>
#include <sstream>
#include <string>

class ResponseDecoder final
{
public:
	using CallResponseDecoder = std::function<MessagePtr(const std::string &)>;
	ResponseDecoder() = default;

	static CallResponseDecoder makeCollable();

private:
	MessagePtr decodeResponse(const std::string &reponse);
};
