#pragma once

#include "Poco/Net/SocketReactor.h"
#include "Poco/Net/SocketAcceptor.h"
#include "Poco/Net/SocketNotification.h"
#include "Poco/Net/StreamSocket.h"
#include "Poco/Net/ServerSocket.h"
#include "Poco/NObserver.h"
#include "Poco/Exception.h"
#include "Poco/Thread.h"
#include "Poco/Util/ServerApplication.h"
#include "Poco/Util/Option.h"
#include "Poco/Util/OptionSet.h"
#include "Poco/Util/HelpFormatter.h"
#include "Poco/AutoPtr.h"

#include "Poco/StringTokenizer.h"
#include "Poco/String.h"
#include <iostream>

#include "ofxMissing.h"
#include "ofxAMFSerializer.h"
#include "ofxAMFPacket.h"
#include "ofxAMFHTTPResponse.h"
#include "ofxAMFHTTPRequest.h"

using Poco::Net::SocketReactor;
using Poco::Net::SocketAcceptor;
using Poco::Net::ReadableNotification;
using Poco::Net::ShutdownNotification;
using Poco::Net::ServerSocket;
using Poco::Net::StreamSocket;
using Poco::NObserver;
using Poco::AutoPtr;
using Poco::Thread;
using Poco::StringTokenizer;
using Poco::toLowerInPlace;

using namespace std;
class ofxAMFServer;

// @todo move this stuff to a generic include or use std::numeric limits (?)
#ifndef UINT32_MAX
#define UINT32_MAX  (0xffffffff)
#endif

class ofxAMFConnection {
public:
	static int count; // tmp
	ofxAMFConnection(StreamSocket rSocket, SocketReactor& rReactor);
	~ofxAMFConnection();
	void setup(ofxAMFServer* amfServer);
	int write(IOBuffer& amfBuffer);
	void onReadable(const AutoPtr<ReadableNotification>& pNotif);
	void onShutdown(const AutoPtr<ShutdownNotification>& pNotif);

private:
	void deserializeRequest();

	// amf
	ofxAMFServer* amf_server;
	ofxAMFHTTPRequest amf_http_request;
	ofxAMFSerializer amf3;

	// network
	StreamSocket socket;
	SocketReactor& reactor;

	// buffers
	IOBuffer buffer;
	IOBuffer amf_request_buffer;
	uint32_t start_body;
	uint32_t content_length;
	uint32_t bytes_waiting;

};
