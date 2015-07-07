/*
 * server.hpp
 *
 *  Created on: Jul 4, 2015
 *      Author: root
 */

#ifndef SERVERCLIENT_HPP_
#define SERVERCLIENT_HPP_

#include <zmq.hpp>
#include <string>
#include <iostream>
#include <unistd.h>

#include "protobuf/stockmarket.pb.h"


#define sleep(n) Sleep(n)

namespace gts {

class ServerClient {
public:
	explicit ServerClient(const std::string& endpoint);
	explicit ServerClient(const std::string& frontend,
						  const std::string& backend);
	virtual ~ServerClient();

	void runProducerConsumer();
	void runRPC();
	void run_worker();
	void run_broker();

	static bool s_send(zmq::socket_t& socket, const std::string& msg)
	{
		zmq::message_t message(msg.size());
		memcpy(message.data(), msg.data(), msg.size());

		bool sent = socket.send(message);
		return sent;
	}

	static bool s_sendmore(zmq::socket_t &socket, const std::string &msg)
	{
		zmq::message_t message(msg.size());
		memcpy(message.data(), msg.data(), msg.size());

		bool sent = socket.send(message, ZMQ_SNDMORE);
		return sent;
	}

	static std::string s_recv(zmq::socket_t &socket)
	{
		zmq::message_t message;
		socket.recv(&message);

		return std::string(static_cast<char*>(message.data()), message.size());
	}
private:
	std::string _endpoint;
	std::string _frontend;
	std::string _backend;

	void handler_rpc(const std::string& service,
			const std::string& method,
			const std::string& serialized_request,
			std::string* serialized_response);


};


} // gts
#endif /* SERVERCLIENT_HPP_ */
