const char index_html_name[] PROGMEM = "/index.html";

const byte index_html[] PROGMEM = {
	0x3c,  0x68,  0x74,  0x6d,  0x6c,  0x3e,  0x3c,  0x62,  
	0x6f,  0x64,  0x79,  0x3e,  0x3c,  0x73,  0x63,  0x72,  
	0x69,  0x70,  0x74,  0x3e,  0x66,  0x75,  0x6e,  0x63,  
	0x74,  0x69,  0x6f,  0x6e,  0x20,  0x75,  0x70,  0x64,  
	0x61,  0x74,  0x65,  0x50,  0x61,  0x67,  0x65,  0x28,  
	0x29,  0x20,  0x7b,  0x76,  0x61,  0x72,  0x20,  0x78,  
	0x68,  0x74,  0x74,  0x70,  0x20,  0x3d,  0x20,  0x6e,  
	0x65,  0x77,  0x20,  0x58,  0x4d,  0x4c,  0x48,  0x74,  
	0x74,  0x70,  0x52,  0x65,  0x71,  0x75,  0x65,  0x73,  
	0x74,  0x28,  0x29,  0x3b,  0x78,  0x68,  0x74,  0x74,  
	0x70,  0x2e,  0x6f,  0x6e,  0x72,  0x65,  0x61,  0x64,  
	0x79,  0x73,  0x74,  0x61,  0x74,  0x65,  0x63,  0x68,  
	0x61,  0x6e,  0x67,  0x65,  0x20,  0x3d,  0x20,  0x66,  
	0x75,  0x6e,  0x63,  0x74,  0x69,  0x6f,  0x6e,  0x28,  
	0x29,  0x20,  0x7b,  0x69,  0x66,  0x20,  0x28,  0x74,  
	0x68,  0x69,  0x73,  0x2e,  0x72,  0x65,  0x61,  0x64,  
	0x79,  0x53,  0x74,  0x61,  0x74,  0x65,  0x20,  0x3d,  
	0x3d,  0x20,  0x34,  0x20,  0x26,  0x26,  0x20,  0x74,  
	0x68,  0x69,  0x73,  0x2e,  0x73,  0x74,  0x61,  0x74,  
	0x75,  0x73,  0x20,  0x3d,  0x3d,  0x20,  0x32,  0x30,  
	0x30,  0x29,  0x20,  0x7b,  0x64,  0x6f,  0x63,  0x75,  
	0x6d,  0x65,  0x6e,  0x74,  0x2e,  0x67,  0x65,  0x74,  
	0x45,  0x6c,  0x65,  0x6d,  0x65,  0x6e,  0x74,  0x42,  
	0x79,  0x49,  0x64,  0x28,  0x22,  0x75,  0x70,  0x74,  
	0x69,  0x6d,  0x65,  0x22,  0x29,  0x2e,  0x69,  0x6e,  
	0x6e,  0x65,  0x72,  0x48,  0x54,  0x4d,  0x4c,  0x20,  
	0x3d,  0x74,  0x68,  0x69,  0x73,  0x2e,  0x72,  0x65,  
	0x73,  0x70,  0x6f,  0x6e,  0x73,  0x65,  0x54,  0x65,  
	0x78,  0x74,  0x3b,  0x7d,  0x7d,  0x3b,  0x78,  0x68,  
	0x74,  0x74,  0x70,  0x2e,  0x6f,  0x70,  0x65,  0x6e,  
	0x28,  0x22,  0x47,  0x45,  0x54,  0x22,  0x2c,  0x20,  
	0x22,  0x75,  0x70,  0x74,  0x69,  0x6d,  0x65,  0x2e,  
	0x74,  0x78,  0x74,  0x22,  0x2c,  0x20,  0x74,  0x72,  
	0x75,  0x65,  0x29,  0x3b,  0x78,  0x68,  0x74,  0x74,  
	0x70,  0x2e,  0x73,  0x65,  0x6e,  0x64,  0x28,  0x29,  
	0x3b,  0x7d,  0x73,  0x65,  0x74,  0x49,  0x6e,  0x74,  
	0x65,  0x72,  0x76,  0x61,  0x6c,  0x28,  0x75,  0x70,  
	0x64,  0x61,  0x74,  0x65,  0x50,  0x61,  0x67,  0x65,  
	0x2c,  0x20,  0x31,  0x30,  0x30,  0x30,  0x29,  0x3b,  
	0x3c,  0x2f,  0x73,  0x63,  0x72,  0x69,  0x70,  0x74,  
	0x3e,  0x3c,  0x68,  0x32,  0x3e,  0x57,  0x65,  0x62,  
	0x62,  0x69,  0x6e,  0x6f,  0x20,  0x23,  0x57,  0x45,  
	0x42,  0x42,  0x49,  0x4e,  0x4f,  0x5f,  0x56,  0x45,  
	0x52,  0x23,  0x3c,  0x2f,  0x68,  0x32,  0x3e,  0x3c,  
	0x68,  0x33,  0x3e,  0x3c,  0x61,  0x20,  0x68,  0x72,  
	0x65,  0x66,  0x3d,  0x22,  0x68,  0x74,  0x74,  0x70,  
	0x73,  0x3a,  0x2f,  0x2f,  0x67,  0x69,  0x74,  0x68,  
	0x75,  0x62,  0x2e,  0x63,  0x6f,  0x6d,  0x2f,  0x53,  
	0x75,  0x6b,  0x6b,  0x6f,  0x50,  0x65,  0x72,  0x61,  
	0x2f,  0x57,  0x65,  0x62,  0x62,  0x69,  0x6e,  0x6f,  
	0x22,  0x3e,  0x68,  0x74,  0x74,  0x70,  0x73,  0x3a,  
	0x2f,  0x2f,  0x67,  0x69,  0x74,  0x68,  0x75,  0x62,  
	0x2e,  0x63,  0x6f,  0x6d,  0x2f,  0x53,  0x75,  0x6b,  
	0x6b,  0x6f,  0x50,  0x65,  0x72,  0x61,  0x2f,  0x57,  
	0x65,  0x62,  0x62,  0x69,  0x6e,  0x6f,  0x3c,  0x2f,  
	0x61,  0x3e,  0x3c,  0x2f,  0x68,  0x33,  0x3e,  0x3c,  
	0x68,  0x33,  0x3e,  0x43,  0x6f,  0x70,  0x79,  0x72,  
	0x69,  0x67,  0x68,  0x74,  0x20,  0x28,  0x43,  0x29,  
	0x20,  0x32,  0x30,  0x31,  0x32,  0x2d,  0x32,  0x30,  
	0x31,  0x39,  0x20,  0x42,  0x79,  0x20,  0x3c,  0x61,  
	0x20,  0x68,  0x72,  0x65,  0x66,  0x3d,  0x22,  0x6d,  
	0x61,  0x69,  0x6c,  0x74,  0x6f,  0x3a,  0x73,  0x6f,  
	0x66,  0x74,  0x77,  0x61,  0x72,  0x65,  0x40,  0x73,  
	0x75,  0x6b,  0x6b,  0x6f,  0x6c,  0x6f,  0x67,  0x79,  
	0x2e,  0x6e,  0x65,  0x74,  0x22,  0x3e,  0x53,  0x75,  
	0x6b,  0x6b,  0x6f,  0x50,  0x65,  0x72,  0x61,  0x3c,  
	0x2f,  0x61,  0x3e,  0x3c,  0x2f,  0x68,  0x33,  0x3e,  
	0x3c,  0x68,  0x34,  0x3e,  0x54,  0x68,  0x69,  0x73,  
	0x20,  0x70,  0x72,  0x6f,  0x67,  0x72,  0x61,  0x6d,  
	0x20,  0x63,  0x6f,  0x6d,  0x65,  0x73,  0x20,  0x77,  
	0x69,  0x74,  0x68,  0x20,  0x41,  0x42,  0x53,  0x4f,  
	0x4c,  0x55,  0x54,  0x45,  0x4c,  0x59,  0x20,  0x4e,  
	0x4f,  0x20,  0x57,  0x41,  0x52,  0x52,  0x41,  0x4e,  
	0x54,  0x59,  0x2e,  0x20,  0x54,  0x68,  0x69,  0x73,  
	0x20,  0x69,  0x73,  0x20,  0x66,  0x72,  0x65,  0x65,  
	0x20,  0x73,  0x6f,  0x66,  0x74,  0x77,  0x61,  0x72,  
	0x65,  0x2c,  0x20,  0x61,  0x6e,  0x64,  0x20,  0x79,  
	0x6f,  0x75,  0x20,  0x61,  0x72,  0x65,  0x20,  0x77,  
	0x65,  0x6c,  0x63,  0x6f,  0x6d,  0x65,  0x20,  0x74,  
	0x6f,  0x20,  0x72,  0x65,  0x64,  0x69,  0x73,  0x74,  
	0x72,  0x69,  0x62,  0x75,  0x74,  0x65,  0x20,  0x69,  
	0x74,  0x20,  0x75,  0x6e,  0x64,  0x65,  0x72,  0x20,  
	0x63,  0x65,  0x72,  0x74,  0x61,  0x69,  0x6e,  0x20,  
	0x63,  0x6f,  0x6e,  0x64,  0x69,  0x74,  0x69,  0x6f,  
	0x6e,  0x73,  0x3b,  0x20,  0x3c,  0x61,  0x20,  0x68,  
	0x72,  0x65,  0x66,  0x3d,  0x22,  0x68,  0x74,  0x74,  
	0x70,  0x3a,  0x2f,  0x2f,  0x77,  0x77,  0x77,  0x2e,  
	0x67,  0x6e,  0x75,  0x2e,  0x6f,  0x72,  0x67,  0x2f,  
	0x6c,  0x69,  0x63,  0x65,  0x6e,  0x73,  0x65,  0x73,  
	0x2f,  0x67,  0x70,  0x6c,  0x2d,  0x33,  0x2e,  0x30,  
	0x2e,  0x68,  0x74,  0x6d,  0x6c,  0x22,  0x3e,  0x63,  
	0x6c,  0x69,  0x63,  0x6b,  0x20,  0x68,  0x65,  0x72,  
	0x65,  0x3c,  0x2f,  0x61,  0x3e,  0x20,  0x66,  0x6f,  
	0x72,  0x20,  0x64,  0x65,  0x74,  0x61,  0x69,  0x6c,  
	0x73,  0x2e,  0x3c,  0x2f,  0x68,  0x34,  0x3e,  0x3c,  
	0x62,  0x72,  0x2f,  0x3e,  0x3c,  0x68,  0x35,  0x3e,  
	0x53,  0x79,  0x73,  0x74,  0x65,  0x6d,  0x20,  0x55,  
	0x70,  0x74,  0x69,  0x6d,  0x65,  0x3a,  0x20,  0x3c,  
	0x73,  0x70,  0x61,  0x6e,  0x20,  0x69,  0x64,  0x3d,  
	0x22,  0x75,  0x70,  0x74,  0x69,  0x6d,  0x65,  0x22,  
	0x2f,  0x3e,  0x3c,  0x2f,  0x68,  0x35,  0x3e,  0x3c,  
	0x2f,  0x62,  0x6f,  0x64,  0x79,  0x3e,  0x3c,  0x2f,  
	0x68,  0x74,  0x6d,  0x6c,  0x3e,  
};

const unsigned int index_html_len PROGMEM = 821;

const char uptime_txt_name[] PROGMEM = "/uptime.txt";

const byte uptime_txt[] PROGMEM = {
	0x23,  0x55,  0x50,  0x54,  0x49,  0x4d,  0x45,  0x23,  
	0x0a,  
};

const unsigned int uptime_txt_len PROGMEM = 9;

/*** CODE TO INCLUDE IN SKETCH ***

const Page page01 PROGMEM = {index_html_name, index_html, index_html_len, NULL};
const Page page02 PROGMEM = {uptime_txt_name, uptime_txt, uptime_txt_len, NULL};

const Page* const pages[] PROGMEM = {
	&page01,
	&page02,
	NULL
};

***/
