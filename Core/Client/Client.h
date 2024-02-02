#pragma once

#include "../Common/Common.h"
#include "../Handlers/ResponseCoder.h"
#include "../Handlers/ResponseDecoder.h"
#include "../Interface/IClient.h"

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

	const bool Run() override
	{
		m_thread = std::thread([this]() { m_context.run(); });
		return true;
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
			std::cerr << "[ERROR] " << e.what() << "\n";
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

	const bool Send(const std::string &msg) override
	{
		asio::write(m_socket, asio::buffer(msg + "\n"));
		return true;
	}

	void Read() override
	{
		asio::async_read_until(m_socket, asio::dynamic_buffer(m_data), '\n',
							   [this](std::error_code errorCode, std::size_t length)
							   {
								   if(!errorCode)
								   {
									   // BE DELETED SOON
									   auto decode = ResponseDecoder::makeCollable()(m_data);
									   std::cout << "[" + decode->getSenderName() + "]: " + decode->getBody();
									   //

									   // m_incomingMessages.push_back(m_data);
									   m_data.erase(0, length);
									   Read();
								   }
								   else
								   {
									   std::cerr << "[ERROR] Read failed: " << errorCode.message() << std::endl;
								   }
							   });
	}

protected:
	// TO DO make thread safe members to handle push, pop etc operations on this container
	std::deque<std::string> m_incomingMessages;

private:
	std::thread m_thread;
	asio::io_context m_context;
	asio::ip::tcp::socket m_socket;
	std::string m_data;
};