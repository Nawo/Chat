#pragma once

#include "../Common/Common.h"
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
								  std::cerr << "[ERROR] " << errorCode.message() << std::endl;
							  }
						  });
	}

	void Read() override
	{
		auto self(shared_from_this());
		asio::async_read_until(
				m_socket, asio::dynamic_buffer(m_data), "\n",
				[this, self](std::error_code errorCode, std::size_t length)
				{
					if(!errorCode)
					{
						std::string line(m_data.substr(0, length - 1));
						m_data.erase(0, length);

						auto decodedMessage = ResponseDecoder::makeCollable()(line);

						m_username = decodedMessage->getSenderName();
						const std::string &receiverName = decodedMessage->getReceiverName();
						const std::string &body = decodedMessage->getBody();
						const MessageType &msgType = decodedMessage->getMessageType();

						switch(msgType)
						{
						case MessageType::Establish:
						{
							m_activeSessions[m_username] = self;

							std::cout << "=====[Log]=====\n";
							std::cout << m_username + " join the chat \n";
							std::cout << "IP: " + m_socket.remote_endpoint().address().to_string() + "\n";
							std::cout << "===============\n";

							Send(ResponseCoder::makeCollable()(MessageType::Message, "SERVER", m_username,
															   PredefinedMessages::successClientLogin));
							break;
						}

						case MessageType::Relinquish:
						{
							m_activeSessions.erase(m_username);

							std::cout << "=====[Log]=====\n";
							std::cout << m_username + " left the chat." + "\n";
							std::cout << "===============\n";

							Send(ResponseCoder::makeCollable()(MessageType::Message, "SERVER", m_username,

															   PredefinedMessages::successClientDisconnect));
							break;
						}

						case MessageType::Message:
						{
							auto recipientSession = m_activeSessions.find(receiverName);
							if(recipientSession != m_activeSessions.end())
							{
								std::cout << "=====[Log]=====\n";
								std::cout << m_username + " sent message to " + receiverName + "\n";
								std::cout << "Bytes: " + std::to_string(length) + "\n";
								std::cout << "Message: " + body + "\n";
								std::cout << "===============\n";

								recipientSession->second->Send(ResponseCoder::makeCollable()(
										MessageType::Message, m_username, receiverName, body));
							}

							else // user not found in m_activeSessions
							{
								std::cout << "=====[Log]=====\n";
								std::cout << m_username + " tried to send message to " + receiverName
													 + " but it is [OFFLINE]" + "\n";
								std::cout << "Bytes: " + std::to_string(length) + "\n";
								std::cout << "Message: " + body + "\n";
								std::cout << "===============\n";

								std::string errorMsg = receiverName + " is OFFLINE";

								std::string codedMessage = ResponseCoder::makeCollable()(MessageType::Error, "SERVER",
																						 m_username, errorMsg);

								Send(codedMessage);
							}

							break;
						}

						case MessageType::MessageAll:
						{
							for(const auto &[clientName, clientSocket] : m_activeSessions)
							{
								if(clientName != m_username)
								{
									std::cout << "=====[Log]=====\n";
									std::cout << m_username + " sent message to all\n";
									std::cout << "Bytes: " + std::to_string(length) + "\n";
									std::cout << "Message: " + body + "\n";
									std::cout << "===============\n";

									clientSocket->Send(ResponseCoder::makeCollable()(MessageType::Message, m_username,
																					 "all", body));
								}
							}
							break;
						}

						default:
						{
							std::cout << "received unknown protocol message, start retrying... " << std::endl;
							break;
						}
						}

						Read();
					}
					else
					{
						std::cout << "connection lost" << std::endl;
						m_activeSessions.erase(m_username);
					}
				});
	}

	// TODO
	// functions to handle message types, e.g. sendMessageToAll, sendMessageToUser, getUsersList
	void SendMessageToAll(const std::string &msg)
	{
	}

	void SendClientSuccessfullLoginMessage(const std::string &msg)
	{
	}

	void SendClientLogoutMessage(const std::string &msg)
	{
	}

	std::string m_data;
	asio::ip::tcp::socket m_socket;									   // Handles client socket
	std::string m_username;											   // Handles client username
	std::map<std::string, std::shared_ptr<Session>> &m_activeSessions; // Reference to all server's sessions
};