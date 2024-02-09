#include "ServerLib.h"

void ServerLib::Update()
{
	GetIncomingMessages().wait();

	if(!GetIncomingMessages().empty())
	{
		auto incomingMessage = GetIncomingMessages().pop_front();

		std::shared_ptr<MessageContext> decodedMessage = ResponseDecoder::makeCollable()(incomingMessage.first);

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

	messageOwner->Send(ResponseCoder::makeCollable()(MessageType::Message, "SERVER", username,
													 PredefinedMessages::successClientLogin));
}

void ServerLib::OnClientUnlogin(std::shared_ptr<MessageContext> &&decodedMessage,
								std::shared_ptr<Session> &&messageOwner)
{
	std::string username = messageOwner->getSessionOwner();

	std::cout << username + " left the chat." + "\n";

	messageOwner->Send(ResponseCoder::makeCollable()(MessageType::Message, "SERVER", username,
													 PredefinedMessages::successClientDisconnect));
}

void ServerLib::OnClientMessage(std::shared_ptr<MessageContext> &&decodedMessage,
								std::shared_ptr<Session> &&messageOwner)
{
	std::string username = messageOwner->getSessionOwner();

	std::shared_ptr<Session> recipientSession = getSessionByUsername(decodedMessage->getReceiverName());
	if(recipientSession != nullptr)
	{
		std::cout << username + " sent message to " + decodedMessage->getReceiverName() + "\n";
		std::cout << "Message: " + decodedMessage->getBody() + "\n";

		recipientSession->Send(ResponseCoder::makeCollable()(
				MessageType::Message, username, decodedMessage->getReceiverName(), decodedMessage->getBody()));
	}

	else // user not found in active sessions
	{
		std::cout << username + " tried to send message to " + decodedMessage->getReceiverName()
							 + " but it is [OFFLINE]" + "\n";
		std::cout << "Message: " + decodedMessage->getBody() + "\n";

		std::string errorMsg = decodedMessage->getReceiverName() + " is OFFLINE";

		std::string codedMessage = ResponseCoder::makeCollable()(MessageType::Error, "SERVER", username, errorMsg);

		messageOwner->Send(codedMessage);
	}
}

void ServerLib::OnClientMessageAll(std::shared_ptr<MessageContext> &&decodedMessage,
								   std::shared_ptr<Session> &&messageOwner)
{
	std::string username = messageOwner->getSessionOwner();

	for(const auto &session : getActiveSessions())
	{
		if(session->getSessionOwner() != username)
		{
			std::cout << username + " sent message to all\n";
			std::cout << "Message: " + decodedMessage->getBody() + "\n";

			session->Send(
					ResponseCoder::makeCollable()(MessageType::Message, username, "all", decodedMessage->getBody()));
		}
	}
}

void ServerLib::OnShowAvailableUsers(std::shared_ptr<MessageContext> &&decodedMessage,
									 std::shared_ptr<Session> &&messageOwner)
{
	std::string username = messageOwner->getSessionOwner();
	std::string activeUsers{"Active users"};

	for(const auto &session : getActiveSessions())
	{
		if(session->getSessionOwner() == username)
			continue;

		activeUsers += ":" + session->getSessionOwner();
	}

	if(!activeUsers.empty())
		messageOwner->Send(ResponseCoder::makeCollable()(MessageType::PrintUsers, username, username, activeUsers));
}

// CODE UNDER WILL BE IN DIFFERENT FILE

int main()
{
	std::shared_ptr<ServerLib> server = std::make_shared<ServerLib>(9000);
	server->Start();
	std::cout << "[SERVER] started" << std::endl;
	while(true)
	{
		server->Update();
	}
}
