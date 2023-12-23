#include <asio.hpp>
#include <iostream>

class Chat
{
public:
    bool setSenderIP ()
    {
        return true;
    }
    bool setReceiverIP ()
    {
        return true;
    }

private:
    char m_receiverIP;
    char m_senderIP;
    const char m_port = 25555;
};

int main ()
{
    return 0;
    std::cout << "work!";
}