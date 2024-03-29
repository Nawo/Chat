#pragma once

enum class MessageType : uint8_t
{
	Establish = 1,
	Relinquish = 2,
	Message = 3,
	MessageAll = 4,
	Error = 5,
	PrintUsers = 6
};

namespace PredefinedMessages
{
	static std::string successClientLogin = "Succesfull loged to the server.";
	static std::string successClientDisconnect = "Succesfull disconnect from the server.";
	static std::string serverStarted = "Server started.";
} // namespace PredefinedMessages