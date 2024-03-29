#pragma once

class IServer
{
public:
	virtual const bool Start() = 0;

	virtual void Stop() = 0;

private:
	virtual void AcceptConnections() = 0;
};