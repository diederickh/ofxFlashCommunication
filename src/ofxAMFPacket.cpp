#include "ofxAMFPacket.h"
#include "ofxAMFMessage.h"
#include "ofxAMFHeader.h"
#include "ofMain.h"
ofxAMFPacket::ofxAMFPacket() {
}

ofxAMFPacket::~ofxAMFPacket() {
	{
		vector<ofxAMFHeader*>::iterator it = headers.begin();
		while(it != headers.end()) {
			*it = NULL;
			++it;
		}
	}
	{
		vector<ofxAMFMessage*>::iterator it = messages.begin();
		while(it != messages.end()) {
			delete *it;
			++it;
		}
	}
}

// Copying
// -----------------------------------------------------------------------------
ofxAMFPacket::ofxAMFPacket(const ofxAMFPacket& other) {
	copyFrom(other);
}

ofxAMFPacket& ofxAMFPacket::operator=(const ofxAMFPacket& other) {
	copyFrom(other);
	return *this;
}

void ofxAMFPacket::copyFrom(const ofxAMFPacket& other) {
	client_version = other.client_version;

	// copy headers
	{
		vector<ofxAMFHeader*>::const_iterator it = other.headers.begin();
		while(it != other.headers.end()) {
			ofxAMFHeader* copied_header = new ofxAMFHeader(*(*it));
			addHeader(copied_header);
			++it;
		}
	}

	// copy messages.
	{
		vector<ofxAMFMessage*>::const_iterator it = other.messages.begin();
		while(it != other.messages.end()) {
			ofxAMFMessage* copied_message = new ofxAMFMessage(*(*it));
			addMessage(copied_message);
			++it;
		}
	}
}

// version
// -----------------------------------------------------------------------------
void ofxAMFPacket::setIsAMF3() {
	client_version = AMF3_VERSION;
}

void ofxAMFPacket::setIsAMF0() {
	client_version = AMF0_VERSION;
}

bool ofxAMFPacket::isAMF3() {
	return client_version == AMF3_VERSION;
}

bool ofxAMFPacket::isAMF0() {
	return client_version == AMF0_VERSION;
}

bool ofxAMFPacket::setClientVersion(uint16_t version) {
	if(version != AMF0_VERSION && version != AMF3_VERSION) {
		printf("amf request: cannot set version: %02X\n", version);
		return false;
	}
	client_version = version;
	return true;
}

uint16_t ofxAMFPacket::getClientVersion() {
	return client_version;
}


	
// headers
// -----------------------------------------------------------------------------
void ofxAMFPacket::addHeader(ofxAMFHeader* header) {
	headers.push_back(header);
}
	
const vector<ofxAMFHeader*> ofxAMFPacket::getHeaders() const {
	return headers;
}	

uint16_t ofxAMFPacket::getNumHeaders() {
	return (uint16_t)headers.size();
}

const vector<ofxAMFHeader*>& ofxAMFPacket::getHeadersRef() const {
	return headers;
}
	
// messages.
// -----------------------------------------------------------------------------
void ofxAMFPacket::addMessage(ofxAMFMessage* message) {
	messages.push_back(message);
}

const vector<ofxAMFMessage*> ofxAMFPacket::getMessages() const{
	return messages;
}

const vector<ofxAMFMessage*>& ofxAMFPacket::getMessagesRef() const {
	return messages;
}

uint16_t ofxAMFPacket::getNumMessages() {
	return (uint16_t)messages.size();
}

ofxAMFMessage* ofxAMFPacket::getMessage(int dx) {
	return messages.at(dx);
}

	