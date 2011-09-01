#include "ofxAMFMessage.h"
ofxAMFMessage::ofxAMFMessage() 
:response_uri("")
,target_uri("")
,message_type(0)
,message_length(0)
{
}

void ofxAMFMessage::setReponseURI(string uri) {
	response_uri = uri;
}

void ofxAMFMessage::setTargetURI(string uri) {
	target_uri = uri;
}

void ofxAMFMessage::setMessageLength(uint32_t len) {
	message_length = len;
}

void ofxAMFMessage::setMessageType(uint8_t type) {
	message_type = type;
}

string ofxAMFMessage::getResponesURI() {
	return response_uri;
}

string ofxAMFMessage::getTargetURI() {
	return target_uri;
}

uint32_t ofxAMFMessage::getMessageLength() {
	return message_length;
}

uint8_t ofxAMFMessage::getMessageType() {
	return message_type;
}
