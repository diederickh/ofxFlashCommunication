#include "ofxAMFHTTPResponse.h"
ofxAMFHTTPResponse::ofxAMFHTTPResponse() {
}

// prepends the headers to the buffer for a http resonse.
IOBuffer ofxAMFHTTPResponse::createHTTPResponse(IOBuffer& buffer) {
	IOBuffer http_buffer;
	http_buffer.setup(buffer.getNumBytesStored());
	http_buffer.storeString("HTTP/1.1 200 OK\r\n");
	http_buffer.storeString("Date: Sat, 03 Sep 2011 12:35:01 GMT\r\n");
	http_buffer.storeString("Server: Apache/2.2.17 (Unix) mod_ssl/2.2.17 OpenSSL/0.9.8r DAV/2 PHP/5.3.3\r\n");
	http_buffer.storeString("X-Powered-By: PHP/5.3.3\r\n");
	stringstream ss;
	ss << "Content-Length: " << buffer.getNumBytesStored() << "\r\n";
	http_buffer.storeString(ss.str());
	
	http_buffer.storeString("Connection: close\r\n");
	http_buffer.storeString("Content-type: application/x-amf\r\n");
	http_buffer.storeString("\r\n");
	
	http_buffer.storeBuffer(buffer);
	return http_buffer;

	
}
