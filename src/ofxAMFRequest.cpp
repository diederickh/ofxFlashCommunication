#include "ofxAMFRequest.h"
#include "ofxAMFMessage.h"
#include "ofxAMFHeader.h"

ofxAMFRequest::ofxAMFRequest() {
}

// version
// -----------------------------------------------------------------------------
void ofxAMFRequest::setIsAMF3() {
	client_version = AMF3_VERSION;
}

void ofxAMFRequest::setIsAMF0() {
	client_version = AMF0_VERSION;
}

void ofxAMFRequest::setClientVersion(uint16_t version) {
	if(version != AMF0_VERSION && version != AMF3_VERSION) {
		printf("amf request: cannot set version\n");
		return false;
	}
	client_version = version;
	return true;
}

uint16_t ofxAMFRequest::getClientVersion() {
	return client_version;
}


	
// headers
// -----------------------------------------------------------------------------
void ofxAMFRequest::addHeader(ofxAMFHeader* header) {
	headers.push_back(header);
}
	
const vector<ofxAMFHeader*> ofxAMFRequest::getHeaders() const {
	return headers;
}	

uint16_t ofxAMFRequest::getNumHeaders() {
	return (uint16_t)headers.size();
}
	
// messages.
// -----------------------------------------------------------------------------
void ofxAMFRequest::addMessage(ofxAMFMessage* message) {
	messages.push_back(message);
}

const vector<ofxAMFMessage*> ofxAMFRequest::getMessages() const{
	return messages;
}


uint16_t ofxAMFRequest::getNumMessages() {
	return (uint16_t)messages.size();
}
	