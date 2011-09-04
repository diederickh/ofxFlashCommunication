#ifndef OFXAMFMESSAGEH
#define OFXAMFMESSAGEH

#include <inttypes.h>
#include <string>
#include "ofxMissing.h"

using std::string;
using namespace roxlu;

class ofxAMFMessage {
public:
	ofxAMFMessage();
	ofxAMFMessage(const ofxAMFMessage& other);
	void setParams(Dictionary dict);
	void setReturnValues(Dictionary dict);
	void setResponseURI(string uri);
	void setTargetURI(string uri);
	void setMessageLength(uint32_t len);
	void setMessageType(uint8_t type);

	Dictionary& getParams();
	Dictionary& getReturnValues();
	string getResponseURI();
	string getTargetURI();
	uint32_t getMessageLength();
	uint8_t getMessageType();

	bool isAMF3();
	ofxAMFMessage& operator=(const ofxAMFMessage& other);

private:
	void copyFrom(const ofxAMFMessage& other);
	Dictionary params;
	Dictionary return_values;
	string response_uri;
	string target_uri;
	uint32_t message_length;
	uint8_t	message_type;
};

#endif
