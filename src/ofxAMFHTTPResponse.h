#ifndef OFXAMFHTTPRESPONSEH
#define OFXAMFHTTPRESPONSEH

#include <sstream>
#include <string>
#include "ofxMissing.h"

using std::stringstream;
using std::string;

class ofxAMFHTTPResponse {
public:
	ofxAMFHTTPResponse();
	IOBuffer createHTTPResponse(IOBuffer& buffer);

};

#endif
