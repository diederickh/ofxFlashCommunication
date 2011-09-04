#ifndef OFXAMFHTTPRESPONSEH
#define OFXAMFHTTPRESPONSEH

#include <sstream>
#include <string>
#include "IOBuffer.h"

using std::stringstream;
using std::string;

class ofxAMFHTTPResponse {
public:
	ofxAMFHTTPResponse();
	IOBuffer createHTTPResponse(IOBuffer& buffer);

};

#endif