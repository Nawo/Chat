#include "Session.h"

Session::Session(asio::ip::tcp::socket socket,
				 std::map<std::string, std::shared_ptr<Session>> &sessions)
	: m_socket(std::move(socket)), m_activeSessions(sessions)
{
}

void Session::start()
{
	receive();
}

void Session::receive()
{
	auto self(shared_from_this());
	asio::async_read(m_socket, asio::buffer(&m_data, sizeof(m_data)),
					 [this, self](std::error_code errorCode, std::size_t length)
					 {
						 if(!errorCode)
						 {
							 if(m_data.type == "login")
							 {
								 m_username = m_data.sender;
								 m_activeSessions[m_username] = self;
								 std::cout
										 << "Succesfull loget user "
													+ m_username + ", "
													+ m_socket.remote_endpoint()
															  .address()
															  .to_string();
							 }
							 if(m_data.type == "logout")
							 {
								 // erase from map
							 }
							 if(m_data.reciever == "ALL")
							 {
								 for(auto &a : m_activeSessions)
									 a.second->send(m_data);
							 }

							 m_data.clear();
							 receive();
						 }
					 });
}

void Session::send(const Message &data)
{
	asio::async_write(
			m_socket, asio::buffer(&data, sizeof(data)),
			[this, data](std::error_code errorCode, std::size_t length)
			{
				if(errorCode)
				{
					std::cerr << "Error sending to " << data.reciever << ": "
							  << errorCode.message() << std::endl;
				}
				else
				{
					std::cout << "Sent " << length << " bytes to "
							  << data.reciever << ": " << data.message
							  << std::endl;
				}
			});
}