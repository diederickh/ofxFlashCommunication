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
#include "IOBuffer.h"

#include "Dictionary.h"
#include "ofxAMFSerializer.h"
#include "ofxAMFPacket.h"
#include "ofxAMFHTTPResponse.h"

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


class ofxAMFConnection {
public:
	enum ofxAMFConnectionStates {
		 RETRIEVE_HEADER
		,RETRIEVE_CONTENT
	};
	
	ofxAMFConnection(StreamSocket rSocket, SocketReactor& rReactor);
	~ofxAMFConnection();
	void setup(ofxAMFServer* amfServer);
	int write(IOBuffer& buffer);
	void onReadable(const AutoPtr<ReadableNotification>& pNotif);
	void onShutdown(const AutoPtr<ShutdownNotification>& pNotif);
	bool parseHTTPHeaders(string& headers, Dictionary& result);
private:
	inline bool parseContentBufferWhenComplete();
	void deserializeRequest();
	ofxAMFServer* amf_server;
	StreamSocket socket;
	SocketReactor& reactor;
	IOBuffer buffer;
	ofxAMFSerializer amf3;
	int state;
	uint32_t content_length;
	uint32_t bytes_waiting; 
	//uint32_t num_content_bytes_received;

};