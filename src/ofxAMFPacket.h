#ifndef OFXAMFPACKET
#define OFXAMFPACKET

#include <vector>
#include <string>
#include <inttypes.h>
#include "ofxAMFTypes.h"

using std::vector;
using std::string;

class ofxAMFHeader;
class ofxAMFMessage;
//#include "ofxAMFMessage.h"

class ofxAMFPacket {
public:
	ofxAMFPacket();
	~ofxAMFPacket();
	
	// general
	bool isAMF3();
	bool isAMF0();
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
	ofxAMFMessage* getMessage(int dx);
	uint16_t getNumMessages();
	
private:
	uint16_t client_version;
	vector<ofxAMFMessage*> messages;
	vector<ofxAMFHeader*> headers;
};
#endif