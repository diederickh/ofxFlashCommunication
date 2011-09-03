#include "ofxAMFSerializer.h"
#include "ofxAMFMessage.h"
#include "ofMain.h" // only for debugging

ofxAMFSerializer::ofxAMFSerializer() {
}

ofxAMFSerializer::~ofxAMFSerializer() {
	
}

// serializes a packet
IOBuffer ofxAMFSerializer::serialize(ofxAMFPacket& packet) {
	IOBuffer buffer;
	buffer.setup();
	
	// write version
	buffer.storeBigEndianUInt16(packet.getClientVersion());
	//buffer.storeBigEndianUInt16(0x00);
	
	// headers
	uint16_t header_count = packet.getNumHeaders();
	buffer.storeBigEndianUInt16(header_count);
	for(uint16_t i = 0; i < header_count; ++i) {
		printf("@todo implement serializing of headers.\n");
	}
	
	// message count
	uint16_t message_count = packet.getNumMessages();
	buffer.storeBigEndianUInt16(message_count);
	
	// messages
	for(uint16_t i = 0; i < message_count; ++i) {
		// write target and response uri.
		ofxAMFMessage* message = packet.getMessage(i);
		cout << "target uri: " << message->getTargetURI() << endl;

		// okay the next couple of lines are confusing and need some work.
		// when serializing a ofxAMFPacket for a AMF request from flash, we
		// need to swap the target<-->response. (we still need to do this! but
		// while testing I'm using the same values from the given ofxAMFPacket
		writeUTF(buffer, message->getResponseURI() +"/onResult"); 
		writeUTF(buffer, "null");
		
		// @TODO ALL DATA IS STORED IN A AMF0 ARRAY OF SIZE 1, THEN
		// A AMF3 ARRAY OF ARBITRARY SIZE, NEED TO FIGURE OUT WHY WE ALWAYS
		// GET A AMF0 ARRAY.
		
		// write the data
		Dictionary data = message->getData();
		Dictionary first_data = data[(uint32_t)0];
		string js;
		first_data.serializeToJSON(js);
		cout << "----------- packetize ----------" << endl;
		cout << js << endl;
		cout << "--------------------------------" << endl;
		cout << endl;


		if(packet.isAMF3()) {
			printf("store afm3\n");

			// we write the contents to a tmp buffer, so we can get the size
			// of it which we are supposed to add before the actually data.
			

			IOBuffer tmp_buffer;
			//tmp_buffer.storeByte(AMF0_ARRAY); // 0x0A 
			//tmp_buffer.storeBigEndianUInt32((uint32_t)0x01); 
			
			tmp_buffer.storeByte(AMF0_AMF3_OBJECT);  // 0x11
			writeAMF3Type(tmp_buffer, first_data);
			
			cout << "\n----------------- generated AMF MESSAGE  -----------------\n";
			tmp_buffer.printHex();
			cout << "\n----------------------------------------------------\n\n";
			
			
			// append size/array info
			cout << "Bytes in tmp bufer: " << tmp_buffer.getNumBytesStored() << endl;
			buffer.storeBigEndianUInt32(tmp_buffer.getNumBytesStored());
			
			// append the actual result/message
			buffer.storeBuffer(tmp_buffer);
												
			cout << "\n----------------- generated AMF (everything) -----------------\n";
			buffer.printHex();
			cout << "\n----------------------------------------------------\n\n";
		}
		else {
			printf("store amf0\n");
			writeType(buffer, data);
		}
		
	}
	
	return buffer;
}
/*
  $this->writeInt(0); //  write the version (always 0)
        $count = count($this->packet->headers);
        l('header count:' .$count);
       
		$count = count($this->packet->messages);
        $this->writeInt($count); // write the Message  count
        for ($i = 0; $i < $count; $i++) {
            $this->resetReferences();
            //write body.
            $message = $this->packet->messages[$i];
            $this->currentMessage = & $message;
            $this->writeUTF($message->targetUri);
            $this->writeUTF($message->responseUri);
            //save the current buffer, and flush it to write the Message
            $tempBuf = $this->outBuffer;
            $this->outBuffer = "";
            $this->writeData($message->data);
            $serializedMessage = $this->outBuffer;
            $this->outBuffer = $tempBuf;
            $this->writeLong(strlen($serializedMessage));
            $this->outBuffer .= $serializedMessage;
        }

*/

void ofxAMFSerializer::writeAMF3Object(IOBuffer& buffer, Dictionary& source) {
	printf("write amf3 object\n");	
	buffer.storeByte(AMF3_OBJECT);
	
	Dictionary s = source;
	uint32_t dense_size = s.getMapDenseSize();
	printf("dense size of array: %u\n", dense_size);
	
	// @todo figure out why this is done in the crtmp server.
	for(uint32_t i = 0; i < dense_size; ++i) {
		s.removeAt(i);	
	}
	
	writeU29(buffer, (dense_size << 1) | 0x01);

	//writeU29(buffer, 0x0b);	
}

void ofxAMFSerializer::writeAMF3Type(IOBuffer& buffer, Dictionary& input) {
	printf("amf3 type: %02X\n",input.type);
	switch(input.type) {
		case D_NULL: {

			break;
		}
		
		case D_MAP: {
			if(input.isArray()) {
				writeAMF3Array(buffer, input);
			}
			else {
				printf("@todo write amf3: write a object map\n");
			}
			break;
		}
		
		case D_STRING: {
			string str = (string)input;
			writeAMF3String(buffer, str);
			break;
		}
		
		default:{
			printf("write amf3: Unhandled input type.\n");
			break;
		};
	}
	
	/*
	 D_NULL		=	0
	,D_BOOL		= 	1
	
	,D_INT8		=	2
	,D_INT16	=	3
	,D_INT32	=	4
	,D_INT64	=	5
	
	,D_UINT8	=	6
	,D_UINT16	=	7
	,D_UINT32	=	8
	,D_UINT64	=	9
	
	,D_DOUBLE	= 	10
	,D_STRING	=	11
	,D_MAP		= 	12
	*/

}

void ofxAMFSerializer::writeAMF3Array(IOBuffer& buffer, Dictionary& source) {
	// to write a AMF3 array we follow this approach: (see amf3 spec page 8/9
	// ----------------------------------------------------
	// - 1. first write AMF3 array-value byte (0x09)
	// - 2. then we store the dense size portion (and reference) in a U29
	// - 3. then a name=value pair array (for which we remove the dense portion 
	// 		of 'source'
	// - 4. then an empty string to indicate end of name=value pairs
	// - 5. then we store the dense portion (see this as the indexed array part)


	// 1. amf3 array
	buffer.storeByte(AMF3_ARRAY); // 0x09
	
	// 2. dense size portion.
	Dictionary s = source;
	uint32_t dense_size = s.getMapDenseSize();
	
	writeU29(buffer, (dense_size << 1) | 0x01); // LSB supposed to be 1
	printf("dense size of array: %u\n", dense_size);
	
	// 3. remove dense part, so we only have "name=value" pairs left which we store
	for(uint32_t i = 0; i < dense_size; ++i) {
		s.removeAt(i);	
	}
	map<string,Dictionary>::iterator it = s.begin();
	map<string,Dictionary>::iterator it_end = s.end();
	while(it != it_end) {
		printf("@todo writeAMF3Array: check if this is correct!\n");
		string key = it->first;
		writeAMF3String(buffer, key);
		writeAMF3Type(buffer, it->second);
		++it;
	}
	
	// 4. indicate end of array
	string empty_key = "";
	writeAMF3String(buffer, empty_key, false);
	
	// 5. write dense portion. (note we use source which has all elements int
	// but because we access using the [](uint32_t) operator we only get the
	// dense part.
	for(uint32_t i = 0; i < dense_size; ++i) {
		Dictionary dense_dict = source[i];
		writeAMF3Type(buffer, dense_dict);
	}
}

void ofxAMFSerializer::writeUTF(IOBuffer& buffer, string value) {
	uint16_t length = (uint16_t)value.length();
	buffer.storeBigEndianUInt16(length);
	buffer.storeString(value);
}

void ofxAMFSerializer::writeType(IOBuffer& buffer, Dictionary& input) {
	switch(input.type) {
		case D_NULL: {
			writeNull(buffer, input);
			break;
		}
		
		case D_MAP: {
			if(input.isArray()) {
				writeArray(buffer, input);
			}
			else {
				printf("@todo write a object map\n");
			}
			break;
		}
		
		case D_STRING: {
			
			break;
		}
		
		default:{
			printf("Unhandled input type.\n");
			break;
		};
	}
	
	/*
	 D_NULL		=	0
	,D_BOOL		= 	1
	
	,D_INT8		=	2
	,D_INT16	=	3
	,D_INT32	=	4
	,D_INT64	=	5
	
	,D_UINT8	=	6
	,D_UINT16	=	7
	,D_UINT32	=	8
	,D_UINT64	=	9
	
	,D_DOUBLE	= 	10
	,D_STRING	=	11
	,D_MAP		= 	12
	*/
}

void ofxAMFSerializer::writeNull(IOBuffer& buffer, Dictionary& source) {
}

void ofxAMFSerializer::writeArray(IOBuffer& buffer, Dictionary& source) {
	
}

// writes uint32_t as 1,2,3 or 4 bytes in big endian format.
bool ofxAMFSerializer::writeU29(IOBuffer& buffer, uint32_t value) {
	uint32_t temp = htonlex(value); 
	uint8_t* temp_ptr = (uint8_t*)&temp;

	if ((0x00000000 <= value) && (value <= 0x0000007f)) {
		printf("U29 Given value: %u\n", value);
		printf("U29 Store : %02X\n", temp_ptr[3]);
		buffer.storeUInt8(temp_ptr[3]);
		return true;
	}
	else if ((0x00000080 <= value) && (value <= 0x00003fff)) {
		buffer.storeUInt8(((temp_ptr[2] << 1) | (temp_ptr[3] >> 7)) | 0x80);
		buffer.storeUInt8(temp_ptr[3]&0x7f);
		return true;
	} else if ((0x00004000 <= value) && (value <= 0x001fffff)) {
		buffer.storeUInt8(((temp_ptr[1] << 2) | (temp_ptr[2] >> 6)) | 0x80);
		buffer.storeUInt8(((temp_ptr[2] << 1) | (temp_ptr[3] >> 7)) | 0x80);
		buffer.storeUInt8(temp_ptr[3]&0x7f);
		return true;
	} else if ((0x0020000 <= value) && (value <= 0x01fffffff)) {
		buffer.storeUInt8(((temp_ptr[0] << 2) | (temp_ptr[1] >> 6)) | 0x80);
		buffer.storeUInt8(((temp_ptr[1] << 1) | (temp_ptr[2] >> 7)) | 0x80);
		buffer.storeUInt8(temp_ptr[2] | 0x80);
		buffer.storeUInt8(temp_ptr[3]);
		return true;
	}
	return false;
}

void ofxAMFSerializer::writeAMF3String(IOBuffer& buffer, string& source, bool writeType) {
	if(writeType) {
		buffer.storeByte(AMF3_STRING);
	}
	uint16_t length = (uint16_t)source.length();
	writeU29(buffer, (length << 1) | 0x01 ); 
	buffer.storeString(source);
}

void ofxAMFSerializer::writeUint8(IOBuffer& buffer, uint8_t value) {
	
}

// @todo pass Dictionary as input
ofxAMFPacket ofxAMFSerializer::deserialize(IOBuffer& buffer) {
	ofxAMFPacket packet;
	buffer.printHex();

	// get version.
	uint16_t version = buffer.consumeLittleEndianUInt16();
	packet.setClientVersion(version);
	
	// get headers count.
	uint16_t header_count = buffer.consumeLittleEndianUInt16();
	
	// parsing headers
	for(uint16_t i = 0; i < header_count; ++i) {
		printf("@todo implement header parsing\n");
	}
	
	uint16_t message_count = buffer.consumeLittleEndianUInt16();
	
	cout << "version: " << version << endl;
	cout << "header count: " << header_count << endl;
	cout << "message count: "<< message_count << endl;
	
	string target_uri = "";
	string response_uri = "";
	for(uint16_t i = 0; i < message_count; ++i) {
		if(!readUTF(buffer, target_uri) || !readUTF(buffer, response_uri)) {
			printf("error while parsing target and/or response uri");
			continue;
		}
		cout << "target uri: " << target_uri << endl;
		cout << "response uri: " << response_uri << endl;

		ofxAMFMessage* message = new ofxAMFMessage();
		message->setTargetURI(target_uri);
		message->setResponseURI(response_uri);
		packet.addMessage(message);	
		
		// body length (can be ignored): buffer.ignore(4)
		//buffer.ignore(4);
		uint32_t message_length = buffer.consumeLittleEndianUInt32();
		message->setMessageLength(message_length);
		
		uint8_t value_type = buffer.consumeUInt8();
		printf("MESSAGE VALUE TYPE: %02X\n", value_type);
		Dictionary result = readType(buffer, value_type);
		message->setData(result);
	
		// debug
		string json;
		if(result.serializeToJSON(json)) {
			cout << "---------------------json-------------\n";
			cout << json << endl;
			cout << "+++++++++++++++++++++++++++++++++++++++\n\n";
		}
		
		// tmp: here we create the response (temp);
		// the response should be formatted the same as the request, so 
		// again the request and response uris version, headers count,
		// message count etc..
		//IOBuffer output;
		//output.setup();
		//serialize(output, result);
	}
	return packet;
}


Dictionary ofxAMFSerializer::readType(IOBuffer& buffer, int type) {
	printf("readType: %02X\n", type);

	// @todo order by most frequest type
	Dictionary result;
	switch(type) {
		// AMF 0
		//------------------------------------------
		case AMF0_AMF3_OBJECT: {
			return readAMF3Type(buffer);
			break;
		}
		
		case AMF0_NUMBER: {
			break;
		}
		case AMF0_BOOLEAN: {
			break;
		}
		case AMF0_SHORT_STRING: {
			result = readString(buffer);	
			break;
		}
		case AMF0_OBJECT: {
			break;
		}
		case AMF0_NULL: {
			break;
		}
		case AMF0_UNDEFINED: {
			break;
		}
		case AMF0_MIXED_ARRAY: {
			break;
		}
		case AMF0_ARRAY: {
			result = readArray(buffer);
			break;
		}
		case AMF0_TIMESTAMP: {
			break;
		}
		case AMF0_LONG_STRING: {
			break;
		}
		case AMF0_TYPED_OBJECT: {
			break;
		}
		default: {
			printf("amf serializer, unknown type.\n");
			result = false;
			break;
		}
	};
	return result;
}

Dictionary ofxAMFSerializer::readAMF3Type(IOBuffer& buffer) {
	uint8_t type = buffer.consumeUInt8();
	printf("readType: %02X\n", type);

	// @todo order by most frequest type
	Dictionary result;
	switch(type) {
		// AMF 3 
		//------------------------------------------
		case AMF3_UNDEFINED: {
			break;
		}  
		case AMF3_NULL: {
			break;
		} 
		case AMF3_FALSE: {
			break;
		} 
		case AMF3_TRUE: {
			break;
		} 
		case AMF3_INTEGER: {
			break;
		} 
		case AMF3_DOUBLE: {
			break;
		} 
		case AMF3_STRING: {
			result = readAMF3String(buffer);
			break;
		} 
		case AMF3_XMLDOC: {
			break;
		} 
		case AMF3_DATE: {
			break;
		} 
		case AMF3_ARRAY: {
			result = readAMF3Array(buffer);
			break;
		} 
		case AMF3_OBJECT: {
			break;
		} 
		case AMF3_XML: {
			break;
		} 
		case AMF3_BYTEARRAY: {
			break;
		} 
		default: {
			printf("amf serializer, unknown type.\n");
			result = false;
			break;
		}
	};
	return result;

}

Dictionary ofxAMFSerializer::readObject(IOBuffer& buffer) {

}

Dictionary ofxAMFSerializer::readString(IOBuffer& buffer) {
	Dictionary result;
	string str;
	if(readUTF(buffer, str)) {
		result = str;
	}
	else {
		result = false;
	}
	return result;
}

// numeric keyed Actionscript Array
Dictionary ofxAMFSerializer::readArray(IOBuffer& buffer) {
	Dictionary result;
	uint32_t num_els = buffer.consumeLittleEndianUInt32();
	printf("Number of elements in array: %u in hex %04X\n",num_els);
	
	for(uint32_t i = 0; i < num_els; ++i) {
		uint8_t type = buffer.consumeUInt8();
		Dictionary array_el = readType(buffer, type);
		result.pushToArray(array_el);
	}
	return result;
}


Dictionary ofxAMFSerializer::readAMF3Array(IOBuffer& buffer) {
	Dictionary result;
	
	// check if we have a reference handle
	uint32_t ref;
	if(!readU29(buffer, ref)) {
		result = false;
		printf("error reading u29\n");
		return result;
	}

	// did we get an array reference	
	if( (ref & 0x01) == 0) {
		printf("We have a reference to an array: %u", ref >> 1); 
		printf("@todo handle array reference.\n");
		return result;
	}
	
	// read array
	while(true) {
		Dictionary key = readAMF3String(buffer);
		cout << "Got amf3 string: " << (string)key << endl;
		if((string)key == "") {
			break;
		}
		else {
			printf("@todo unhandled portion in read array ..!\n");
			break; // @todo implement
		}
	}
	
	// read dense portion
	uint32_t dense_size = ref >> 1;
	printf("dense size: %02X\n", dense_size);
	printf("ref: %02X\n", ref);
	for(uint32_t i = 0; i < dense_size; ++i) {
		result[(uint32_t)i] = readAMF3Type(buffer);
	}
	result.isArray(true);
	return result;
}

Dictionary ofxAMFSerializer::readAMF3String(IOBuffer& buffer) {
	Dictionary result;
	uint32_t ref;
	if(!readU29(buffer, ref)) {
		result = false;
		cout << "erro reading amf3 string." << endl;
		return result;
	}
	
	// did we get an reference.
	if( (ref & 0x01) == 0) {
		printf("@todo handle string reference\n");
		result = false;
		return result;
	}
	else {
		uint32_t len = ref >> 1;
		if(len == 0) {
			result = "";
		}	
		else {
			result = buffer.consumeString(len);
		}
	}
	return result;
}

// Read next data as UTF string. (utf precedes with a 16bit length.)
bool ofxAMFSerializer::readUTF(IOBuffer& buffer, string& result) {
	uint16_t utf_length = buffer.consumeLittleEndianUInt16();
	cout << "UTF length: " << utf_length << endl;	
	if(utf_length > 0) {
		result = buffer.consumeString(utf_length);
		return true;
	}
	return false;
}


bool ofxAMFSerializer::readU29(IOBuffer& buffer, uint32_t& value) {
	value = 0;
	for(uint32_t i = 0; i < 4; ++i) {
		uint8_t byte = buffer.consumeUInt8();
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
	cout << "readU29: " << value << endl;
	printf("readU29 HEX: %02X\n", value);
	return true;
}