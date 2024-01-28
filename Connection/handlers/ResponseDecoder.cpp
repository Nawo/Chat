
#include "ResponseDecoder.hpp"

namespace chat::connection::handlers
{
	MessagePtr ResponseDecoder::decodeResponse(const std::string &response)
	{
		std::istringstream iss(response);
		std::string type, sender, receiver, body;

		std::getline(iss, type, '|');
		std::getline(iss, sender, '|');
		std::getline(iss, receiver, '|');
		std::getline(iss, body, '|');

		return std::make_shared<Message>(
				static_cast<MessageType>(std::stoi(type)), sender, receiver,
				body);
	}

	ResponseDecoder::CallResponseDecoder ResponseDecoder::makeCollable()
	{
		return [&](const std::string &response)
		{
			auto decoder = ResponseDecoder();
			return decoder.decodeResponse(response);
		};
	}

} // namespace chat::connection::handlers