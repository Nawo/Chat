#pragma once
#include <string>

struct Message
{
	void clear()
	{
		sender.clear();
		reciever.clear();
		message.clear();
	}

	uint8_t ID;
	std::string sender;
	std::string reciever = "null";
	std::string message;
	std::string type;
};