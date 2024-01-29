#include "Session.hpp"

Session::Session(asio::ip::tcp::socket socket,
				 std::map<std::string, std::shared_ptr<Session>> &sessions)
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
					if(decodedMessage->getMessageType()
					   == MessageType::Establish)
					{
						m_activeSessions[username_] = self;
						std::cout << "=====[Log]=====\n";
						std::cout << username_ + " join the chat \n";
						std::cout << "IP: "
											 + m_socket.remote_endpoint()
													   .address()
													   .to_string()
											 + "\n";
						std::cout << "===============\n";
					}
					else if(decodedMessage->getMessageType()
							== MessageType::Relinquish)
					{
						std::cout << "=====[Log]=====\n";
						std::cout << username_ + " left the chat."
											 + decodedMessage->getSenderName();
						std::cout << "===============\n";

						m_activeSessions.erase(username_);
					}
					else if(decodedMessage->getMessageType()
							== MessageType::Message)
					{
						auto recipientSession = m_activeSessions.find(
								decodedMessage->getReceiverName());
						if(recipientSession != m_activeSessions.end())
						{
							std::cout << "=====[Log]=====\n";
							std::cout << username_ + " sent message to "
												 + decodedMessage
														   ->getReceiverName()
												 + "\n";
							std::cout << "Bytes: " + std::to_string(length)
												 + "\n";
							std::cout << "Message: " + decodedMessage->getBody()
												 + "\n";
							std::cout << "===============\n";

							std::string forwardMessage =
									std::to_string(static_cast<int>(
											decodedMessage->getMessageType()))
									+ "|" + decodedMessage->getSenderName()
									+ "|" + decodedMessage->getReceiverName()
									+ "|" + decodedMessage->getBody();
							recipientSession->second->request(forwardMessage);
						}
						else
						{
							std::cout << "=====[Log]=====\n";
							std::cout << username_ + " sent message to "
												 + decodedMessage
														   ->getReceiverName()
												 + " [OFFLINE]" + "\n";
							std::cout << "Bytes: " + std::to_string(length)
												 + "\n";
							std::cout << "Message: " + decodedMessage->getBody()
												 + "\n";
							std::cout << "===============\n";
						}
					}
					else
						std::cout << "received unknown protocol message, start "
									 "retrying... "
								  << std::endl;
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
	asio::async_write(
			m_socket, asio::buffer(request + "\n"),
			[this, self, request](std::error_code errorCode, std::size_t length)
			{
				if(errorCode)
				{
					std::cerr << "Error sending to " << username_ << ": "
							  << errorCode.message() << std::endl;
				}
			});
}
