#pragma once

#include "ServerLib.h"

#include <functional>
#include <memory>
class ServerFactory
{
public:
	using CallServerFactory = std::function<std::shared_ptr<IServerLib>(int16_t port)>;

	static CallServerFactory invokeServerFactory();

private:
	std::shared_ptr<IServerLib> createServer(uint16_t port);
};
