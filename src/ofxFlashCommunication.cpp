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
	delete socket;
	delete reactor;
	delete address;
	delete acceptor;
	delete reactor;
}

void ofxFlashCommunication::setup(string sIP, int nPort) {
	ip = sIP;
	port = nPort;
}

bool ofxFlashCommunication::start() {
	if(socket != NULL) {
		return false;
	}
	address = new SocketAddress(ip,port);
	socket = new ServerSocket(*address);
	reactor = new SocketReactor();
	acceptor = new ofxFlashSocketAcceptor<ofxFlashConnection>(*socket, *reactor, this);
	reactor->addEventHandler(*socket, NObserver<ofxFlashCommunication, ReadableNotification>(*this, &ofxFlashCommunication::onConnect));
	thread.start(*reactor);
	return true;
}

void ofxFlashCommunication::addPolicy(string sDomain, string sPort) {
	policies.push_back(ofxFlashPolicy(sDomain, sPort));
}

// @todo remove this.
void ofxFlashCommunication::onConnect(const AutoPtr<ReadableNotification>& pNotif) {
}

string ofxFlashCommunication::getPoliciesXML() {
	std::vector<ofxFlashPolicy>::iterator it = policies.begin();
	string policy = "<?xml version=\"1.0\"?>" \
					"<!DOCTYPE cross-domain-policy SYSTEM \"http://www.adobe.com/xml/dtds/cross-domain-policy.dtd\">" \
					"<cross-domain-policy>";
	while(it != policies.end()) {	
		policy += (*it).getXML();
		++it;
	}
	policy += "</cross-domain-policy>";
	/*
	policy.push_back(0x00);
	policy.push_back(0x0D);
	policy.push_back(0x0A);
	*/
	//policy += '\0'; 
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
	vector<ofxFlashConnection*>::iterator it = clients.begin();
	while(it != clients.end()) {
		(*it)->write(sData);
		++it;
	}
}