#ifndef OFXAMFHTTPRESPONSEH
#define OFXAMFHTTPRESPONSEH

#include <sstream>
#include <string>
#include "IOBuffer.h"

using std::stringstream;
using std::string;

/**
 * This class generates a HTTP response buffer, which is
 * send back to the client over a ofxAMFConnction.
 */
 
class ofxAMFHTTPResponse {
public:
	ofxAMFHTTPResponse();
	IOBuffer createHTTPResponse(IOBuffer& buffer);

};

#endif