#include "ofxAMFConnection.h"
#include "ofxAMFServer.h"
#include "ofxFlashCommunication.h"

ofxAMFConnection::ofxAMFConnection(
	 StreamSocket rSocket
	,SocketReactor& rReactor
)
:socket(rSocket)
,reactor(rReactor)
,amf_server(NULL)
,content_length(0)
,start_body(0)
,bytes_waiting(UINT32_MAX)
,state(RETRIEVE_HEADER)
{
	buffer.setup();
	amf_request_buffer.setup();
	reactor.addEventHandler(socket, NObserver<ofxAMFConnection, ReadableNotification>(*this, &ofxAMFConnection::onReadable));
	reactor.addEventHandler(socket, NObserver<ofxAMFConnection, ShutdownNotification>(*this, &ofxAMFConnection::onShutdown));
}

ofxAMFConnection::~ofxAMFConnection() {
	reactor.removeEventHandler(socket, NObserver<ofxAMFConnection, ReadableNotification>(*this, &ofxAMFConnection::onReadable));
	reactor.removeEventHandler(socket, NObserver<ofxAMFConnection, ShutdownNotification>(*this, &ofxAMFConnection::onShutdown));
	amf_server->removeClient(this);
	amf_server = NULL;
}

void ofxAMFConnection::setup(ofxAMFServer* amfServer) {
	amf_server = amfServer;
}

void ofxAMFConnection::onReadable(const AutoPtr<ReadableNotification>& pNotif) {	
	uint8_t tmp[1024];
	int n = socket.receiveBytes(tmp, 1024);
	cout << "\n-------------------------- received --------------------------------\n";
	for(int i = 0; i < n; ++i) {
		cout << (char)tmp[i];
	}
	cout << "\n--------------------------------------------------------------------\n";
	cout << "\n";
	cout << flush;
	
	// @todo - this is not working nicely at the moment, because we 
	// need to parse the HTTP header better. We need to make sure that we have
	// read until the "\r\n\r\n" which means we have read the complete header,
	// then we need to check if we have received the total "Content-length"
	cout << "ofxamfconnection: received bytes: " << n << " bytes." << endl;
	if(n > 0) {
		buffer.storeBytes(tmp, n);
		if(amf_http_request.parseHTTPRequest(buffer, amf_request_buffer)) {
			cout << "ofxamfconnection: we have a complete amf request" << endl;
			deserializeRequest();
		}
		else {
			cout << "ofxamfconnection: cannot parse http request yet." << endl;
		}
		
		/*
		// add the new bytes to the buffer.
		buffer.storeBytes(tmp, n);
		cout << "Bytes stored in buffer: " << buffer.getNumBytesStored() << endl; 
		// step 1: read http headers	
		if(state == RETRIEVE_HEADER) {
			string raw_headers;
			start_body = buffer.consumeUntil("\r\n\r\n", raw_headers);
			if(start_body) {
				Dictionary clean_headers;
				if(parseHTTPHeaders(raw_headers, clean_headers)) {
					content_length = clean_headers["content-length"].getAsUInt32();
				}
				cout << "Found end marker of http header at: " << start_body << endl;
				cout << "Found this as content length: " << content_length << endl;
				cout << "----------------- hex which is stored in buffer now -----------------\n";
				//buffer.printHex(0);
				cout << "\n--------------------------------------------------------------------\n";
			
			
				uint32_t bytes_stored = buffer.getNumBytesStored();
				uint32_t bytes_expected = start_body + content_length;
				bytes_waiting = bytes_expected - bytes_stored;
				cout << "Bytes to received yet: " << bytes_waiting << endl;
				state = RETRIEVE_CONTENT;
				if(!parseContentBufferWhenComplete()) {
					return;
				}
				
			}
		
			
		}

		if(state == RETRIEVE_CONTENT) {
			cout << "Currently waiting for: " << bytes_waiting << endl;
			cout << "Currently received: " << n << endl;
			bytes_waiting -= n;
			cout << "State is:" << state << endl;
			cout << "Waiting for: " << bytes_waiting << " number of bytes" << endl;
			parseContentBufferWhenComplete();
			
		}
			*/
	}
	else {
	
		delete this;
	}
}

bool ofxAMFConnection::parseContentBufferWhenComplete() {
	/*
	if(bytes_waiting == 0) {
		// reset state.
		state = RETRIEVE_HEADER; 
		deserializeRequest();
		return true;
		
	}
	*/
	return false;
}

// @todo maybe create ofxAMFHTTPRequest() as we have a ofxAMFHTTPResponse()
bool ofxAMFConnection::parseHTTPHeaders(string& headers, Dictionary& result) {
/*
	cout << endl;
	cout << "----------------------- raw http headers -----------------------------\n";
	cout << headers << endl;
	cout << "\n--------------------------------------------------------------------\n";
	
	StringTokenizer header_tokens(
		 headers
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
			printf( "parse http header error: spec count not 2 for: %s\n"
					,header_tokens[i].c_str()
			);
			return false;
		}
		if(i == 0) {
			// @todo add request type (post or get)
		}
		// for
		else if(i > 0) {
			string spec_val = "";
			for(int j = 1; j < spec_token.count(); ++j) {
				spec_val += spec_token[j] +((j < spec_token.count()-1) ? ":" : "");
			}
			string spec_name = spec_token[0];
			toLowerInPlace(spec_name);
			cout << "\t >> "  << spec_name << " = " << spec_val<< endl;
			result[spec_name] = spec_val;
		}
	}
	cout << "\n--------------------------------------------------------------------\n";
	return true;
	*/
	return false;
}

void ofxAMFConnection::deserializeRequest() {
//	cout << "----------------- RECEIVED AMF message ---------------------------------------\n";
//	buffer.printHex(start_body);
//	cout << "\n--------------------------------------------------------------------\n";
	
	// parse packet
	ofxAMFPacket request = amf3.deserialize(amf_request_buffer);
	
	// @todo notify for new request.
	// @todo testing serializing....
	ofxAMFPacket response = request; // makes a complete copy!! (new instances created)
	IOBuffer response_buffer = amf3.serialize(response);
	cout << "-------------------------- going to send: ------------------------\n";
	response_buffer.printHex();
	cout << "\n---------------------------------------------------------------\n";

	ofxAMFHTTPResponse http_response;
	IOBuffer http_buffer = http_response.createHTTPResponse(response_buffer);
	write(http_buffer);
	cout << "================= HTTP RESPONSE ==============================" << endl;
	http_buffer.printHex();
	cout << "\n\n";
//	delete this;

			
}

void ofxAMFConnection::onShutdown(const AutoPtr<ShutdownNotification>& pNotif) {
	delete this;
}

int ofxAMFConnection::write(IOBuffer& amfBuffer) {
	int n =  amfBuffer.getNumBytesStored();
	int bytes_left = n;
	char* buffer_ptr = (char*)amfBuffer.getPtr();
	
	int done = socket.sendBytes(buffer_ptr, n);
	bytes_left -= done;
	cout << "Writing to socket, send: " << done << " number of bytes already." << endl;
	
	// untested (cannot find a way to get inside this loop on Mac)
	// then number of bytes actually sent can be less than requested
	// as stated in the documentation. 
	// http://pocoproject.org/docs/
	while(bytes_left > 0) {
		done = socket.sendBytes(buffer_ptr+done,bytes_left);
		bytes_left -= done;
	}
	return done;
}