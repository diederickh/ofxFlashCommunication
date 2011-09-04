#ifndef OFXAMFEVENTH
#define OFXAMFEVENTH

#include "ofxMissing.h"
#include "ofxAMFPacket.h"
#include "ofxAMFMessage.h"

using namespace roxlu;

class ofxAMFEvent {
public:
	ofxAMFEvent(ofxAMFPacket& amfPacket, ofxAMFMessage& amfMessage);
	Dictionary& getParams();
	void setReturnValues(Dictionary resultValues);
	string getTargetURI(); // what's called
private:
	ofxAMFPacket& amf_packet;
	ofxAMFMessage& amf_message; // belongs to amf_packet

};

#endif