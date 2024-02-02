#pragma once

class ISession
{
public:
	virtual void start() = 0;

private:
	virtual void request(const std::string &request) = 0;
	virtual void response() = 0;
};