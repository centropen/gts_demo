/*
 * serverclient.cpp
 *
 *  Created on: Jul 5, 2015
 *      Author: root
 */
#include "serverclient.hpp"

#include <google/protobuf/descriptor.h>
#include <google/protobuf/dynamic_message.h>
#include <google/protobuf/io/coded_stream.h>
#include <google/protobuf/io/zero_copy_stream_impl.h>
#include <google/protobuf/stubs/common.h>

#include <string>
#include <algorithm>
#include <iostream>

namespace gts {
using namespace google::protobuf::io;

ServerClient::ServerClient(const std::string& endpoint)
	: _endpoint(endpoint) {}

ServerClient::ServerClient(const std::string &frontend, const std::string &backend)
	: _frontend(frontend), _backend(backend) {}

ServerClient::~ServerClient() {}

/**
 * Simple Producer -> Consumer case
 * One known request and known response, only for one message over the same
 * request/reply socket.
 */
void ServerClient::runProducerConsumer()
{
	zmq::context_t context(1);
	zmq::socket_t socket(context, ZMQ_REP);
	socket.bind(_endpoint.c_str());

	while(true) {
		zmq::message_t request;
		StockMarket::StockQuoteRequest pb_request;

		//get the request and parse the protocol buffer
		socket.recv(&request);
		pb_request.ParseFromArray(request.data(), request.size());

		//todo logger
		std::cout << "<Server> Request Received " << pb_request.symbol()
				<< ": " << "Start: "<< pb_request.start()
				<< ": " << "End: "<< pb_request.end();
		//todo some logic comes here

		//create the response
		StockMarket::StockQuoteResponse pb_response;
		const int StockQuoteSize = 2;
		for (int i=0;i<StockQuoteSize; ++i) {
			StockMarket::StockQuote* quote = pb_response.add_result();
			quote->set_uuid("d866a824-a880-4953-8fed-6ad27d5cb69d");
			quote->set_symbol("GOOG");
			quote->set_timestamp(20150321032100);
			quote->set_open(554.89);
			quote->set_high(554.80);
			quote->set_low(558.42);
			quote->set_close(550.10);
			quote->set_volume(2435);
			quote->set_adjustedclose(177.57);
		}
		std::string serialized;
		pb_response.SerializeToString(&serialized);
		//create the reply
		zmq::message_t reply(serialized.size());
		memcpy((void*)reply.data(), serialized.c_str(), serialized.size());
		socket.send(reply);
	}
}

/**
 * runRPC (Remote Procedure Call implementation) is on server side,
 * whereas a protobuf is wrapped, i.e. the messages can be wrapped
 * dynamically so long the server and client do know type of messages
 * interchanged over the wire
 */
void ServerClient::runRPC()
{
	zmq::context_t context(1);
	zmq::socket_t socket(context, ZMQ_REP);

	socket.bind(_endpoint.c_str());
	while (true) {
	//[0] receive and parse the RPC wrapper message
		zmq::message_t zmq_request;
		StockMarket::StockRPCRequest rpc_request;
		socket.recv(&zmq_request);
		rpc_request.ParseFromArray(zmq_request.data(),
				zmq_request.size());
	//[1] hand over the content to the handler, the string will
	// be serialized
		std::string serialized_response;
		handler_rpc(rpc_request.service(), rpc_request.method(),
				rpc_request.protobuf(), &serialized_response);

	//[2] set up the response wrapper
		StockMarket::StockRPCResponse rpc_response;
		rpc_response.set_protobuf(serialized_response);
	//[3] serialize the wrapper
		std::string rpc_response_serialized;
		rpc_response.SerializeToString(&rpc_response_serialized);

	//[4] send the response back to the client
		zmq::message_t reply(rpc_response_serialized.size());
		memcpy((void*)reply.data(), rpc_response_serialized.c_str(),
				rpc_response_serialized.size());
		socket.send(reply);
	}
}

/**
 * handler_rpc dispatches registered callbacks to handle different
 * methods, for demonstration purposes two functions handled
 * @param service
 * @param method
 * @param serialized_request
 * @param serialized_response
 */
void ServerClient::handler_rpc(const std::string& service,
		const std::string& method,
		const std::string& serialized_request,
		std::string* serialized_response)
{
	if (method.compare("addition") == 0) {
		// for demo purposes no reflection api used, we know all the methods in advance
		google::protobuf::Message *request = new StockMarket::StockRPCAddRequest;
		const google::protobuf::Descriptor* descriptor = request->GetDescriptor();
		// find the fields
		const google::protobuf::FieldDescriptor* func1_field =
				descriptor->FindFieldByName("func1");
		const google::protobuf::FieldDescriptor* func2_field =
				descriptor->FindFieldByName("func2");
		// parse the fields
		request->ParseFromString(serialized_request);
		const google::protobuf::Reflection* reflection = request->GetReflection();

		// call the actual RPC method
		uint32_t sum = reflection->GetUInt32(*request, func1_field) +
				reflection->GetUInt32(*request, func2_field);

		// set up the response
		StockMarket::StockRPCAddResponse response;
		response.set_sum(sum);
		response.SerializeToString(serialized_response);
	} else if (method.compare("reverse") ==0 ) {
		StockMarket::StockRPCReverseRequest request;
		request.ParseFromString(serialized_request);
		std::string to_reverse = request.to_reverse();
		reverse(to_reverse.begin(), to_reverse.end());
		StockMarket::StockRPCReverseResponse response;
		response.set_reversed(to_reverse);
		response.SerializeToString(serialized_response);
	}
}

/**
 * runWorker - queue worker
 */
void ServerClient::run_worker(){
	zmq::context_t context(1);
	zmq::socket_t responder(context, ZMQ_REP);
	responder.connect(_backend.c_str());
	zmq::message_t message;
	int64_t more;
	while (true) {
		StockMarket::StockWorkerRequest worker_request;
		while (true) {
			responder.recv(&message);
			size_t more_size = sizeof(more);
			responder.getsockopt(ZMQ_RCVMORE, &more, &more_size);

			if (!more) {
				worker_request.ParseFromArray(message.data(), message.size());
				break;
			}
		}
		std::string to_reverse = worker_request.string_in();
		reverse(to_reverse.begin(), to_reverse.end());
		//todo logger
		std::cout << "IN " << worker_request.string_in() << std::endl;
		std::cout << "OUT " << to_reverse << std::endl;
		s_send(responder, "STOCK");
	}
}

/**
 * broker
 */
void ServerClient::run_broker() {
	zmq::context_t context(1);
	zmq::socket_t frontend(context, ZMQ_ROUTER);
	zmq::socket_t backend(context, ZMQ_DEALER);

	frontend.bind(_frontend.c_str());

}

} //gts


