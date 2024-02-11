#include "ClientFactory.hpp"

#include "ClientLib.h"
#include "ResponseCoder.h"
#include "ResponseDecoder.h"

ClientFactory::CallClientFactory ClientFactory::invokeClientFactory()
{
	return [&]()
	{
		auto clientFactory = ClientFactory();
		return clientFactory.createClient();
	};
}

std::shared_ptr<IClientLib> ClientFactory::createClient()
{
	auto coder = ResponseCoder::makeCollable();
	auto decoder = ResponseDecoder::makeCollable();
	auto client = std::make_shared<Client>();
	return std::make_shared<ClientLib>(client, decoder, coder);
}