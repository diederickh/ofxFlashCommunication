#ifndef OFXAMFMESSAGEH
#define OFXAMFMESSAGEH

#include <inttypes.h>
#include <string>

using std::string;

class ofxAMFMessage {
public:
	ofxAMFMessage();
	void setReponseURI(string uri);
	void setTargetURI(string uri);
	void setMessageLength(uint32_t len);
	void setMessageType(uint8_t type);

	string getResponesURI();
	string getTargetURI();
	uint32_t getMessageLength();
	uint8_t getMessageType();

private:
	string response_uri;
	string target_uri;
	uint32_t message_length;
	uint8_t	message_type;
};
#endif