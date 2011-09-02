#ifndef OFXAMFMESSAGEH
#define OFXAMFMESSAGEH

#include <inttypes.h>
#include <string>
#include "Dictionary.h"

using std::string;

class ofxAMFMessage {
public:
	ofxAMFMessage();
	void setData(Dictionary dict);	
	void setResponseURI(string uri);
	void setTargetURI(string uri);
	void setMessageLength(uint32_t len);
	void setMessageType(uint8_t type);

	Dictionary& getData();
	string getResponseURI();
	string getTargetURI();
	uint32_t getMessageLength();
	uint8_t getMessageType();
	
	bool isAMF3();
	

private:
	Dictionary data;
	string response_uri;
	string target_uri;
	uint32_t message_length;
	uint8_t	message_type;
};

#endif