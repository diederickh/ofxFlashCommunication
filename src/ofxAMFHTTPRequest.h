#ifndef OFXAMFHTTPREQUESTH
#define OFXAMFHTTPREQUESTH

#include <iostream>
#include <string>

#include "Poco/Exception.h"
#include "Poco/Thread.h"
#include "Poco/StringTokenizer.h"
#include "Poco/String.h"
#include "ofxMissing.h"

using Poco::StringTokenizer;
using Poco::toLowerInPlace;
using std::string;
using namespace roxlu;

class ofxAMFHTTPRequest {
public:
	ofxAMFHTTPRequest();
	bool parseHTTPRequest(IOBuffer& buffer, IOBuffer& amfResultBuffer);

private:
	bool parseHTTPHeaders();
	IOBuffer amf_buffer;
	uint32_t num_bytes_in_header;
	uint32_t num_bytes_in_body;
	string raw_headers;
	Dictionary headers;
};

#endif
