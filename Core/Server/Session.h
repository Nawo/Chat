#pragma once

#include "../Common/Asio.h"
#include "../Common/Common.h"
#include "../Common/TSqueue.h"
#include "../Handlers/ResponseCoder.h"
#include "../Handlers/ResponseDecoder.h"
#include "../Interface/ISession.h"

class Session : public ISession, public std::enable_shared_from_this<Session>
{
public:
	explicit Session(asio::ip::tcp::socket socket, std::map<std::string, std::shared_ptr<Session>> &sessions)
		: m_socket(std::move(socket)), m_activeSessions(sessions)
	{
	}

	void Start() override
	{
		Read();
	}

private:
	void Send(const std::string &msg) override
	{
		auto self(shared_from_this());
		asio::async_write(m_socket, asio::buffer(msg + "\n"),
						  [this, self, msg](std::error_code errorCode, std::size_t length)
						  {
							  if(errorCode)
							  {
								  std::cerr << "[ERROR] " + errorCode.message() << std::endl;
							  }
						  });
	}

	void Read() override
	{
		auto self(shared_from_this());

		asio::async_read_until(m_socket, asio::dynamic_buffer(m_data), "\n",
							   [self, this](std::error_code errorCode, std::size_t length)
							   {
								   if(!errorCode)
								   {
									   std::string msg(m_data.substr(0, length - 1));
									   m_data.erase(0, length);

									   Update(msg);
									   Read();
								   }
								   else
								   {
									   std::cout << "[ERROR]" + errorCode.message() << std::endl;
									   m_activeSessions.erase(m_sessionOwner);
								   }
							   });
	}

	void Update(const std::string &msg)
	{
		std::shared_ptr<MessageContext> decodedMessage = ResponseDecoder::makeCollable()(msg);

		const MessageType &msgType = decodedMessage->getMessageType();

		switch(msgType)
		{
		case MessageType::Establish:

			OnClientLogin(std::move(decodedMessage));
			break;

		case MessageType::Relinquish:

			OnClientUnlogin(std::move(decodedMessage));
			break;

		case MessageType::Message:

			OnClientMessage(std::move(decodedMessage));
			break;

		case MessageType::MessageAll:

			OnClientMessageAll(std::move(decodedMessage));
			break;

		default:

			std::cout << "received unknown protocol message, start retrying... " << std::endl;
		}
	}

	void OnClientLogin(std::shared_ptr<MessageContext> &&decodedMessage)
	{
		std::shared_ptr<Session> self(shared_from_this());

		m_sessionOwner = decodedMessage->getSenderName();

		m_activeSessions[m_sessionOwner] = self;

		std::cout << m_sessionOwner + " join the chat \n";
		std::cout << "IP: " + m_socket.remote_endpoint().address().to_string() + "\n";

		Send(ResponseCoder::makeCollable()(MessageType::Message, "SERVER", m_sessionOwner,
										   PredefinedMessages::successClientLogin));
	}

	void OnClientUnlogin(std::shared_ptr<MessageContext> &&decodedMessage)
	{
		m_activeSessions.erase(m_sessionOwner);

		std::cout << m_sessionOwner + " left the chat." + "\n";

		Send(ResponseCoder::makeCollable()(MessageType::Message, "SERVER", m_sessionOwner,
										   PredefinedMessages::successClientDisconnect));
	}

	void OnClientMessage(std::shared_ptr<MessageContext> &&decodedMessage)
	{
		auto recipientSession = m_activeSessions.find(decodedMessage->getReceiverName());
		if(recipientSession != m_activeSessions.end())
		{
			std::cout << m_sessionOwner + " sent message to " + decodedMessage->getReceiverName() + "\n";
			std::cout << "Message: " + decodedMessage->getBody() + "\n";

			recipientSession->second->Send(ResponseCoder::makeCollable()(MessageType::Message, m_sessionOwner,
																		 decodedMessage->getReceiverName(),
																		 decodedMessage->getBody()));
		}

		else // user not found in m_activeSessions
		{
			std::cout << m_sessionOwner + " tried to send message to " + decodedMessage->getReceiverName()
								 + " but it is [OFFLINE]" + "\n";
			std::cout << "Message: " + decodedMessage->getBody() + "\n";

			std::string errorMsg = decodedMessage->getReceiverName() + " is OFFLINE";

			std::string codedMessage =
					ResponseCoder::makeCollable()(MessageType::Error, "SERVER", m_sessionOwner, errorMsg);

			Send(codedMessage);
		}
	}

	void OnClientMessageAll(std::shared_ptr<MessageContext> &&decodedMessage)
	{
		for(const auto &[clientName, clientSocket] : m_activeSessions)
		{
			if(clientName != m_sessionOwner)
			{
				std::cout << m_sessionOwner + " sent message to all\n";
				std::cout << "Message: " + decodedMessage->getBody() + "\n";

				clientSocket->Send(ResponseCoder::makeCollable()(MessageType::Message, m_sessionOwner, "all",
																 decodedMessage->getBody()));
			}
		}
	}

	std::string m_data;
	asio::ip::tcp::socket m_socket;									   // Handles client socket
	std::string m_sessionOwner;										   // Handles client username
	std::map<std::string, std::shared_ptr<Session>> &m_activeSessions; // Reference to all server's sessions
};