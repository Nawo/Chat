#include "ServerFactory.hpp"

#include "ResponseCoder.h"
#include "ResponseDecoder.h"
#include "ServerLib.h"

ServerFactory::CallServerFactory ServerFactory::invokeServerFactory()
{
	return [&](uint16_t port)
	{
		auto serverFactory = ServerFactory();
		return serverFactory.createServer(port);
	};
}

std::shared_ptr<IServerLib> ServerFactory::createServer(uint16_t port)
{
	auto coder = ResponseCoder::makeCollable();
	auto decoder = ResponseDecoder::makeCollable();
	auto server = std::make_shared<Server>(port);
	return std::make_shared<ServerLib>(server, decoder, coder);
}