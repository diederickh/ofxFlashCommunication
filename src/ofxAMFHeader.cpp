#include "ofxAMFHeader.h"
ofxAMFHeader::ofxAMFHeader() {
}

ofxAMFHeader::ofxAMFHeader(const ofxAMFHeader& other) {
	copyFrom(other);
}

ofxAMFHeader& ofxAMFHeader::operator=(const ofxAMFHeader& other) {
	copyFrom(other);
	return *this;
}


void ofxAMFHeader::copyFrom(const ofxAMFHeader& other) {
	// nothing for now... but be prepared ;-)
}
