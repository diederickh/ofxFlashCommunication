#ifndef OFXAMFSERIALIZER
#define OFXAMFSERIALIZER

#include "Dictionary.h"
#include "IOBuffer.h"

#include "ofxAMFPacket.h"


//#include <arpa/inet.h>
#include "Endianness.h" // ofxMissing

// nice: http://www.acmewebworks.com/Downloads/openCS/TheAMF.pdf
// AFM 0: http://opensource.adobe.com/wiki/download/attachments/1114283/amf0_spec_121207.pdf
// good info on enveloppes: http://osflash.org/documentation/amf/envelopes/remoting
// also nice: https://github.com/timwhitlock/node-amf/blob/master/node-amf/serialize.js

#include "ofxAMFTypes.h"

class ofxAMFSerializer {
public:
	ofxAMFSerializer();
	virtual ~ofxAMFSerializer();
	ofxAMFPacket deserialize(IOBuffer& buffer);
	IOBuffer serialize(ofxAMFPacket& packet);
	
	

	// AMF0
	Dictionary readType(IOBuffer& buffer, int type);
	Dictionary readObject(IOBuffer& buffer);
	Dictionary readString(IOBuffer& buffer);
	Dictionary readArray(IOBuffer& buffer);
	
	// AMF3
	Dictionary readAMF3Type(IOBuffer& buffer);
	Dictionary readAMF3Array(IOBuffer& buffer);
	Dictionary readAMF3String(IOBuffer& buffer);
	
	bool readUTF(IOBuffer& buffer, string& result);
	
	
	void writeType(IOBuffer& buffer, Dictionary& source);
	void writeAMF3Type(IOBuffer& buffer, Dictionary& source);
	void writeAMF3Array(IOBuffer& buffer, Dictionary& source);
	void writeAMF3Object(IOBuffer& buffer, Dictionary& source);
	void writeAMF3String(IOBuffer& buffer, string& source, bool writeType = true);
		
	void writeNull(IOBuffer& buffer, Dictionary& source);
	void writeArray(IOBuffer& buffer, Dictionary& source);

	void writeUint8(IOBuffer& buffer, uint8_t value);
	
	void writeUTF(IOBuffer& buffer, string value);
	

	
	bool readU29(IOBuffer& buffer, uint32_t& value);
	bool writeU29(IOBuffer& buffer, uint32_t value);
	
private:

};



/*	

	
//	bool read(IOBuffer& buffer, Dictionary& dict);
//	
//	bool readUndefined(IOBuffer& buffer, Dictionary& dict);
//	bool readNull(IOBuffer& buffer, Dictionary& dict);
//	bool readFalse(IOBuffer& buffer, Dictionary& dict);
//	bool readTrue(IOBuffer& buffer, Dictionary& dict);
//	bool readInteger(IOBuffer& buffer, Dictionary& dict);
//	bool readDouble(IOBuffer& buffer, Dictionary& dict);
//	bool readString(IOBuffer& buffer, Dictionary& dict);
//	bool readArray(IOBuffer& buffer, Dictionary& dict);
//	bool readObject(IOBuffer& buffer, Dictionary& dict);
//	bool readByteArray(IOBuffer& buffer, Dictionary& dict);
//	
//	bool readByte(IOBuffer& buffer, Dictionary& dict);
//	bool readUint16(IOBuffer& buffer, Dictionary& dict);
//	bool readUint32(IOBuffer& buffer, Dictionary& dict);




			// read version
			Dictionary version;
			amf3.readUint16(buffer,version);
			
			// read number of headers.
			Dictionary header_count;
			amf3.readUint16(buffer,header_count);
			cout << "header count:" << (string)header_count << endl;
			
			// process headers
			for(int i = 0; i < (uint16_t)header_count; ++i) {
				cout << "TODO processing header:" << i << endl;
				
			}
			
			// read number of messages
			Dictionary message_count;
			amf3.readUint16(buffer,message_count);
			cout << "message count:" << (string)message_count << endl;
					
			// process messages			
			for(int i = 0; i < (uint16_t)message_count; ++i) {
				
				// get object to be called, ie. YourClass.SomeMethod
				Dictionary target_uri;
				amf3.readUTF(buffer, target_uri);
				cout << "target uri:" << (string)target_uri << endl;
				
				// aka: /1, /2, /3 (unique identifier)
				Dictionary response_uri;
				amf3.readUTF(buffer, response_uri);
				cout << "response uri:" << (string)response_uri << endl;
				
				// get message body length
				
//				Dictionary message_body_length;
//				amf3.readUint32(buffer, message_body_length);
//				cout << "message body length:" << (string)message_body_length << endl;
				
				buffer.ignore(4);
				
				// value type
				uint8_t marker;
				buffer.consumeByte(marker);
				printf("marker: %02X\n", marker);
				
				if(marker == 0x0a) {
					Dictionary num_elements;
					amf3.readUint32(buffer, num_elements);
					cout << "number of elements in array: " << (string)num_elements << endl;
					for(uint32_t i = 0; i < (uint32_t)num_elements; ++i) {
						uint8_t type;
						buffer.consumeByte(type);

						// string
						if(type == 0x02) {
							Dictionary str;
							amf3.readUTF(buffer, str);
							cout << "got string: '" << (string)str << "'"  << endl;
						}
					}
				}

				
			}
			
		
		*/




#endif