#ifndef OFXAMFHEADERH
#define OFXAMFHEADERH

class ofxAMFHeader {
public:
	ofxAMFHeader();
	ofxAMFHeader(const ofxAMFHeader& other);
	ofxAMFHeader& operator=(const ofxAMFHeader& other);
	
private:
	void copyFrom(const ofxAMFHeader& other);
};

#endif