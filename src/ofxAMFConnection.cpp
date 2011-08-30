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
	// @todo - this is not working nicely at the moment, because we 
	// need to parse the HTTP header better. We need to make sure that we have
	// read until the "\r\n\r\n" which means we have read the complete header,
	// then we need to check if we have received the total "Content-length"
	
	if(n > 0) {
		buffer.storeBytes(tmp, n);
		string line;
		int read_until = buffer.consumeUntil("\r\n\r\n", line);
		cout << "we have read until:" << read_until << ", and consumed:" << buffer.consumed <<endl;
		cout << "------------------------------" << endl;
		buffer.printHex(read_until, n);
		cout << "------------------------------" << endl;
		if(read_until) {
			
			// read version
			Dictionary version;
			amf3.readUint16(buffer,version);
			
			// read number of headers.
			Dictionary header_count;
			amf3.readUint16(buffer,header_count);
			cout << "header count:" << (string)header_count << endl;
			
			// process headers
			for(int i = 0; i < (uint16_t)header_count; ++i) {
				cout << "TODO processing header:" << i << endl;
				
			}
			
			// read number of messages
			Dictionary message_count;
			amf3.readUint16(buffer,message_count);
			cout << "message count:" << (string)message_count << endl;
					
			// process messages			
			for(int i = 0; i < (uint16_t)message_count; ++i) {
				
				// get object to be called, ie. YourClass.SomeMethod
				Dictionary target_uri;
				amf3.readUTF(buffer, target_uri);
				cout << "target uri:" << (string)target_uri << endl;
				
				// aka: /1, /2, /3 (unique identifier)
				Dictionary response_uri;
				amf3.readUTF(buffer, response_uri);
				cout << "response uri:" << (string)response_uri << endl;
				
				// get message body length
				/*
				Dictionary message_body_length;
				amf3.readUint32(buffer, message_body_length);
				cout << "message body length:" << (string)message_body_length << endl;
				*/
				buffer.ignore(4);
				
				// value type
				uint8_t marker;
				buffer.consumeByte(marker);
				printf("marker: %02X\n", marker);
				
				if(marker == 0x0a) {
					Dictionary num_elements;
					amf3.readUint32(buffer, num_elements);
					cout << "number of elements in array: " << (string)num_elements << endl;
					for(uint32_t i = 0; i < (uint32_t)num_elements; ++i) {
						uint8_t type;
						buffer.consumeByte(type);

						// string
						if(type == 0x02) {
							Dictionary str;
							amf3.readUTF(buffer, str);
							cout << "got string: '" << (string)str << "'"  << endl;
						}
					}
				}

				
			}
			
		}
	}
	else {
	
		delete this;
	}
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