#pragma once

template <class DataType>
class IClient
{
public:
	/**
	 * @returns true, if successfull run, false if not
	 *
	 * @note This function have to be called to run the client thread
	 */
	virtual const bool Run() = 0;

	/**
	 * @returns true, if successfull connect, false if not
	 *
	 * @note This function have to be called to connect with server,
	 * as host we give IP (e.g. 123.456.789.1), and port which server listening
	 */
	virtual const bool Connect(const std::string &host, const std::string &port) = 0;

	/**
	 * @returns true, if successfull disconnect, false if not
	 *
	 * @note This function have to be called to disconnect from the server
	 */
	virtual const bool Disconnect() = 0;

	/**
	 * @returns true, if client is connected, false if not
	 *
	 * @note This function is used to check client connection
	 */
	virtual const bool IsConnected() const = 0;

	/**
	 * @returns true, if successfull send message, false if not
	 *
	 * @note This function is called to send coded message,
	 * messsage is coded by this schemat:
	 *    int     string    string   string
	 * <msgType>|<sender>|<receiver>|<body>
	 * types of messages:
	 * 1 - Establish
	 * 2 - Relinquish
	 * 3 - Message
	 * 4 - Error
	 */
	virtual const bool Send(const DataType &msg) = 0;

	/**
	 * @returns std::string with coded message
	 *
	 * @note This function returns coded message
	 * messsage is coded by this schemat:
	 *    int     string    string   string
	 * <msgType>|<sender>|<receiver>|<body>
	 * types of messages:
	 * 1 - Establish
	 * 2 - Relinquish
	 * 3 - Message
	 * 4 - Error
	 */
	virtual const DataType Read() = 0;
};
