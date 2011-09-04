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

class ofxAMFPacket {
public:
	ofxAMFPacket();
	~ofxAMFPacket();
	ofxAMFPacket(const ofxAMFPacket& other);
	
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
	const vector<ofxAMFHeader*>& getHeadersRef() const;
	uint16_t getNumHeaders();
	
	// messages.
	void addMessage(ofxAMFMessage* message);
	const vector<ofxAMFMessage*> getMessages() const;
	const vector<ofxAMFMessage*>& getMessagesRef() const;
	ofxAMFMessage* getMessage(int dx);
	uint16_t getNumMessages();
	
	ofxAMFPacket& operator=(const ofxAMFPacket& other);
	
private:
	void copyFrom(const ofxAMFPacket& other);
	uint16_t client_version;
	vector<ofxAMFMessage*> messages;
	vector<ofxAMFHeader*> headers;
};
#endif