#include "net_chat.h"

enum class CustomMsgTypes : uint32_t
{
	Message
};

class CustomClient : public chat::net::client_interface<CustomMsgTypes>
{
public:
	bool SendMessage(std::string message)
	{
		chat::net::message<CustomMsgTypes> msg;
		msg.header.id = CustomMsgTypes::Message;
		msg << message;
		// m_connection->Send(msg);
		return true;
	}
};

int main()
{
	// CustomClient c;
	// c.Connect("sampleServer", 60000);
	// c.SendMessage("hej");
	// return 0;
}