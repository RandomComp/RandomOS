#ifndef _TYPES_H
#define _TYPES_H

#define true 1

#define false 0

#define __far

#define PI 3.14159265358979323846

#define PACKED __attribute__((__packed__))

#define ALIGNED4096 __attribute__((aligned(4096)))

typedef unsigned char uint8;

typedef unsigned short uint16;

typedef unsigned int uint32;

typedef unsigned long long uint64;

typedef char int8;

typedef short int16;

typedef int int32;

typedef long long int64;

typedef uint8 bool;

union mem {
	uint8 byte;

	uint16 word;

	uint32 dword;
		
	uint64 qword;
};

#endif