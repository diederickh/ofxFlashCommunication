#include "ofxAMFConnection.h"
#include "ofxAMFServer.h"
#include "ofxFlashCommunication.h"
#include <inttypes.h>

int ofxAMFConnection::count = 0;

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
{
	//buffer.setup();
	amf_request_buffer.setup();
	reactor.addEventHandler(socket, NObserver<ofxAMFConnection, ReadableNotification>(*this, &ofxAMFConnection::onReadable));
	reactor.addEventHandler(socket, NObserver<ofxAMFConnection, ShutdownNotification>(*this, &ofxAMFConnection::onShutdown));
	count++;
	cout << "ofxAMFConnction - created: " << count << endl;
}

ofxAMFConnection::~ofxAMFConnection() {
	reactor.removeEventHandler(socket, NObserver<ofxAMFConnection, ReadableNotification>(*this, &ofxAMFConnection::onReadable));
	reactor.removeEventHandler(socket, NObserver<ofxAMFConnection, ShutdownNotification>(*this, &ofxAMFConnection::onShutdown));
	amf_server->removeClient(this);
	amf_server = NULL;
	count--;
	cout << "ofxAMFConnction - deleted: " << count << endl;
}


void ofxAMFConnection::setup(ofxAMFServer* amfServer) {
	amf_server = amfServer;
}


// Read some data from the socket.
// --------------------------------
void ofxAMFConnection::onReadable(const AutoPtr<ReadableNotification>& pNotif) {
	uint8_t tmp[1024];
	int n = socket.receiveBytes(tmp, 1024);
	if(n > 0) {
		buffer.storeBytes(tmp, n);
		if(amf_http_request.parseHTTPRequest(buffer, amf_request_buffer)) {
			deserializeRequest(); // uncomment
			//delete this; // tmp
		}
	}
	else {
		delete this;
	}
}


// When we received a complete AMF message, deserialize it.
// --------------------------------------------------------
void ofxAMFConnection::deserializeRequest() {
	ofxAMFPacket request = amf3.deserialize(amf_request_buffer);

	const vector<ofxAMFMessage*>& amf_messages = request.getMessagesRef();
	vector<ofxAMFMessage*>::const_iterator it = amf_messages.begin();
	while(it != amf_messages.end()) {
		// notify.
		ofxAMFEvent ev(request, *(*it));
		amf_server->notifyEvent(ev);
		++it;
	}


	// Create response buffer with return values.	
	IOBuffer response_buffer = amf3.serialize(request);
	
	ofxAMFHTTPResponse http_response;
	IOBuffer http_buffer = http_response.createHTTPResponse(response_buffer);
	write(http_buffer);
}

void ofxAMFConnection::onShutdown(const AutoPtr<ShutdownNotification>& pNotif) {
	delete this;
}


// Write a AMF buffer back to the client.
// --------------------------------------
int ofxAMFConnection::write(IOBuffer& amfBuffer) {
	int n =  amfBuffer.getNumBytesStored();
	int bytes_left = n;
	char* buffer_ptr = (char*)amfBuffer.getPtr();

	int done = socket.sendBytes(buffer_ptr, n);
	bytes_left -= done;


	while(bytes_left > 0) {
		done = socket.sendBytes(buffer_ptr+done,bytes_left);
		bytes_left -= done;
	}
	return done;
}
