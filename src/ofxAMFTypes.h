#ifndef OFXAMFTYPESH
#define OFXAMFTYPESH

#define AMF0_VERSION		0x00
#define AMF3_VERSION		0x03

#define AMF0_NUMBER         0x00
#define AMF0_BOOLEAN        0x01
#define AMF0_SHORT_STRING   0x02
#define AMF0_OBJECT         0x03
#define AMF0_NULL           0x05
#define AMF0_UNDEFINED      0x06
#define AMF0_MIXED_ARRAY    0x08
#define AMF0_ARRAY          0x0a
#define AMF0_TIMESTAMP      0x0b
#define AMF0_LONG_STRING    0x0c
#define AMF0_TYPED_OBJECT	0x10
#define AMF0_AMF3_OBJECT    0x11

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

#define AMF3_TRAITS_DYNAMIC "____isDynamic____"
#define AMF3_TRAITS_DYNAMIC_LEN 17
#define AMF3_TRAITS "____traits____"
#define AMF3_TRAITS_LEN 14
#define AMF3_TRAITS_CLASSNAME "____class_name____"
#define AMF3_TRAITS_CLASSNAME_LEN 18
#endif