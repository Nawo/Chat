#include "Session.hpp"

Session::Session(asio::ip::tcp::socket socket, std::map<std::string, std::shared_ptr<Session>> &sessions)
	: m_socket(std::move(socket)), m_activeSessions(sessions)
{
}

void Session::start()
{
	response();
}

void Session::response()
{
	auto self(shared_from_this());
	asio::async_read_until(
			m_socket, asio::dynamic_buffer(data_), '\n',
			[this, self](std::error_code errorCode, std::size_t length)
			{
				if(!errorCode)
				{
					std::string line(data_.substr(0, length - 1));
					data_.erase(0, length);

					auto decodedMessage = ResponseDecoder::makeCollable()(line);

					username_ = decodedMessage->getSenderName();
					const std::string &receiverName = decodedMessage->getReceiverName();
					const std::string &body = decodedMessage->getBody();
					const MessageType &msgType = decodedMessage->getMessageType();

					if(msgType == MessageType::Establish)
					{
						m_activeSessions[username_] = self;

						std::cout << "=====[Log]=====\n";
						std::cout << username_ + " join the chat \n";
						std::cout << "IP: " + m_socket.remote_endpoint().address().to_string() + "\n";
						std::cout << "===============\n";
					}
					else if(msgType == MessageType::Relinquish)
					{
						m_activeSessions.erase(username_);

						std::cout << "=====[Log]=====\n";
						std::cout << username_ + " left the chat." + "\n";
						std::cout << "===============\n";
					}
					else if(msgType == MessageType::Message)
					{
						if(receiverName == "all")
						{
							for(const auto &[clientName, clientSocket] : m_activeSessions)
							{
								if(clientName != username_)
								{
									std::cout << "=====[Log]=====\n";
									std::cout << username_ + " sent message to all\n";
									std::cout << "Bytes: " + std::to_string(length) + "\n";
									std::cout << "Message: " + body + "\n";
									std::cout << "===============\n";

									std::string forwardMessage = std::to_string(static_cast<int>(msgType)) + "|"
																 + username_ + "|" + clientName + "|" + body;

									clientSocket->request(forwardMessage);
								}
							}
						}
						else
						{
							auto recipientSession = m_activeSessions.find(receiverName);
							if(recipientSession != m_activeSessions.end())
							{
								std::cout << "=====[Log]=====\n";
								std::cout << username_ + " sent message to " + receiverName + "\n";
								std::cout << "Bytes: " + std::to_string(length) + "\n";
								std::cout << "Message: " + body + "\n";
								std::cout << "===============\n";

								std::string forwarddMessage = std::to_string(static_cast<int>(msgType)) + "|"
															  + username_ + "|" + receiverName + "|" + body;

								recipientSession->second->request(forwarddMessage);
							}

							else // user not found in m_activeSessions
							{
								std::cout << "=====[Log]=====\n";
								std::cout << username_ + " tried to send message to " + receiverName
													 + " but it is [OFFLINE]" + "\n";
								std::cout << "Bytes: " + std::to_string(length) + "\n";
								std::cout << "Message: " + body + "\n";
								std::cout << "===============\n";
							}
						}
					}
					else
					{
						std::cout << "received unknown protocol message, start retrying... " << std::endl;
					}

					response();
				}
				else
				{
					std::cout << "connection lost" << std::endl;
					m_activeSessions.erase(username_);
				}
			});
}

void Session::request(const std::string &request)
{
	auto self(shared_from_this());
	asio::async_write(m_socket, asio::buffer(request + "\n"),
					  [this, self, request](std::error_code errorCode, std::size_t length)
					  {
						  if(errorCode)
						  {
							  std::cerr << "[ERROR]" << errorCode.message() << std::endl;
						  }
					  });
}
