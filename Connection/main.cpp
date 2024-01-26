
#include "ResponseDecoder.hpp"

namespace chat::connection
{

	int main()
	{
		std::string test("1|Dominik|Roman|siema");

		auto decode = ResponseDecoder::makeCollable();

		auto message = decode(test);

		std::cout << message->senderName_ << '\n';
		std::cout << message->receiverName_ << '\n';
		std::cout << message->body_ << '\n';

        return 0;
	}

} // namespace chat::connection