#include "sampleDataGetter.h"

void grabData(asio::ip::tcp::socket &socket)
{
	socket.async_read_some(asio::buffer(buffer.data(), buffer.size()),
						   [&](std::error_code ec, size_t lenght)
						   {
							   if(!ec)
							   {
								   std::cout << "Read: " << lenght << " bytes;"
											 << std::endl;

								   for(int i = 0; i < lenght; i++)
								   {
									   std::cout << buffer[i];
								   }

								   grabData(socket);
							   }
						   });
}

void infoGetter()
{
	asio::error_code ec;

	asio::io_context context;

	asio::io_context::work fakeWork(context);

	std::thread threadContext([&]() { context.run(); });

	asio::ip::tcp::endpoint endpoint(asio::ip::make_address("51.38.81.49", ec),
									 80);

	asio::ip::tcp::socket socket(context);

	socket.connect(endpoint, ec);

	if(ec)
	{
		std::cout << "problem" << ec.message() << std::endl;
	}
	if(socket.is_open())
	{
		grabData(socket);

		std::string sRequest = "GET /index.html HTTP/1.1\r\n"
							   "Host: david-barr.co.uk\r\n"
							   "Connection: close\r\n\r\n";

		socket.write_some(asio::buffer(sRequest.data(), sRequest.size()), ec);

		using namespace std::chrono_literals;
		std::this_thread::sleep_for(20000ms);
		context.stop();

		if(threadContext.joinable())
		{
			threadContext.join();
		}
	}
}