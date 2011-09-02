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
,num_content_bytes_received(0)
,state(RETRIEVE_HEADER)
{
	buffer.setup();
	reactor.addEventHandler(socket, NObserver<ofxAMFConnection, ReadableNotification>(*this, &ofxAMFConnection::onReadable));
	reactor.addEventHandler(socket, NObserver<ofxAMFConnection, ShutdownNotification>(*this, &ofxAMFConnection::onShutdown));
}

ofxAMFConnection::~ofxAMFConnection() {
	printf("hier");
	reactor.removeEventHandler(socket, NObserver<ofxAMFConnection, ReadableNotification>(*this, &ofxAMFConnection::onReadable));
	reactor.removeEventHandler(socket, NObserver<ofxAMFConnection, ShutdownNotification>(*this, &ofxAMFConnection::onShutdown));
	// @todo make sure to remove this...
	amf_server->removeClient(this);
	amf_server = NULL;
	printf("removed ofxAMFConnection");
}

void ofxAMFConnection::setup(ofxAMFServer* amfServer) {
	amf_server = amfServer;
}

void ofxAMFConnection::onReadable(const AutoPtr<ReadableNotification>& pNotif) {	
	uint8_t tmp[1024];
	int n = socket.receiveBytes(tmp, 1024);
	cout << "\n------------ received ---------------\n";
	for(int i = 0; i < n; ++i) {
		cout << (char)tmp[i];
	}
	cout << "\n-------------------------------------";
	cout << "\n\n\n";
	
	// @todo - this is not working nicely at the moment, because we 
	// need to parse the HTTP header better. We need to make sure that we have
	// read until the "\r\n\r\n" which means we have read the complete header,
	// then we need to check if we have received the total "Content-length"
	cout << "got:" << n << " bytes" << endl;
	if(n > 0) {
		// add the new bytes to the buffer.
		buffer.storeBytes(tmp, n);
	
		// step 1: read http headers	
		if(state == RETRIEVE_HEADER) {
			string raw_headers;
			int until = buffer.consumeUntil("\r\n\r\n", raw_headers);
			if(until) {
				Dictionary clean_headers;
				if(parseHTTPHeaders(raw_headers, clean_headers)) {
					content_length = clean_headers["content-length"].asUInt32();
				}
				state = RETRIEVE_CONTENT;
			}
		}
		
		if(state == RETRIEVE_CONTENT) {
			// when we retrieved all data, deserialize it.
			num_content_bytes_received += n;
			if(num_content_bytes_received >= content_length) {
				// reset state.
				state = RETRIEVE_HEADER; 
				num_content_bytes_received = 0;
				
				// parse packet
				ofxAMFPacket request = amf3.deserialize(buffer);
				
				// @todo notify for new request.
				ofxAMFPacket response = request;
				IOBuffer response_buffer = amf3.serialize(response);
				response_buffer.printHex();
				
				
			}
			else {
				cout << "no:" << num_content_bytes_received << endl;
			}
		}
	}
	else {
	
		delete this;
	}
}




bool ofxAMFConnection::parseHTTPHeaders(string& headers, Dictionary& result) {
	cout << "Parse http headers" << endl;
	cout << "Parsing raw headers:" << endl;
	cout << "------------------------------------------------" <<endl;
	cout << headers << endl;
	cout << "------------------------------------------------" <<endl;
	
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
			cout << spec_name << " = " << spec_val<< endl;
			result[spec_name] = spec_val;
		}
	}
	return true;
}

void ofxAMFConnection::parseBuffer() {
	uint8_t* ptr = buffer.getPtr();
}

void ofxAMFConnection::onShutdown(const AutoPtr<ShutdownNotification>& pNotif) {
	delete this;
}

int ofxAMFConnection::write(string sData) {
	int n = sData.size();
	int done = socket.sendBytes(sData.c_str(),sData.size());
	// untested (cannot find a way to get inside this loop on Mac)
	// then number of bytes actually sent can be less than requested
	// as stated in the documentation. 
	// http://pocoproject.org/docs/
	while(n < done) {
		done+= socket.sendBytes(sData.c_str(),sData.size());
	}
	return done;
}