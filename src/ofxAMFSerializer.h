#ifndef OFXAMFSERIALIZER
#define OFXAMFSERIALIZER

// Based on Gnash, CRTMP, AMFPHP, ZendPHP
// ----------------------------------------
// nice: http://www.acmewebworks.com/Downloads/openCS/TheAMF.pdf
// AFM 0: http://opensource.adobe.com/wiki/download/attachments/1114283/amf0_spec_121207.pdf
// good info on enveloppes: http://osflash.org/documentation/amf/envelopes/remoting
// also nice: https://github.com/timwhitlock/node-amf/blob/master/node-amf/serialize.js


#include <vector>
#include <string>

#include "ofxMissing.h"
#include "ofxAMFPacket.h"
#include "ofxAMFTypes.h"

using std::vector;
using std::string;
using namespace roxlu;

class ofxAMFSerializer {
public:
	ofxAMFSerializer();
	virtual ~ofxAMFSerializer();

	// Serialize & Deserialze
	ofxAMFPacket deserialize(IOBuffer& buffer);
	IOBuffer serialize(ofxAMFPacket& packet);

	// AMF3
	Dictionary readAMF3Type(IOBuffer& buffer);
	Dictionary readAMF3Array(IOBuffer& buffer);
	Dictionary readAMF3String(IOBuffer& buffer);
	Dictionary readAMF3Null(IOBuffer& buffer);
	Dictionary readAMF3True(IOBuffer& buffer);
	Dictionary readAMF3False(IOBuffer& buffer);
	Dictionary readAMF3Integer(IOBuffer& buffer);
	Dictionary readAMF3Double(IOBuffer& buffer);
	Dictionary readAMF3Object(IOBuffer& buffer);
	Dictionary readAMF3Undefined(IOBuffer& buffer);
	Dictionary readAMF3ByteArray(IOBuffer& buffer);

	void writeAMF3Type(IOBuffer& buffer, Dictionary& source);
	void writeAMF3Array(IOBuffer& buffer, Dictionary& source);
	void writeAMF3Object(IOBuffer& buffer, Dictionary& source);
	void writeAMF3String(IOBuffer& buffer, string& source, bool writeType = true);
	void writeAMF3Null(IOBuffer& buffer, Dictionary& source);
	void writeAMF3True(IOBuffer& buffer);
	void writeAMF3False(IOBuffer& buffer);
	void writeAMF3Double(IOBuffer& buffer, Dictionary& source);
	void writeAMF3Integer(IOBuffer& buffer, Dictionary& source);
	void writeAMF3Undefined(IOBuffer& buffer);
	void writeAMF3ByteArray(IOBuffer& buffer, Dictionary& source);

	// AMF 0
	Dictionary readType(IOBuffer& buffer, int type);
	Dictionary readObject(IOBuffer& buffer);
	Dictionary readString(IOBuffer& buffer);
	Dictionary readArray(IOBuffer& buffer);
	Dictionary readNumber(IOBuffer& buffer);

	void writeType(IOBuffer& buffer, Dictionary& source);
	void writeNull(IOBuffer& buffer, Dictionary& source);
	void writeArray(IOBuffer& buffer, Dictionary& source);
	void writeUTF(IOBuffer& buffer, string value);

	bool readU29(IOBuffer& buffer, uint32_t& value);
	bool writeU29(IOBuffer& buffer, uint32_t value);
	bool readUTF(IOBuffer& buffer, string& result);

private:
	// amf3 referenced values (partly implemented)
	vector<string> strings;
	vector<Dictionary> objects;
	vector<Dictionary> traits;
	vector<Dictionary> byte_arrays;
};



#endif
