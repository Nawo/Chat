#pragma once

#define ASIO_STANDALONE

#include "../Common/TSqueue.h"
#include "../Interface/ISession.h"

#include <asio.hpp>
#include <asio/ts/buffer.hpp>
#include <asio/ts/internet.hpp>

class Session : public ISession<std::string>, public std::enable_shared_from_this<Session>
{
public:
	explicit Session(asio::ip::tcp::socket socket,
					 tsqueue<std::pair<std::string, std::shared_ptr<Session>>> &incomingMessages)
		: std::enable_shared_from_this<Session>(), m_socket(std::move(socket)), m_incomingMessages(incomingMessages)
	{
	}

	void Start() override
	{
		Read();
	}

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

									   m_incomingMessages.push_back(std::make_pair(msg, self));

									   Read();
								   }
								   else
								   {
									   std::cerr << "[ERROR] " + errorCode.message() << std::endl;
								   }
							   });
	}

	void setSessionOwner(const std::string &owner) override
	{
		m_sessionOwner = owner;
	}

	const std::string getSessionOwner() const override
	{
		return m_sessionOwner;
	}

	const std::string getSessionIP() const override
	{
		return m_socket.remote_endpoint().address().to_string();
	}

	const bool IsConnected() const override
	{
		return m_socket.is_open();
	}

private:
	std::string m_data;
	asio::ip::tcp::socket m_socket;
	std::string m_sessionOwner;
	tsqueue<std::pair<std::string, std::shared_ptr<Session>>> &m_incomingMessages;
};