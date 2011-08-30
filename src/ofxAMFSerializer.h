#ifndef OFXAMFSERIALIZER
#define OFXAMFSERIALIZER

#include "Dictionary.h"
#include "IOBuffer.h"
//#include <arpa/inet.h>
#include "Endianness.h" // ofxMissing

// nice: http://www.acmewebworks.com/Downloads/openCS/TheAMF.pdf
// AFM 0: http://opensource.adobe.com/wiki/download/attachments/1114283/amf0_spec_121207.pdf

#define AMF3_UNDEFINED      0x00
#define AMF3_NULL           0x01
#define AMF3_FALSE          0x02
#define AMF3_TRUE           0x03
#define AMF3_INTEGER        0x04
#define AMF3_DOUBLE         0x05
#define AMF3_STRING         0x06
#define AMF3_XMLDOC         0x07
#define AMF3_DATE           0x08
#define AMF3_ARRAY          0x09
#define AMF3_OBJECT         0x0a
#define AMF3_XML            0x0b
#define AMF3_BYTEARRAY      0x0c

class ofxAMFSerializer {
private:
public:
	ofxAMFSerializer();
	virtual ~ofxAMFSerializer();
	
	bool read(IOBuffer& buffer, Dictionary& dict);
	
	bool readUndefined(IOBuffer& buffer, Dictionary& dict);
	bool readNull(IOBuffer& buffer, Dictionary& dict);
	bool readFalse(IOBuffer& buffer, Dictionary& dict);
	bool readTrue(IOBuffer& buffer, Dictionary& dict);
	bool readInteger(IOBuffer& buffer, Dictionary& dict);
	bool readDouble(IOBuffer& buffer, Dictionary& dict);
	bool readString(IOBuffer& buffer, Dictionary& dict);
	bool readArray(IOBuffer& buffer, Dictionary& dict);
	bool readObject(IOBuffer& buffer, Dictionary& dict);
	bool readByteArray(IOBuffer& buffer, Dictionary& dict);
	
	bool readByte(IOBuffer& buffer, Dictionary& dict);
	bool readUint16(IOBuffer& buffer, Dictionary& dict);
	bool readUint32(IOBuffer& buffer, Dictionary& dict);
	bool readUTF(IOBuffer& buffer, Dictionary& dict);
	
	static bool readU29(IOBuffer& buffer, uint32_t& value);
	
};
#endif