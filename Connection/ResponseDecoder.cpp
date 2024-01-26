
#include "../Connection/ResponseDecoder.hpp"

namespace chat::connection
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
				std::stoi(type), sender,
				(receiver.empty() ? std::nullopt
								  : std::optional<std::string>(receiver),
				 (body.empty() ? std::nullopt
							   : std::optional<std::string>(body))));
	}

	ResponseDecoder::CallResponseDecoder ResponseDecoder::makeCollable()
	{
		return [&](const std::string &response){
			auto decoder = ResponseDecoder();
			decoder.decodeResponse(response);
		};
	}

} // namespace chat::connection