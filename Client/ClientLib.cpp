#include "ClientLib.h"

ClientLib::ClientLib(std::shared_ptr<IClient<std::string>> client, ResponseDecoder::CallResponseDecoder responseDecoder,
					 ResponseCoder::CallResponseCoder responseCoder)
	: client_(std::move(client)), responseDecoder_(std::move(responseDecoder)), responseCoder_(std::move(responseCoder))
{
}

bool ClientLib::login(const std::string &userName)
{
	std::string codedMessage = responseCoder_(MessageType::Establish, userName, "", "");

	client_->Send(codedMessage);

	return true;
}

bool ClientLib::unlogin()
{
	std::string codedMessage = responseCoder_(MessageType::Relinquish, "", "", "");

	client_->Disconnect();

	client_->Send(codedMessage);

	return true;
}

bool ClientLib::printUsers()
{
	std::string codedMessage = responseCoder_(MessageType::PrintUsers, "", "", "");

	client_->Send(codedMessage);

	return true;
}

bool ClientLib::sendMessage(const std::string &sender, const std::string &recipient, const std::string &message)
{
	std::string codedMessage = responseCoder_(MessageType::Message, sender, recipient, message);

	client_->Send(codedMessage);

	return true;
}

bool ClientLib::sendMessageToAll(const std::string &message)
{
	std::string codedMessage = responseCoder_(MessageType::MessageAll, "", "", message);

	client_->Send(codedMessage);

	return true;
}

bool ClientLib::connect(const std::string &host, const std::string &port) const
{
	return client_->Connect(host, port);
}

bool ClientLib::isConnected() const
{
	return client_->IsConnected();
}

MessageContext ClientLib::readMessage()
{
	client_->GetIncomingMessages().wait();

	return *(responseDecoder_(client_->GetIncomingMessages().pop_front()));
}
