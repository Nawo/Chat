#include "ServerLib.h"

ServerLib::ServerLib(std::shared_ptr<IServer> server, ResponseDecoder::CallResponseDecoder responseDecoder,
					 ResponseCoder::CallResponseCoder responseCoder)
	: server_(std::move(server)), responseDecoder_(std::move(responseDecoder)), responseCoder_(std::move(responseCoder))
{
}

void ServerLib::Update()
{
	server_->GetIncomingMessages().wait();

	if(!server_->GetIncomingMessages().empty())
	{
		auto incomingMessage = server_->GetIncomingMessages().pop_front();

		auto decodedMessage = responseDecoder_(incomingMessage.first);

		const MessageType &msgType = decodedMessage->getMessageType();

		switch(msgType)
		{
		case MessageType::Establish:

			OnClientLogin(std::move(decodedMessage), std::move(incomingMessage.second));
			break;

		case MessageType::Relinquish:

			OnClientUnlogin(std::move(decodedMessage), std::move(incomingMessage.second));
			break;

		case MessageType::Message:

			OnClientMessage(std::move(decodedMessage), std::move(incomingMessage.second));
			break;

		case MessageType::MessageAll:

			OnClientMessageAll(std::move(decodedMessage), std::move(incomingMessage.second));
			break;

		case MessageType::PrintUsers:

			OnShowAvailableUsers(std::move(decodedMessage), std::move(incomingMessage.second));
			break;

		default:

			std::cout << "received unknown protocol message, start retrying... " << std::endl;
		}
	}
}

void ServerLib::OnClientLogin(std::shared_ptr<MessageContext> &&decodedMessage, std::shared_ptr<Session> &&messageOwner)
{
	std::string username = decodedMessage->getSenderName();

	messageOwner->setSessionOwner(username);

	std::cout << username + " join the chat \n";
	std::cout << "IP: " + messageOwner->getSessionIP() + "\n";

	messageOwner->Send(
			responseCoder_(MessageType::Message, "SERVER", username, PredefinedMessages::successClientLogin));
}

void ServerLib::OnClientUnlogin(std::shared_ptr<MessageContext> &&decodedMessage,
								std::shared_ptr<Session> &&messageOwner)
{
	std::string username = messageOwner->getSessionOwner();

	std::cout << username + " left the chat." + "\n";

	messageOwner->Send(
			responseCoder_(MessageType::Message, "SERVER", username, PredefinedMessages::successClientDisconnect));
}

void ServerLib::OnClientMessage(std::shared_ptr<MessageContext> &&decodedMessage,
								std::shared_ptr<Session> &&messageOwner)
{
	std::string username = messageOwner->getSessionOwner();

	std::shared_ptr<Session> recipientSession = server_->getSessionByUsername(decodedMessage->getReceiverName());
	if(recipientSession != nullptr)
	{
		std::cout << username + " sent message to " + decodedMessage->getReceiverName() + "\n";
		std::cout << "Message: " + decodedMessage->getBody() + "\n";

		recipientSession->Send(responseCoder_(MessageType::Message, username, decodedMessage->getReceiverName(),
											  decodedMessage->getBody()));
	}

	else // user not found in active sessions
	{
		std::cout << username + " tried to send message to " + decodedMessage->getReceiverName()
							 + " but it is [OFFLINE]" + "\n";
		std::cout << "Message: " + decodedMessage->getBody() + "\n";

		std::string errorMsg = decodedMessage->getReceiverName() + " is OFFLINE";

		messageOwner->Send(responseCoder_(MessageType::Error, "SERVER", username, errorMsg));
	}
}

void ServerLib::OnClientMessageAll(std::shared_ptr<MessageContext> &&decodedMessage,
								   std::shared_ptr<Session> &&messageOwner)
{
	std::string username = messageOwner->getSessionOwner();

	for(const auto &session : server_->getActiveSessions())
	{
		if(session->getSessionOwner() != username)
		{
			std::cout << username + " sent message to all\n";
			std::cout << "Message: " + decodedMessage->getBody() + "\n";

			session->Send(responseCoder_(MessageType::Message, username, "all", decodedMessage->getBody()));
		}
	}
}

void ServerLib::OnShowAvailableUsers(std::shared_ptr<MessageContext> &&decodedMessage,
									 std::shared_ptr<Session> &&messageOwner)
{
	std::string username = messageOwner->getSessionOwner();
	std::string activeUsers{"Active users"};

	for(const auto &session : server_->getActiveSessions())
	{
		if(session->getSessionOwner() == username)
			continue;

		activeUsers += ":" + session->getSessionOwner();
	}

	if(!activeUsers.empty())
		messageOwner->Send(responseCoder_(MessageType::PrintUsers, "SERVER", username, activeUsers));
}

const bool ServerLib::Start() const
{
	return server_->Start();
}