#include "testApp.h"

//--------------------------------------------------------------
void testApp::setup(){
	ofSetFrameRate(60);

	// setup logger.
	ofxLog::setupUsingDefaultSettingsForFileRotation();
	ofxLog::setLevel(OF_LOG_VERBOSE);	
	display.setup(16);
	ofSetLogLevel(OF_LOG_NOTICE);	


	// testing some raw calls. Saved using WireShark (follow stream, save as raw)	
	// --------------------------------------------------------------------------
	IOBuffer fake_request;
	fake_request.loadFile(ofToDataPath("amf3_simple_object.raw",true)); // just sends 1.5 as double
	//fake_request.loadFile(ofToDataPath("amf3_request_as_array.raw",true));
	//fake_request.loadFile(ofToDataPath("amf3_request_as_object.raw",true)); // just sends 1.5 as double
	//fake_request.loadFile(ofToDataPath("amf3_one_param_as_array.raw",true)); // just sends 1.5 as double
	//fake_request.loadFile(ofToDataPath("amf3_request_only_number.raw",true)); // just sends 1.5 as double
	//fake_request.loadFile(ofToDataPath("amf3_request.raw",true));
	ofxAMFSerializer amf_serializer;
	ofxAMFHTTPRequest amf_http_request;
	IOBuffer amf_body;
	if(amf_http_request.parseHTTPRequest(fake_request, amf_body)) {
		amf_body.printHex();
		ofxAMFPacket amf_packet = amf_serializer.deserialize(amf_body);
		IOBuffer send_buffer = amf_serializer.serialize(amf_packet);
	}
	else {
		cout << "cannot parse http request" << endl;
	}
	

	// Create a amf server.
	amf_server.setup("localhost", 1234);
	amf_server.start();
	amf_server.addListener(this, &testApp::onAMFevent);
}

void testApp::onAMFevent(ofxAMFEvent& ev) {
	cout << "amf request for: " << ev.getTargetURI() << endl;
	Dictionary test;
	test["username"] = "roxlu";
	test["score"] = 10.5;
	test["email"] = "info@roxlu.com";
	test["id"] = 1;
	ev.setReturnValues(test);
}

//--------------------------------------------------------------
void testApp::update(){
	ofSetWindowTitle(ofToString(ofGetFrameRate()));
	
}

//--------------------------------------------------------------
void testApp::draw(){
	display.draw(15,13);
}

//--------------------------------------------------------------
void testApp::keyPressed(int key){

}

//--------------------------------------------------------------
void testApp::keyReleased(int key){

}

//--------------------------------------------------------------
void testApp::mouseMoved(int x, int y ){

}

//--------------------------------------------------------------
void testApp::mouseDragged(int x, int y, int button){

}

//--------------------------------------------------------------
void testApp::mousePressed(int x, int y, int button){

}

//--------------------------------------------------------------
void testApp::mouseReleased(int x, int y, int button){

}

//--------------------------------------------------------------
void testApp::windowResized(int w, int h){

}

//--------------------------------------------------------------
void testApp::gotMessage(ofMessage msg){

}

//--------------------------------------------------------------
void testApp::dragEvent(ofDragInfo dragInfo){ 

}