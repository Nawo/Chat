#pragma once

#include "IClientLib.h"

#include <functional>
#include <memory>
class ClientFactory
{
public:
	using CallClientFactory = std::function<std::shared_ptr<IClientLib>()>;

	static CallClientFactory invokeClientFactory();

private:
	std::shared_ptr<IClientLib> createClient();
};
