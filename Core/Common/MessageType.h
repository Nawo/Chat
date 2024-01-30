#pragma once

enum class MessageType : uint8_t
{
	Establish = 1,
	Relinquish = 2,
	Message = 3,
	Error = 4
};

namespace PredefinedMessages
{
	static std::string successClientLogin = "Succesfull loged to the server.";
	static std::string successClientDisconnect = "Succesfull disconnect from the server.";
} // namespace PredefinedMessages