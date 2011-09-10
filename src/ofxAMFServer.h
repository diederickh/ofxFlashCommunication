#ifndef OFXAMFSERVERH
#define OFXAMFSERVERH

/*
- we need a better documentation system which does not clutter the code ;-)

ofxAMFServer.setup(string host, int port)
==========================================
Setup a new ofxAMFServer listening on host ip address and the given port. 
You can pass * for the value of host which will create a server bound on all
local IP addresses.

@param	string	host		The IP on which we start listening. Pass * to 
							listen on all IP addresses
@param	int		port		The port on which the server will be started.

*/

#include "Poco/Thread.h"
#include "Poco/Mutex.h"
#include "Poco/Runnable.h"
#include "Poco/Net/ServerSocket.h"
#include "Poco/Net/StreamSocket.h"
#include "Poco/Net/SocketAddress.h"
#include "Poco/Net/SocketReactor.h"
#include "Poco/Net/SocketAcceptor.h"
#include "Poco/NObserver.h"
#include "Poco/AutoPtr.h"

#include <vector>
#include <string>

#include "ofxAMFEvent.h"
#include "ofxFlashPolicy.h"
#include "ofxAMFConnection.h"
#include "ofMain.h" // for debugging

using Poco::Net::ReadableNotification;
using Poco::NObserver;
using Poco::Thread;
using Poco::Mutex;
using Poco::Runnable;
using Poco::Net::ServerSocket;
using Poco::Net::StreamSocket;
using Poco::Net::SocketAddress;
using Poco::Net::SocketReactor;
using Poco::Net::SocketAcceptor;
using Poco::Net::IPAddress;
using std::string;
using std::vector;

extern ofEvent<ofxAMFEvent> ofx_amf_event_dispatcher;

template<typename T>
class ofxAMFSocketAcceptor;


class ofxAMFServer {
public:
	ofxAMFServer();
	~ofxAMFServer();
	void setup(int port); // listens on all IP addresses.
	void setup(string host, int port);
	void addPolicy(string domain, string port);
	void removeClient(ofxAMFConnection* con);
	void addClient(ofxAMFConnection* con);
	bool start();
	
	void notifyEvent(ofxAMFEvent& ev);
	
	// event listener: addListener
	template <typename ArgumentsType, class ListenerClass>
	void addListener(
					ListenerClass* listener
					,void (ListenerClass::*listenerMethod)(ArgumentsType&))
	{
		ofAddListener(ofx_amf_event_dispatcher, listener, listenerMethod);
	}
	
private: 
	string server_host;
	int server_port;
	ServerSocket* socket;
	SocketReactor* reactor;
	SocketAddress* address;
	ofxAMFSocketAcceptor<ofxAMFConnection>* acceptor;
	vector<ofxFlashPolicy> policies;	
	vector<ofxAMFConnection*> clients;
	Thread thread;
};


template<typename T>
class ofxAMFSocketAcceptor : public SocketAcceptor<T> {
public:
	ofxAMFSocketAcceptor(ServerSocket& socket, SocketReactor& reactor, ofxAMFServer* amfServer)
		:SocketAcceptor<T>(socket,reactor)
		,amf_server(amfServer)
	{
	}
	
	ofxAMFServer* amf_server;
protected:
	virtual T* createServiceHandler(StreamSocket& sock) {
		T* obj = new T(sock, *(SocketAcceptor<T>::reactor()));
		obj->setup(amf_server);
		amf_server->addClient(obj);
		return obj;
	}
};
#endif