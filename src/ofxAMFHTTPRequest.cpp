#include "ofxAMFHTTPRequest.h"
#include "ofMain.h"  // temp debuggin.

ofxAMFHTTPRequest::ofxAMFHTTPRequest() 
:num_bytes_in_header(0)
,raw_headers("")
,num_bytes_in_body(0)
{
}

bool ofxAMFHTTPRequest::parseHTTPRequest(IOBuffer& buffer, IOBuffer& amfResultBuffer) {
	// step 1. parse http headers
	if(num_bytes_in_header == 0) {
		num_bytes_in_header = buffer.consumeUntil("\r\n\r\n", raw_headers);
	}
	
	// step 2. parse headers when all received.
	if(num_bytes_in_header != 0) {
		if(parseHTTPHeaders()) {
			num_bytes_in_body = headers["content-length"].getAsUInt32();
		}
	}
	else {
		return false;
	}
	
	// step 3. parse body when received content-length.
	if(num_bytes_in_body != 0) {
		uint32_t bytes_stored = buffer.getNumBytesStored();
		uint32_t bytes_total = num_bytes_in_header + num_bytes_in_body;
		uint32_t bytes_left = bytes_total - bytes_stored;
		if(bytes_left == 0) {
			amfResultBuffer.storeBuffer(buffer, num_bytes_in_body);
			//cout << headers.toString() << endl;
			return true;
		}
	}
	return false;
}


bool ofxAMFHTTPRequest::parseHTTPHeaders() {
	StringTokenizer header_tokens(
		 raw_headers
		,"\r\n"
		,StringTokenizer::TOK_TRIM | StringTokenizer::TOK_IGNORE_EMPTY
	);
	
	size_t num_tokens = header_tokens.count();
	for(int i = 0; i < num_tokens; ++i) {
	
		StringTokenizer spec_token(
			header_tokens[i]
			,":"
			,StringTokenizer::TOK_TRIM | StringTokenizer::TOK_IGNORE_EMPTY
		);
		
		// 0 = GET or POST
		if(i > 0 && spec_token.count() < 2) {
			ofLogError("ofxamfhttprequest: parse http header error: spec count not 2 for: " +header_tokens[i]);
			return false;
		}
		if(i == 0) {
			// @todo add request type (post or get)
		}
		else if(i > 0) {
			string spec_val = "";
			for(int j = 1; j < spec_token.count(); ++j) {
				spec_val += spec_token[j] +((j < spec_token.count()-1) ? ":" : "");
			}
			string spec_name = spec_token[0];
			toLowerInPlace(spec_name);
			headers[spec_name] = spec_val;
		}
	}
	return true;
}
