#include "ofxAMFSerializer.h"
#include "ofMain.h" // only for debugging
ofxAMFSerializer::ofxAMFSerializer() {
}

ofxAMFSerializer::~ofxAMFSerializer() {
	
}

bool ofxAMFSerializer::read(IOBuffer& buffer, Dictionary& dict) {
	return true;
}

bool ofxAMFSerializer::readUndefined(IOBuffer& buffer, Dictionary& dict) {
	dict.reset();
	return true;
}

bool ofxAMFSerializer::readNull(IOBuffer& buffer, Dictionary& dict){
	dict.reset();
	return true;
}

bool ofxAMFSerializer::readFalse(IOBuffer& buffer, Dictionary& dict) {
	dict = (bool)false;
	return true;

}

bool ofxAMFSerializer::readTrue(IOBuffer& buffer, Dictionary& dict) {	
	dict = (bool)true;
	return true;
}

bool ofxAMFSerializer::readInteger(IOBuffer& buffer, Dictionary& dict) {
	return true;
}


bool ofxAMFSerializer::readDouble(IOBuffer& buffer, Dictionary& dict) {
	return true;
}


bool ofxAMFSerializer::readString(IOBuffer& buffer, Dictionary& dict) {
	return true;
}

bool ofxAMFSerializer::readArray(IOBuffer& buffer, Dictionary& dict) {
	return true;
}


bool ofxAMFSerializer::readObject(IOBuffer& buffer, Dictionary& dict) {
	return true;
}


bool ofxAMFSerializer::readByteArray(IOBuffer& buffer, Dictionary& dict) {
	return true;
}


bool ofxAMFSerializer::readByte(IOBuffer& buffer, Dictionary& dict) {
	uint8_t b;
	buffer.consumeByte(b);
	dict = b;
	return true;
}

bool ofxAMFSerializer::readUint32(IOBuffer& buffer, Dictionary& dict) {	
	uint32_t u;
	buffer.consumeUint32(u);
	u = ntohlex(u);
	dict = u;
	return true;
}

bool ofxAMFSerializer::readUTF(IOBuffer& buffer, Dictionary& dict) {
	// utf precedes with a 16bit length.
	Dictionary utf_length;
	readUint16(buffer, utf_length);
		
	// when we found a function copy the name
	if((uint16_t)utf_length > 0) {
		string utf_value;
		buffer.consumeString(utf_value, utf_length);
		dict = utf_value;
		return true;
	}
	return false;
}

bool ofxAMFSerializer::readUint16(IOBuffer& buffer, Dictionary& dict) {
	uint16_t buf;
	buffer.consumeUint16(buf);
	dict = (uint16_t)ntohsex(buf);
}

bool ofxAMFSerializer::readU29(IOBuffer& buffer, uint32_t& value) {
	value = 0;
	for(uint32_t i = 0; i < 4; ++i) {
		uint8_t byte;
		buffer.consumeByte(byte);
		if(i != 3) {
			value = (value << 7) | (byte & 0x7f);
		}
		else {
			value = (value << 8) | byte;
		}
		if((byte & 0x80) == 0) {
			break;
		}
	}
	return true;
}