#ifndef OFXFLASHLISTENERH
#define OFXFLASHLISTENERH

#include <string>

class ofxFlashConnection;
class ofxFlashListener {
public:
	virtual void onData(
		std::string sData
		,ofxFlashConnection* pConnection
	) = 0;
};
#endif