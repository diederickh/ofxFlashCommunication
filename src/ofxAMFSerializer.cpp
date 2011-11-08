#include "ofxAMFSerializer.h"
#include "ofxAMFMessage.h"
#include "ofMain.h"

#include <bitset> // debuggin.

ofxAMFSerializer::ofxAMFSerializer() {
}

ofxAMFSerializer::~ofxAMFSerializer() {
}

// SERIALZING
//------------------------------------------------------------------------------
//------------------------------------------------------------------------------
IOBuffer ofxAMFSerializer::serialize(ofxAMFPacket& packet) {
	//! start with version, header-, message count etc..
	IOBuffer buffer;
	buffer.setup();
	
	// write: version
	buffer.storeBigEndianUI16(packet.getClientVersion());
	
	// write: headers
	uint16_t header_count = packet.getNumHeaders();
	buffer.storeBigEndianUI16(header_count);
	for(uint16_t i = 0; i < header_count; ++i) {
		ofLogError("ofxamfserializer: : @todo: implement serializing of headers.");
	}
	
	// write: message count
	uint16_t message_count = packet.getNumMessages();
	buffer.storeBigEndianUI16(message_count);
	

	for(uint16_t i = 0; i < message_count; ++i) {
		// write: target and response uri.
		ofxAMFMessage* message = packet.getMessage(i);

		// write:: new target and response
		writeUTF(buffer, message->getResponseURI() +"/onResult"); 
		writeUTF(buffer, "null");

		// write: the dictionary which hold the result
		Dictionary data = message->getReturnValues();

		if(packet.isAMF3()) {
			// tmp buffer will hold the actualy amf serialized dictionary.
			IOBuffer tmp_buffer;

			tmp_buffer.storeByte(AMF0_AMF3_OBJECT);  // 0x11
			writeAMF3Type(tmp_buffer, data);

					
			// append size/array info
			buffer.storeBigEndianUI32(tmp_buffer.getNumBytesStored());
				
			//! append the created AMF message
			buffer.storeBuffer(tmp_buffer);
												
		}
		else {
			ofLogWarning("ofxamfserializer: : you're using amf0 this is work in progress");
			writeType(buffer, data);
		}
	}
	return buffer;
}

// Serialize to a AMF3 type
// -------------------------
void ofxAMFSerializer::writeAMF3Type(IOBuffer& buffer, Dictionary& input) {
	//printf("ofxamfserializer: amf3 type: %02X\n",input.type);

	switch(input.type) {
		case D_UNDEFINED: {
			writeAMF3Undefined(buffer);
			break;
		}
		
		case D_NULL: {
			writeAMF3Null(buffer, input);
			break;
		}
		
		case D_BYTEARRAY: {
			writeAMF3ByteArray(buffer, input);
			break;
		}
		
		case D_BOOL: {
			if((bool)input) {
				writeAMF3True(buffer);
			}
			else {
				writeAMF3False(buffer);
			}
			break;
		};
		
		case D_MAP: {
			if(input.isArray()) {
				writeAMF3Array(buffer, input);
			}
			else {
				writeAMF3Object(buffer, input);
				
			}
			break;
		}
		
		case D_STRING: {
			string str = (string)input;
			writeAMF3String(buffer, str);
			break;
		}
		
		case D_INT8:
		case D_INT16:
		case D_INT32:
		case D_UINT8:
		case D_UINT16:
		case D_UINT32: {
			writeAMF3Integer(buffer, input);
			break;
		}
		
		case D_UINT64:
		case D_INT64:
		case D_DOUBLE: {
			writeAMF3Double(buffer, input);
			break;
		};
		
		default:{
			ofLogError("ofxamfserializer: write amf3: uhandled input type.");
			break;
		};
	}
}

// AMF3 - write UNDEFINED (0x00)
void ofxAMFSerializer::writeAMF3Undefined(IOBuffer& buffer) {
	buffer.storeByte(AMF3_UNDEFINED);
}

// AMF3 -  Write NULL (0x01) // @todo why pass Dictionary& as well?
// --------------------------
void ofxAMFSerializer::writeAMF3Null(IOBuffer& buffer, Dictionary& source) {
	buffer.storeByte(AMF3_NULL);
}

// AMF3 -  Write FALSE (0x02)
// ---------------------------
void ofxAMFSerializer::writeAMF3False(IOBuffer& buffer) {
	buffer.storeByte(AMF3_FALSE);
}

// AMF3 -  Write TRUE (0x03)
// --------------------------
void ofxAMFSerializer::writeAMF3True(IOBuffer& buffer) {
	buffer.storeByte(AMF3_TRUE);
}

// AMF3 -  Write INTEGER (0x04)
// --------------------------
void ofxAMFSerializer::writeAMF3Integer(IOBuffer& buffer, Dictionary& source) {
	buffer.storeByte(AMF3_INTEGER);
	writeU29(buffer, (uint32_t)source);
}

// AMF3 -  Write DOUBLE (0x05)
// ---------------------------
void ofxAMFSerializer::writeAMF3Double(IOBuffer& buffer, Dictionary& source) {
	buffer.storeByte(AMF3_DOUBLE);
	buffer.storeBigEndianDouble((double)source);
}

// AMF3 -  Write STRING (0x06)
// ---------------------------
void ofxAMFSerializer::writeAMF3String(IOBuffer& buffer, string& source, bool writeType) {
	if(writeType) {
		buffer.storeByte(AMF3_STRING);
	}
	uint16_t length = (uint16_t)source.length();
	writeU29(buffer, (length << 1) | 0x01 ); 
	buffer.storeString(source);
}

// AMF3 - Write ARRAY (0x09)
// ---------------------------
void ofxAMFSerializer::writeAMF3Array(IOBuffer& buffer, Dictionary& source) {
	// to write a AMF3 array we follow this approach: (see amf3 spec page 8/9
	// ----------------------------------------------------
	// - 1. first write AMF3 array-value byte (0x09)
	// - 2. then we store the dense size portion (and reference) in a U29
	// - 3. then a name=value pair array (for which we remove the dense portion 
	// 		of 'source'
	// - 4. then an empty string to indicate end of name=value pairs
	// - 5. then we store the dense portion (see this as the indexed array part)


	// 1. 
	buffer.storeByte(AMF3_ARRAY); // 0x09
	
	// 2. 
	Dictionary s = source;
	uint32_t dense_size = s.getMapDenseSize();
	
	writeU29(buffer, (dense_size << 1) | 0x01); // LSB supposed to be 1

	
	// 3. 
	for(uint32_t i = 0; i < dense_size; ++i) {
		s.removeAt(i);	
	}
	map<string,Dictionary>::iterator it = s.begin();
	map<string,Dictionary>::iterator it_end = s.end();
	while(it != it_end) {
		string key = it->first;
		writeAMF3String(buffer, key);
		writeAMF3Type(buffer, it->second);
		++it;
	}
	
	// 4. 
	string empty_key = "";
	writeAMF3String(buffer, empty_key, false);
	
	// 5. 
	for(uint32_t i = 0; i < dense_size; ++i) {
		Dictionary dense_dict = source[i];
		writeAMF3Type(buffer, dense_dict);
	}
}


// AMF3 -  Write OBJECT (0x0A)
// ----------------------------
void ofxAMFSerializer::writeAMF3Object(IOBuffer& buffer, Dictionary& source) {
	buffer.storeByte(AMF3_OBJECT);
	uint32_t traits_count = 0x0b;
	if(!writeU29(buffer, traits_count)) {
		ofLogError("ofxamfserialzier: write amf3 object, cannot write traits count");
		return;
	}
	
	string class_name = "";
	writeAMF3String(buffer, class_name, false);
	
	map<string,Dictionary>::iterator it = source.begin();
	map<string,Dictionary>::iterator it_end = source.end();
	while(it != it_end) {

		string keyname = it->first;
		writeAMF3String(buffer, keyname,false);
		writeAMF3Type(buffer, it->second);
		++it;
	}
	
	writeAMF3String(buffer, class_name,false);
}

// AMF3 - Byte array
// ------------------
void ofxAMFSerializer::writeAMF3ByteArray(IOBuffer& buffer, Dictionary& source) {
	buffer.storeByte(AMF3_BYTEARRAY);
	string tmp = (string)source;
	uint32_t byte_size = (tmp.length() << 1) | 0x01;
	if(!writeU29(buffer, byte_size)) {
		ofLogError("Error while storing byte array size");
		return;
	}
	buffer.storeString(tmp);
}


// AMF0 - Write UTF string 
// -----------------------
void ofxAMFSerializer::writeUTF(IOBuffer& buffer, string value) {
	uint16_t length = (uint16_t)value.length();
	buffer.storeBigEndianUI16(length);
	buffer.storeString(value);
}

// AMF0 - Write a generic type 
// ----------------------------
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
		case D_BOOL: {
			
		}	
		case D_STRING: {
			
			break;
		}
		
		default:{
			printf("Unhandled input type.\n");
			break;
		};
	}
}

void ofxAMFSerializer::writeNull(IOBuffer& buffer, Dictionary& source) {
}

void ofxAMFSerializer::writeArray(IOBuffer& buffer, Dictionary& source) {
	
}

// writes uint32_t as 1,2,3 or 4 bytes in big endian format.
bool ofxAMFSerializer::writeU29(IOBuffer& buffer, uint32_t value) {
	uint32_t temp = ToBE32(value); 
	uint8_t* temp_ptr = (uint8_t*)&temp;

	if ((0x00000000 <= value) && (value <= 0x0000007f)) {
		buffer.storeUI8(temp_ptr[3]);
		return true;
	}
	else if ((0x00000080 <= value) && (value <= 0x00003fff)) {
		buffer.storeUI8(((temp_ptr[2] << 1) | (temp_ptr[3] >> 7)) | 0x80);
		buffer.storeUI8(temp_ptr[3]&0x7f);
		return true;
	} else if ((0x00004000 <= value) && (value <= 0x001fffff)) {
		buffer.storeUI8(((temp_ptr[1] << 2) | (temp_ptr[2] >> 6)) | 0x80);
		buffer.storeUI8(((temp_ptr[2] << 1) | (temp_ptr[3] >> 7)) | 0x80);
		buffer.storeUI8(temp_ptr[3]&0x7f);
		return true;
	} else if ((0x0020000 <= value) && (value <= 0x01fffffff)) {
		buffer.storeUI8(((temp_ptr[0] << 2) | (temp_ptr[1] >> 6)) | 0x80);
		buffer.storeUI8(((temp_ptr[1] << 1) | (temp_ptr[2] >> 7)) | 0x80);
		buffer.storeUI8(temp_ptr[2] | 0x80);
		buffer.storeUI8(temp_ptr[3]);
		return true;
	}
	return false;
}

// DE-SERIALZING
//------------------------------------------------------------------------------
//------------------------------------------------------------------------------
ofxAMFPacket ofxAMFSerializer::deserialize(IOBuffer& buffer) {
	ofxAMFPacket packet;
	//buffer.printHex();
	
	// get version.
	uint16_t version = buffer.consumeBigEndianUI16();
	packet.setClientVersion(version);
	
	// get headers count.
	uint16_t header_count = buffer.consumeBigEndianUI16();
	
	// parsing headers
	for(uint16_t i = 0; i < header_count; ++i) {
		ofLogError("@todo implement header parsing");
	}
	
	uint16_t message_count = buffer.consumeBigEndianUI16();
	
	string target_uri = "";
	string response_uri = "";
	for(uint16_t i = 0; i < message_count; ++i) {
		if(!readUTF(buffer, target_uri) || !readUTF(buffer, response_uri)) {
			ofLogError("error while parsing target and/or response uri");
			continue;
		}

		ofxAMFMessage* message = new ofxAMFMessage();
		message->setTargetURI(target_uri);
		message->setResponseURI(response_uri);
		packet.addMessage(message);	
		
		uint32_t message_length = buffer.consumeBigEndianUI32();
		message->setMessageLength(message_length);
		
		uint8_t value_type = buffer.consumeUI8();
		Dictionary result = readType(buffer, value_type);
		
		/*
		cout << "raw result:" << endl;
		cout << "--------------------------" << endl;
		cout << result.toJSON() << endl;
		cout << "--------------------------" << endl;
		cout << endl;
		*/
		
		// @todo this needs some serious testing... I need to create a 
		// flash app which sends all kinds of different params
		message->setParams(result[(uint32_t)0]);
	}
	return packet;
}


Dictionary ofxAMFSerializer::readType(IOBuffer& buffer, int type) {
	//printf("ofxamfserializer: read amf0 type: %02X\n", type);

	Dictionary result;
	switch(type) {
		case AMF0_AMF3_OBJECT: {
			return readAMF3Type(buffer);
			break;
		}
		
		case AMF0_NUMBER: {
			return readNumber(buffer);
			break;
		}
		case AMF0_BOOLEAN: {
			ofLogError("ofxamfserializer: @todo implement read AMF0_BOOLEAN");
			break;
		}
		case AMF0_SHORT_STRING: {
			result = readString(buffer);	
			break;
		}
		case AMF0_OBJECT: {
			ofLogError("ofxamfserializer: @todo implement read AFM0_OBJECT");
			break;
		}
		case AMF0_NULL: {
			ofLogError("ofxamfserializer: @todo implement read AFM0_NULL");
			break;
		}
		case AMF0_UNDEFINED: {
			ofLogError("ofxamfserializer: @todo implement read AFM0_UNDEFINED");
			break;
		}
		case AMF0_MIXED_ARRAY: {
			ofLogError("ofxamfserializer: @todo implement read AFM0_MIXED_ARRAY");
			break;
		}
		case AMF0_ARRAY: {
			result = readArray(buffer);
			break;
		}
		case AMF0_TIMESTAMP: {
			ofLogError("ofxamfserializer: @todo implement read AFM0_TIMESTAMP");
			break;
		}
		case AMF0_LONG_STRING: {
			ofLogError("ofxamfserializer: @todo implement read AFM0_LONG_STRING");
			break;
		}
		case AMF0_TYPED_OBJECT: {
			ofLogError("ofxamfserializer: @todo implement read AFM0_TYPED_OBJECT");
			break;
		}
		default: {
			ofLogError("ofxamfserializer: unhandled type for readType");
			result = false;
			break;
		}
	};
	return result;
}

Dictionary ofxAMFSerializer::readNumber(IOBuffer& buffer) {
	Dictionary result = (double)0.0;
	double d = buffer.consumeBigEndianDouble();
	result = (double)d;
	return result;
}


Dictionary ofxAMFSerializer::readAMF3Type(IOBuffer& buffer) {
	uint8_t type = buffer.consumeUI8();
	//printf("ofxamfserializer: read amf3 type: %02X\n", type);

	Dictionary result;
	switch(type) {
		case AMF3_UNDEFINED: {
			//result = readAMF3Undefined(buffer);
			break;
		}  
		case AMF3_NULL: {
			result = readAMF3Null(buffer);
			break;
		} 
		case AMF3_FALSE: {
			result = readAMF3False(buffer);
			break;
		} 
		case AMF3_TRUE: {
			result = readAMF3True(buffer);
			break;
		} 
		case AMF3_INTEGER: {
			result = readAMF3Integer(buffer);
			break;
		} 
		case AMF3_DOUBLE: {
			result = readAMF3Double(buffer);
			break;
		} 
		case AMF3_STRING: {
			result = readAMF3String(buffer);
			break;
		} 
		case AMF3_XMLDOC: {
			ofLogError("ofxamfserializer: @todo implement read AMF3_XMLDOC");
			break;
		} 
		case AMF3_DATE: {
			ofLogError("ofxamfserializer: @todo implement read AMF3_DATE");
			break;
		} 
		case AMF3_ARRAY: {
			result = readAMF3Array(buffer);
			break;
		} 
		case AMF3_OBJECT: {
			result = readAMF3Object(buffer);
			break;
		} 
		case AMF3_XML: {
			ofLogError("ofxamfserializer: @todo implement read AMF3_XML");	
			break;
		} 
		case AMF3_BYTEARRAY: {
			result = readAMF3ByteArray(buffer);
			break;
		} 
		default: {
			ofLogError("ofxamfserializer: unhandled or unknown AMF3 type");	
			result = false;
			break;
		}
	};
	//cout << "result:" << result.toString() << endl;
	return result;

}

Dictionary ofxAMFSerializer::readAMF3Undefined(IOBuffer& buffer) {
	Dictionary result;
	result.reset();
	return result;
}

Dictionary ofxAMFSerializer::readAMF3Null(IOBuffer& buffer) {
	Dictionary result;
	result.reset();
	return result;
}

Dictionary ofxAMFSerializer::readAMF3True(IOBuffer& buffer) {
	Dictionary result;
	result = (bool)true;
	return result;
}

Dictionary ofxAMFSerializer::readAMF3False(IOBuffer& buffer) {
	Dictionary result;
	result = (bool)false;
	return result;
}

Dictionary ofxAMFSerializer::readAMF3Integer(IOBuffer& buffer) {
	Dictionary result;
	uint32_t number = 0;
	if(!readU29(buffer, number)) {
		ofLogError("ofxamfserializer: error while reading amf3 integer");	
		result = (bool)0;
		return result;
	}
	result = (uint32_t)number;
	return result;
}


Dictionary ofxAMFSerializer::readAMF3Double(IOBuffer& buffer) {
	Dictionary result = (double)0.0;
	double d = buffer.consumeBigEndianDouble();
	result = (double)d;
	return result;
}

Dictionary ofxAMFSerializer::readAMF3ByteArray(IOBuffer& buffer) {
	uint32_t ref = 0;
	Dictionary result;
	if(!readU29(buffer, ref)) {
		result = (bool)false;
		return result;
	}
	// no reference
	if((ref & 0x01) == 0x01) {
		int num_bytes = ref >> 1;
		string bytes = buffer.consumeString(num_bytes);
		result = bytes;
		result.isByteArray(true);
		byte_arrays.push_back(result);
	}
	else {
		// return referenced...
		result = byte_arrays[ref >> 1];
	}
	return result;
}

Dictionary ofxAMFSerializer::readObject(IOBuffer& buffer) {
	//!implement
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

// Numeric keyed Actionscript Array
Dictionary ofxAMFSerializer::readArray(IOBuffer& buffer) {
	Dictionary result;
	uint32_t num_els = buffer.consumeBigEndianUI32();
	
	for(uint32_t i = 0; i < num_els; ++i) {
		uint8_t type = buffer.consumeUI8();
		Dictionary array_el = readType(buffer, type);
		result.pushToArray(array_el);
	}
	return result;
}


// 
// See AMFPHP which has a excellent, clear implementation.
// -------------------------------------------------------
Dictionary ofxAMFSerializer::readAMF3Object(IOBuffer& buffer) {
	Dictionary result;
	
	//get reference...
	uint32_t ref;
	if(!readU29(buffer, ref)) {
		result = (bool)false;
		ofLogError("ofxamfserializer: readamf3 object error reading reference.");
	}
	
	// check the kind of object we have (reference are "parented")
	bool fetch_object_by_reference = ((ref & 0x01) == 0);
	bool fetch_object_traits_by_reference = ((ref & 0x02) == 0);
	bool is_externalizable = ((ref & 0x07) == 0x07);
	bool is_dynamic = ((ref & 0x08) == 0x08); 
	int dx_object_reference = (ref >> 1);
	int dx_trait_reference = ref >> 2;
	uint32_t sealed_member_count = ref >> 4;
	Dictionary trait_definition = Dictionary();
	
	// already stored?
	if(fetch_object_by_reference) {
		return objects[dx_object_reference];
	}
	
	// externalizable object. this is custom serialized (and unknown data)
	if(is_externalizable) { // bits: 1 1 1
		Dictionary type_identifier = readAMF3String(buffer);
		ofLogError("Externalizable objects are no yet implemented.");
	}

	// object traits are passed referenced.
	if(fetch_object_traits_by_reference) {
		//bitset<29> bitset_ref(ref);
		trait_definition = traits[dx_trait_reference];
	}
	else {
		// store traits information 		
		Dictionary class_name = readAMF3String(buffer);
		trait_definition[AMF3_TRAITS_DYNAMIC] = (bool)is_dynamic;
		trait_definition[AMF3_TRAITS_CLASSNAME] = class_name;
		traits.push_back(trait_definition);
	
		// get sealed members.
		for(uint32_t i = 0; i < sealed_member_count; ++i) {
			ofLogError("ofxamfserializer: @todo read class sealed members");
		}
	}
	
	// we create and store a new object already as it can be referenced 
	// when parsing the dynamic properties.
	int new_object_index = objects.size();
	objects.push_back(result);
	
	bool read_dynamic_properties;
	if(fetch_object_traits_by_reference) {
		// did the object have dynamic fields.
		// @todo use "traits" member, trait[AMF3_TRAITS_DYNAMIC] to figure out if
		// the referneced object has dynamic fields.
		read_dynamic_properties = (bool)trait_definition[AMF3_TRAITS_DYNAMIC];
	}
	else {	
		read_dynamic_properties = is_dynamic;
	}
		
	// dynamic properties 
	if(read_dynamic_properties) {
		Dictionary key = readAMF3String(buffer);
		while ((string)key != "") {
			Dictionary value = readAMF3Type(buffer);
			result[(string)key] = value;
			key = readAMF3String(buffer);
		} 
		//!implement explicit types
	}
	objects[new_object_index] = result;
	return result;
}


Dictionary ofxAMFSerializer::readAMF3Array(IOBuffer& buffer) {
	Dictionary result;
	
	// check if we have a reference handle
	uint32_t ref;
	if(!readU29(buffer, ref)) {
		result = (bool)false;
		ofLogError("ofxamfserializer: read amf3 array - error reading u29\n");
		return result;
	}

	// did we get an array reference	
	if( (ref & 0x01) == 0) {
		result = objects[ref >> 1];
		return result;
	}
	
	// create and a empty result alreay. 
	int curr_array_dx = objects.size();
	objects.push_back(result);
		
	// read array
	while(true) {
		Dictionary key = readAMF3String(buffer);
		if((string)key == "") {
			break;
		}
		else {
			//!implement
			ofLogError("ofxamfserializer: read amf3 array - @todo unhandled portion in read array");
			break; // @todo implement
		}
	}
	
	// read dense portion
	uint32_t dense_size = ref >> 1;
	for(uint32_t i = 0; i < dense_size; ++i) {
		result[(uint32_t)i] = readAMF3Type(buffer);
	}
	result.isArray(true);
	
	// update the array.
	objects[curr_array_dx] = result;
	
	return result;
}

Dictionary ofxAMFSerializer::readAMF3String(IOBuffer& buffer) {
	Dictionary result;
	uint32_t ref;
	if(!readU29(buffer, ref)) {
		result = false;
		ofLogError("ofxamfserializer: read amf3 string, error reading size");
		return result;
	}
	
	// lsb == 0, get from reference table, lsb == 1, new string. (page 4, amf3 spec)
	if( (ref & 0x01) == 0) {
		result = strings[ref >> 1];
		return result;
	}
	else {
		uint32_t len = ref >> 1;
		if(len == 0) {
			result = "";
		}	
		else {
			result = buffer.consumeString(len);
			strings.push_back(result); 
		}
	}
	return result;
}

// Read next data as UTF string. (utf precedes with a 16bit length.)
bool ofxAMFSerializer::readUTF(IOBuffer& buffer, string& result) {
	uint16_t utf_length = buffer.consumeBigEndianUI16();
	if(utf_length > 0) {
		result = buffer.consumeString(utf_length);
		return true;
	}
	return false;
}

bool ofxAMFSerializer::readU29(IOBuffer& buffer, uint32_t& value) {
	value = 0;
	for(uint32_t i = 0; i < 4; ++i) {
		uint8_t byte = buffer.consumeUI8();
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