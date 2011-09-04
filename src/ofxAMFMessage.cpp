#include "ofxAMFMessage.h"
ofxAMFMessage::ofxAMFMessage() 
:response_uri("")
,target_uri("")
,message_type(0)
,message_length(0)
{
}

ofxAMFMessage::ofxAMFMessage(const ofxAMFMessage& other) {
	copyFrom(other);
}

ofxAMFMessage& ofxAMFMessage::operator=(const ofxAMFMessage& other) {
	copyFrom(other);
	return *this;
}

void ofxAMFMessage::copyFrom(const ofxAMFMessage& other) {
	target_uri = other.target_uri;
	response_uri = other.response_uri;
	params = other.params;
	message_type = other.message_type;
	message_length = other.message_length;
	return_values = other.return_values;
}


void ofxAMFMessage::setParams(Dictionary dict) {
	params = dict;
}

Dictionary& ofxAMFMessage::getParams() {
	return params;
}

void ofxAMFMessage::setReturnValues(Dictionary dict) {
	return_values = dict;
}

Dictionary& ofxAMFMessage::getReturnValues() {
	return return_values;
}

void ofxAMFMessage::setResponseURI(string uri) {
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

string ofxAMFMessage::getResponseURI() {
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
