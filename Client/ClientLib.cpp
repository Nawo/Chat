#include "ClientLib.h"

bool ClientLib::login(const std::string &userName)
{
	std::string codedMessage = ResponseCoder::makeCollable()(MessageType::Establish, userName, "", "");

	Send(codedMessage);

	return true;
}

bool ClientLib::unlogin()
{
	std::string codedMessage = ResponseCoder::makeCollable()(MessageType::Relinquish, "", "", "");

	Disconnect();

	Send(codedMessage);

	return true;
}

bool ClientLib::printUsers()
{
	std::string codedMessage = ResponseCoder::makeCollable()(MessageType::PrintUsers, "", "", "");

	Send(codedMessage);

	return true;
}

bool ClientLib::sendMessage(const std::string &sender, const std::string &recipient, const std::string &message)
{
	std::string codedMessage = ResponseCoder::makeCollable()(MessageType::Message, sender, recipient, message);

	Send(codedMessage);

	return true;
}

bool ClientLib::sendMessageToAll(const std::string &message)
{
	std::string codedMessage = ResponseCoder::makeCollable()(MessageType::MessageAll, "", "", message);

	Send(codedMessage);

	return true;
}

MessageContext ClientLib::readMessage()
{
	GetIncomingMessages().wait();

	if(!GetIncomingMessages().empty())
	{
		return *ResponseDecoder::makeCollable()(GetIncomingMessages().pop_front()).get();
	}
}