#pragma once

class ISession
{
public:
	virtual void Start() = 0;

private:
	virtual void Send(const std::string &msg) = 0;

	virtual void Read() = 0;
};