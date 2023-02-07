#pragma once
#include <Windows.h>

#define USER_EXIT			0

#define PPVOID(function)	&(PVOID&)(function)
#define SIZE(c_array)		(c_array == NULL) ? 0 : (sizeof(c_array)/ sizeof(c_array[0]))

typedef unsigned char	uint8;
typedef unsigned short	uint16;
typedef unsigned int	uint32;
typedef signed char		int8;
typedef signed short	int16;
typedef signed int		int32;


struct DETOUR_ITEM {
	PVOID * pOriginalFunction;
	PVOID	hookFunction;
	int *	oFunctionSignature;
	int		signatureLength;
	char *  targetFunctionName;
	char *  moduleName = NULL;
	bool    useSignature = true;
};
