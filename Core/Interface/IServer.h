#pragma once

class IServer
{
public:
	virtual bool start() = 0;
	virtual void stop() = 0;

private:
	virtual void acceptConnection() = 0;
};