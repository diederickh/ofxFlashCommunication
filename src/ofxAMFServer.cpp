#include "ofxAMFServer.h"

ofEvent<ofxAMFEvent> ofx_amf_event_dispatcher;

ofxAMFServer::ofxAMFServer()
:server_port(0)
,server_host("")
,socket(NULL)
,reactor(NULL)
,address(NULL)
{
}

ofxAMFServer::~ofxAMFServer() {
	if(socket != NULL) {
		delete socket;
	}
	if(reactor != NULL) {
		delete reactor;
	}
	if(address != NULL) {
		delete address;
	}
}

// use * for host to start listening on all IP addresses
void ofxAMFServer::setup(string host, int port) {
	server_host = host;
	server_port = port;
}


void ofxAMFServer::addPolicy(string domain, string port) {
	policies.push_back(ofxFlashPolicy(domain, port));
}

bool ofxAMFServer::start() {
	if(socket != NULL) {
		ofLogError("ofxAMFServer already created");
		return false;
	}
	
	if(server_host == "*") {	
		IPAddress wildcard_address;
		address = new SocketAddress(wildcard_address, server_port);
	}
	else {
		address = new SocketAddress(server_host, server_port);		
	}
	
	socket = new ServerSocket(*address);
	reactor = new SocketReactor();
	acceptor = new ofxAMFSocketAcceptor<ofxAMFConnection>(*socket, *reactor, this);
	thread.start(*reactor);
	return true;
}

void ofxAMFServer::addClient(ofxAMFConnection* con) {
	clients.push_back(con);
}

void ofxAMFServer::removeClient(ofxAMFConnection* con) {
	vector<ofxAMFConnection*>::iterator it = std::find(clients.begin(), clients.end(), con);
	if(it == clients.end()) {
		ofLogError("Error while trying to remove a client; not found.");
		return;
	}
	ofLogNotice("Removed client");
	clients.erase(it);
}

void ofxAMFServer::notifyEvent(ofxAMFEvent& event) {
	ofNotifyEvent(ofx_amf_event_dispatcher, event);
}