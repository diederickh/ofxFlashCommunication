#include "ofxAMFEvent.h"

ofxAMFEvent::ofxAMFEvent(ofxAMFPacket& amfPacket, ofxAMFMessage& amfMessage)
:amf_packet(amfPacket)
,amf_message(amfMessage)
{
}

Dictionary& ofxAMFEvent::getParams() {
	return amf_message.getParams();
}

void ofxAMFEvent::setReturnValues(Dictionary returnValues) {
	amf_message.setReturnValues(returnValues);
}

string ofxAMFEvent::getTargetURI() {
	return amf_message.getTargetURI();
}
