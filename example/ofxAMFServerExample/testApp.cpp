#include "testApp.h"

//--------------------------------------------------------------
void testApp::setup(){
	ofSetFrameRate(60);
	ofBackground(253,245,219);

	amf_server.addListener(this, &testApp::onAMFEvent);
	amf_server.setup("*", 2222);
	amf_server.start();
}

void testApp::onAMFEvent(ofxAMFEvent& ev) {
	cout << "Request uri:" << ev.getTargetURI() << endl;
	cout << "Params (JSON): " << ev.getParams().toJSON() << endl;
	cout << "Params (XML):" << endl << ev.getParams().toXML() << endl;
	last_amf_params = ev.getParams().toXML();
	cout << "Data from flash:" << (string)ev.getParams()["data"] << endl;
	
	Dictionary result;
	result["name"] = "Some result";
	ev.setReturnValues(result);

}

//--------------------------------------------------------------
void testApp::update(){
	ofSetWindowTitle(ofToString(ofGetFrameRate()));
}

//--------------------------------------------------------------
void testApp::draw(){
	ofSetColor(0,0,0);
	ofDrawBitmapString(last_amf_params, 30,30);
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