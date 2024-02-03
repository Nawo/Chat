#include "Session.h"

Session::Session(asio::ip::tcp::socket socket, std::map<std::string, std::shared_ptr<Session>> &sessions)
	: m_socket(std::move(socket)), m_activeSessions(sessions)
{
}

void Session::start()
{
	response();
}

namespace
{
	// TODO
	// functions to handle message types, e.g. sendMessageToAll, sendMessageToUser, getUsersList
	void sendMessageToAll(const std::string &request)
	{
	}

	void sendClientSuccessfullLoginMessage(const std::string &request)
	{
	}

	void sendClientLogoutMessage(const std::string &request)
	{
	}
} // namespace

void Session::response() // read messages from client
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

					switch(msgType)
					{
					case MessageType::Establish:
					{
						m_activeSessions[username_] = self;

						std::cout << "=====[Log]=====\n";
						std::cout << username_ + " join the chat \n";
						std::cout << "IP: " + m_socket.remote_endpoint().address().to_string() + "\n";
						std::cout << "===============\n";

						request(ResponseCoder::makeCollable()(MessageType::Message, "SERVER", username_,
															  PredefinedMessages::successClientLogin));
						break;
					}

					case MessageType::Relinquish:
					{
						m_activeSessions.erase(username_);

						std::cout << "=====[Log]=====\n";
						std::cout << username_ + " left the chat." + "\n";
						std::cout << "===============\n";

						request(ResponseCoder::makeCollable()(MessageType::Message, "SERVER", username_,

															  PredefinedMessages::successClientDisconnect));
						break;
					}

					case MessageType::Message:
					{
						auto recipientSession = m_activeSessions.find(receiverName);
						if(recipientSession != m_activeSessions.end())
						{
							std::cout << "=====[Log]=====\n";
							std::cout << username_ + " sent message to " + receiverName + "\n";
							std::cout << "Bytes: " + std::to_string(length) + "\n";
							std::cout << "Message: " + body + "\n";
							std::cout << "===============\n";

							recipientSession->second->request(
									ResponseCoder::makeCollable()(MessageType::Message, username_, receiverName, body));
						}

						else // user not found in m_activeSessions
						{
							std::cout << "=====[Log]=====\n";
							std::cout << username_ + " tried to send message to " + receiverName
												 + " but it is [OFFLINE]" + "\n";
							std::cout << "Bytes: " + std::to_string(length) + "\n";
							std::cout << "Message: " + body + "\n";
							std::cout << "===============\n";

							std::string errorMsg = receiverName + " is OFFLINE";

							std::string codedMessage =
									ResponseCoder::makeCollable()(MessageType::Error, "SERVER", username_, errorMsg);

							request(codedMessage);
						}

						break;
					}

					case MessageType::MessageAll:
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

								clientSocket->request(
										ResponseCoder::makeCollable()(MessageType::Message, username_, "all", body));
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

					response();
				}
				else
				{
					std::cout << "connection lost" << std::endl;
					m_activeSessions.erase(username_);
				}
			});
}

void Session::request(const std::string &request) // Send request to client's socket
{
	auto self(shared_from_this());
	asio::async_write(m_socket, asio::buffer(request + "\n"),
					  [this, self, request](std::error_code errorCode, std::size_t length)
					  {
						  if(errorCode)
						  {
							  std::cerr << "[ERROR] " << errorCode.message() << std::endl;
						  }
					  });
}
