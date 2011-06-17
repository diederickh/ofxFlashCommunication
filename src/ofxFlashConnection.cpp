#include "ofxFlashConnection.h"
#include "ofxFlashCommunication.h"

ofxFlashConnection::ofxFlashConnection(
	 StreamSocket rSocket
	,SocketReactor& rReactor
)
:socket(rSocket)
,reactor(rReactor)
,com(NULL)
,raw(new char[BUFFER_SIZE])
{
	reactor.addEventHandler(socket, NObserver<ofxFlashConnection, ReadableNotification>(*this, &ofxFlashConnection::onReadable));
	reactor.addEventHandler(socket, NObserver<ofxFlashConnection, ShutdownNotification>(*this, &ofxFlashConnection::onShutdown));
}

ofxFlashConnection::~ofxFlashConnection() {
	com->removeClient(this);
}

void ofxFlashConnection::setup(ofxFlashCommunication* pCom) {
	com = pCom;
}
void ofxFlashConnection::onReadable(const AutoPtr<ReadableNotification>& pNotif) {
	int n = socket.receiveBytes(raw, BUFFER_SIZE);
	if(n > 0) {
		for(int i = 0; i < n; ++i) {
			buffer.push_back(raw[i]);
		}
		parseBuffer();
	}
}

void ofxFlashConnection::parseBuffer() {
	vector<char>::iterator it = buffer.begin();
	string line;
	int state = 0;
	while(it != buffer.end()) {
		char& c = *it;
		
		if(c == 0x00) {
			if(line == "<policy-file-request/>") {
				string policies = com->getPoliciesXML();
				write(policies);
			}
			else {
				//parseCommand(cmd, line);
			}
			it = buffer.erase(buffer.begin(), it);
		}
		else {
			line.push_back(c);
		}
		++it;
	}
}

void ofxFlashConnection::onShutdown(const AutoPtr<ShutdownNotification>& pNotif) {
	cout << "yep!" << endl;
}

int ofxFlashConnection::write(string sData) {
	//sData.push_back('\0');	
	int n = sData.size();
	int done = 0;
	while(n < done) {
		done+= socket.sendBytes(sData.c_str(),sData.size());
	}
	return done;
	
	
}