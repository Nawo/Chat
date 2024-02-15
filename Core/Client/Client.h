#pragma once

#define ASIO_STANDALONE

#include "../Interface/IClient.h"

#include <asio.hpp>
#include <asio/ts/buffer.hpp>
#include <asio/ts/internet.hpp>

class Client : public IClient<std::string>
{
public:
	Client() : m_socket(m_context)
	{
	}
	~Client()
	{
		Disconnect();
	}

	const bool Connect(const std::string &host, const std::string &port) override
	{
		try
		{
			asio::ip::tcp::resolver resolver(m_context);
			asio::connect(m_socket, resolver.resolve(host, port));

			if(!IsConnected())
			{
				return false;
			}

			Run();
			Read();
		}
		catch(std::exception &e)
		{
			std::cerr << "[ERROR] " << e.what() << std::endl;

			return false;
		}

		return true;
	}

	const bool Disconnect() override
	{
		if(IsConnected())
		{
			asio::post(m_context, [this]() { m_socket.close(); });
		}

		m_context.stop();

		if(m_thread.joinable())
		{
			m_thread.join();
		}

		return true;
	}

	const bool IsConnected() const override
	{
		return m_socket.is_open();
	}

protected:
	const bool Run() override
	{
		try
		{
			m_thread = std::thread([this]() { m_context.run(); });
		}
		catch(std::exception &e)
		{
			std::cerr << "[ERROR] " << e.what() << std::endl;

			return false;
		}

		return true;
	}

	const bool Send(const std::string &msg) override
	{
		asio::write(m_socket, asio::buffer(msg + "\n"));
		return true;
	}

	void Read() override
	{
		asio::async_read_until(m_socket, asio::dynamic_buffer(m_data), "\n",
							   [this](std::error_code errorCode, std::size_t length)
							   {
								   if(!errorCode)
								   {
									   m_incomingMessages.push_back(m_data);
									   m_data.erase(0, length);
									   Read();
								   }
								   else
								   {
									   std::cerr << "[ERROR] " << errorCode.message() << std::endl;
								   }
							   });
	}

	tsqueue<std::string> &GetIncomingMessages() override
	{
		return m_incomingMessages;
	}

private:
	tsqueue<std::string> m_incomingMessages;
	std::thread m_thread;
	asio::io_context m_context;
	asio::ip::tcp::socket m_socket;
	std::string m_data;
};