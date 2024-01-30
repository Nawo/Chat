#pragma once

class IServer
{
public:
	virtual void start() = 0;
	virtual void stop() = 0;

private:
	virtual void acceptConnection() = 0;
};

using ServerPtr = std::shared_ptr<IServer>;