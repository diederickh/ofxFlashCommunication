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
#include <iostream>

#include "IOBuffer.h"

using Poco::Net::SocketReactor;
using Poco::Net::SocketAcceptor;
using Poco::Net::ReadableNotification;
using Poco::Net::ShutdownNotification;
using Poco::Net::ServerSocket;
using Poco::Net::StreamSocket;
using Poco::NObserver;
using Poco::AutoPtr;
using Poco::Thread;

#include "Dictionary.h"
#include "ofxAMFSerializer.h"

using namespace std;
class ofxAMFServer;
class ofxAMFConnection {
public:
	ofxAMFConnection(StreamSocket rSocket, SocketReactor& rReactor);
	~ofxAMFConnection();
	void setup(ofxAMFServer* amfServer);
	int write(string sData);
	void onReadable(const AutoPtr<ReadableNotification>& pNotif);
	void onShutdown(const AutoPtr<ShutdownNotification>& pNotif);
private:
	void parseBuffer();
	ofxAMFServer* amf_server;
	StreamSocket socket;
	SocketReactor& reactor;
	IOBuffer buffer;
	ofxAMFSerializer amf3;
};