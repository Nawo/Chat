#pragma once

template <class DataType>
class ISession
{
public:
	virtual void Start() = 0;

	virtual const bool IsConnected() const = 0;

	virtual void setSessionOwner(const std::string &) = 0;

	virtual const std::string getSessionOwner() const = 0;

	virtual const std::string getSessionIP() const = 0;

	virtual void Send(const DataType &) = 0;

	virtual void Read() = 0;
};