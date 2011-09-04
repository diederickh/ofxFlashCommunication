#ifndef OFXFLASHPOLICYH
#define OFXFLASHPOLICYH

#include <string>
using std::string;

// Container for flash policies
// ----------------------------
struct ofxFlashPolicy {
	ofxFlashPolicy(string sDomain,string sPort)
	:domain(sDomain)
	,port(sPort)
	{
	}
						//		+"secure=\"false\" " 
	std::string getXML() {
			std::string xml = "<allow-access-from " \
								"domain=\"" +domain +"\" " \
								"secure=\"false\" " \
								"to-ports=\"" +port +"\" />";
		return xml;
	}
	string domain;
	string port;
};

#endif