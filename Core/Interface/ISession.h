#pragma once

template <class DataType>
class ISession
{
public:
	virtual void Start() = 0;

private:
	virtual void Send(const DataType &msg) = 0;

	virtual void Read() = 0;
};