#include "Session.hpp"

#include "../Handlers/ResponseDecoder.cpp"

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

					auto decode = ResponseDecoder::makeCollable();
					auto messageContext = decode(line);

					if(messageContext->messageType_ == MessageType::Establish)
					{
						std::cout << "successfully established session with "
											 + messageContext->senderName_
								  << std::endl;
						username_ = messageContext->senderName_;
						m_activeSessions[username_] = self;
					}
					else if(messageContext->messageType_
							== MessageType::Relinquish)
					{
						std::cout << "successfully closed session with "
											 + messageContext->senderName_
								  << std::endl;
						m_activeSessions.erase(username_);
					}
					else if(messageContext->messageType_
							== MessageType::Message)
					{
						std::cout << "Current active sessions: ";
						for(const auto &pair : m_activeSessions)
						{
							std::cout << pair.first << " ";
						}
						std::cout << std::endl;

						auto recipientSession = m_activeSessions.find(
								messageContext->receiverName_);
						if(recipientSession != m_activeSessions.end())
						{
							std::cout << "got message from "
												 + messageContext->senderName_
												 + ", forwarding message to "
												 + messageContext->receiverName_
									  << std::endl;
							std::string forwardMessage =
									"Message|" + messageContext->senderName_
									+ "|" + messageContext->receiverName_ + "|"
									+ messageContext->body_;
							recipientSession->second->request(forwardMessage);
						}
						else
							std::cout << "got message from "
												 + messageContext->senderName_
												 + ", but recipient: "
												 + messageContext->receiverName_
												 + " is offline"
									  << std::endl;
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
	std::cout << "About to send to " << username_ << ": " << request
			  << std::endl;

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
				else
				{
					std::cout << "Sent " << length << " bytes to " << username_
							  << ": " << request << std::endl;
				}
			});
}
