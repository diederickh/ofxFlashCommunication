#ifndef OFXAMFMESSAGEH
#define OFXAMFMESSAGEH

#include <vector>
#include <string>
#include <inttypes.h>
#include "ofxAMFTypes.h"

using std::vector;
using std::string;

class ofxAMFHeader;
class ofxAMFMessage;

class ofxAMFRequest {
public:
	ofxAMFRequest();

	// general
	void setIsAMF3();
	void setIsAMF0();
	bool setClientVersion(uint16_t version);
	uint16_t getClientVersion();
	
	// header
	void addHeader(ofxAMFHeader* header);
	const vector<ofxAMFHeader*> getHeaders() const;
	uint16_t getNumHeaders();
	
	// messages.
	void addMessage(ofxAMFMessage* message);
	const vector<ofxAMFMessage*> getMessages() const;
	uint16_t getNumMessages();
	
private:
	uint16_t client_version;
	vector<ofxAMFMessage*> messages;
	vector<ofxAMFHeader*> headers;
};
#endif