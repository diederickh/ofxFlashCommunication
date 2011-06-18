#include "ofxFlashCommunication.h"
ofxFlashCommunication::ofxFlashCommunication() 
:socket(NULL)
,reactor(NULL)
,address(NULL)
,acceptor(NULL)
,ip("")
,port(0)
,buffer(new char[BUFFER_SIZE])
{

}

ofxFlashCommunication::~ofxFlashCommunication() {
	try {
		delete socket;
		delete reactor;
		delete address;
		delete acceptor;
		delete reactor;
	}
	catch(...) {
	}
}

void ofxFlashCommunication::setup(string sIP, int nPort) {
	ip = sIP;
	port = nPort;
}

bool ofxFlashCommunication::start() {
	if(socket != NULL) {
		return false;
	}
	address 	= new SocketAddress(ip,port);
	socket 		= new ServerSocket(*address);
	reactor 	= new SocketReactor();
	acceptor	= new ofxFlashSocketAcceptor<ofxFlashConnection>(*socket, *reactor, this);
	thread.start(*reactor);
	return true;
}

void ofxFlashCommunication::addPolicy(string sDomain, string sPort) {
	policies.push_back(ofxFlashPolicy(sDomain, sPort));
}

string ofxFlashCommunication::getPoliciesXML() {
	std::vector<ofxFlashPolicy>::iterator it = policies.begin();
	string policy = "<cross-domain-policy>";
	while(it != policies.end()) {	
		policy += (*it).getXML();
		++it;
	}
	policy += "</cross-domain-policy>";
	policy += '\0'; // we need this: http://help.adobe.com/en_US/as3/dev/WS5b3ccc516d4fbf351e63e3d118a9b90204-7c60.html
	return policy;
}

void ofxFlashCommunication::addClient(ofxFlashConnection* pClient) {
	clients.push_back(pClient);
}

void ofxFlashCommunication::removeClient(ofxFlashConnection* pClient) {
	vector<ofxFlashConnection*>::iterator it = clients.begin();
	while(it != clients.end()) {
		if((*it) == pClient) {
			clients.erase(it);
			break;
		}
		++it;
	}
}

void ofxFlashCommunication::send(string sData) {
	sData.push_back('\0');
	vector<ofxFlashConnection*>::iterator it = clients.begin();
	while(it != clients.end()) {
		(*it)->write(sData);
		++it;
	}
}

void ofxFlashCommunication::addMessage(string sMessage) {
	Mutex::ScopedLock lock(mutex);
	messages.push_back(sMessage);
}

bool ofxFlashCommunication::hasMessage() {
	Mutex::ScopedLock lock(mutex);
	return messages.size() > 0;
}

string ofxFlashCommunication::getNextMessage() {
	string msg = "EMPTY";
	Mutex::ScopedLock lock(mutex);
	if(messages.size() > 0) {
		msg = messages.front();
		messages.erase(messages.begin());
	}
	return msg;
}

