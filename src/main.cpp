//============================================================================
// Name        : gts.cpp
// Author      : monika mueller
// Version     :
// Copyright   : Your copyright notice
// Description : Hello World in C++, Ansi-style
//============================================================================
//#include "log/logger.hpp"

#include "log/logger.hpp"
#include "serverclient.hpp"
#include "msg/msg_uuid.hpp"

#include <google/protobuf/descriptor.h>
#include <google/protobuf/dynamic_message.h>
#include <google/protobuf/io/coded_stream.h>
#include <google/protobuf/io/zero_copy_stream_impl.h>
#include <google/protobuf/stubs/common.h>

#include <boost/bind.hpp>
#include <boost/thread.hpp>
#include <boost/shared_ptr.hpp>
#include <boost/scoped_ptr.hpp>

#include <assert.h>
#include <unistd.h>
#include <sys/wait.h>
#include <signal.h>
#include <pthread.h>
#include <iostream>

using namespace std;
using namespace google::protobuf::io;


int main() {
	cout << "general trading system" << endl; // prints general trading system

	START_LOGGING
	using namespace gts;
	try {
		LOG_INFO << "********* Main entry *********";
		const char* test_UUID = GTS_MSG_UUID_C ;
		LOG_INFO << test_UUID;

		GOOGLE_PROTOBUF_VERIFY_VERSION;

		// start the server in thread, as member function, not static !
		const std::string endpoint = "tcp://127.0.0.1:8989";
		ServerClient server(endpoint);
		boost::scoped_ptr<boost::thread> t1(new boost::thread(boost::bind(
					&ServerClient::runProducerConsumer, &server)));
		//boost::thread* t2 = new boost::thread(boost::bind(&ServerClient::runProducerConsumer, &sc));
		zmq::context_t context(1);
		zmq::socket_t socket(context, ZMQ_REQ);
		socket.connect(endpoint.c_str());
		for (int irequest=0; irequest != 2; irequest++) {
			// set up the request protobuf
			StockMarket::StockQuoteRequest pb_request;
			pb_request.set_symbol("GOO");
			pb_request.set_start(1111111);
			pb_request.set_end(222222);
			// serialize the request to a string
			std::string serialized;
			pb_request.SerializeToString(&serialized);
			// create and send the message
			zmq::message_t request(serialized.size());
			memcpy((void*)request.data(), serialized.c_str(),
					serialized.size());
			LOG_INFO << "Client: Sending request " << request << " .";
			socket.send(request);
			// create and receive the reply
			zmq::message_t reply;
			socket.recv(&reply);
			// get the response object and parse it
			StockMarket::StockQuoteResponse pb_response;
			pb_response.ParseFromArray(reply.data(), reply.size());

			//todo assert on the same response, request ID!
		}

		t1->join();

		return EXIT_SUCCESS;
	}
	//TODO own exception handling scalable
	catch (std::exception& e) {
		std::cout << "FAILURE: " << e.what() << std::endl;
		return EXIT_FAILURE;
	}
}
