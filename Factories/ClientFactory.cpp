#include "ClientFactory.hpp"

#include "ClientLib.h"
#include "ResponseCoder.h"
#include "ResponseDecoder.h"

ClientFactory::CallClientFactory ClientFactory::invokeClientFactory(std::function<void(std::string)> callback = nullptr)
{
	return [&]()
	{
		auto clientFactory = ClientFactory();
		return clientFactory.createClient(callback);
	};
}

std::shared_ptr<IClientLib> ClientFactory::createClient(std::function<void(std::string)> callback)
{
	auto coder = ResponseCoder::makeCollable();
	auto decoder = ResponseDecoder::makeCollable();
	auto client = std::make_shared<Client>(callback);
	return std::make_shared<ClientLib>(client, decoder, coder);
}