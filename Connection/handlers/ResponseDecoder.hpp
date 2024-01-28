#pragma once

#include "../common/Message.hpp"

#include <functional>
#include <iostream>
#include <memory>
#include <sstream>
#include <string>

namespace chat::connection::handlers
{

	class ResponseDecoder final
	{
	public:
		using CallResponseDecoder =
				std::function<MessagePtr(const std::string &)>;
		ResponseDecoder() = default;

		static CallResponseDecoder makeCollable();

	private:
		MessagePtr decodeResponse(const std::string &reponse);
	};

} // namespace chat::connection::handlers
