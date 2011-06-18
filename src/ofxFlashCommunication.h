#pragma once

// Include all the lovely Poco stuff
// ---------------------------------
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

#include <string>
#include <vector>

#include "ofxFlashConnection.h"

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

using namespace std;

// Container for flash policies
// ----------------------------
struct ofxFlashPolicy {
	ofxFlashPolicy(string sDomain,string sPort)
	:domain(sDomain)
	,port(sPort)
	{
	}
						//		+"secure=\"false\" " 
	std::string getXML() {
			std::string xml = "<allow-access-from " \
								"domain=\"" +domain +"\" " \
								"secure=\"false\" " \
								"to-ports=\"" +port +"\" />";
		return xml;
	}
	string domain;
	string port;
};

// Customer factory which creates connection instances
// ----------------------------------------------------
template<typename T>
class ofxFlashSocketAcceptor;

// The server to which flash connects
// ----------------------------------
class ofxFlashCommunication {
public:
	ofxFlashCommunication();
	~ofxFlashCommunication();
	void 	addPolicy(string sDomain, string sPort);
	void 	setup(string sIP, int nPort);
	bool 	start();
	void 	send(string sData);
	string 	getPoliciesXML();
	void 	addClient(ofxFlashConnection* pClient);
	void 	removeClient(ofxFlashConnection* pClient);
	void 	addMessage(string sMessage);
	bool	hasMessage();
	string	getNextMessage();
private:
	char* 			buffer;
	int				port;
	string			ip;
	Thread			thread;
	Mutex 			mutex;
	ServerSocket*	socket;
	SocketReactor*	reactor;
	SocketAddress*	address;
	
	ofxFlashSocketAcceptor<ofxFlashConnection>* acceptor;
	vector<ofxFlashPolicy>						policies;
	vector<ofxFlashConnection*> 				clients;
	vector<string> 								messages;
	vector<string>::iterator					messages_it;
	enum { BUFFER_SIZE = 1024 };
};

template<typename T>
class ofxFlashSocketAcceptor : public SocketAcceptor<T> {
public:
	ofxFlashSocketAcceptor(ServerSocket& socket, SocketReactor& reactor, ofxFlashCommunication* pCom)
		:SocketAcceptor<T>(socket,reactor)
		,com(pCom)
	{
	}
	
	ofxFlashCommunication* com;
protected:
	virtual T* createServiceHandler(StreamSocket& sock) {
		T* obj = new T(sock, *(SocketAcceptor<T>::reactor()));
		obj->setup(com);
		com->addClient(obj);
		return obj;
	}
};


