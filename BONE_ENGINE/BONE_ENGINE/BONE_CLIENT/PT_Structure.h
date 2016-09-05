#pragma once
#include "PreHeader.h"

typedef struct _S_PT_REG_USER
{
	WCHAR USER_ID[32];
	WCHAR USER_NAME[32];
	DWORD AGE;
	BYTE SEX;
	WCHAR ADDRESS[32];
} S_PT_REG_USER;

typedef struct _S_PT_QUERY_USER
{
	WCHAR USER_ID[32];
} S_PT_QUERY_USER;

typedef struct _S_PT_QUERY_USER_RESULT
{
	WCHAR USER_ID[32];
	WCHAR USER_NAME[32];
	DWORD AGE;
	BYTE SEX;
	WCHAR ADDRESS[32];
} S_PT_QUERY_USER_RESULT;

typedef struct _S_PT_REG_COMPUTER
{
	WCHAR COMPUTER_NAME[32];
	WCHAR IP_ADDRESS[32];
	BYTE CPU_TYPE;
	DWORD RAM;
	DWORD HDD;
} S_PT_REG_COMPUTER;

typedef struct _S_PT_QUERY_COMPUTER
{
	WCHAR COMPUTER_NAME[32];
} S_PT_QUERY_COMPUTER;

typedef struct _S_PT_QUERY_COMPUTER_RESULT
{
	WCHAR COMPUTER_NAME[32];
	WCHAR IP_ADDRESS[32];
	BYTE CPU_TYPE;
	DWORD RAM;
	DWORD HDD;
} S_PT_QUERY_COMPUTER_RESULT;

typedef struct _S_PT_REG_PROGRAM
{
	WCHAR PROGRAM_NAME[32];
	DWORD COST;
	WCHAR COMMENT[32];
} S_PT_REG_PROGRAM;

typedef struct _S_PT_QUERY_PROGRAM
{
	WCHAR PROGRAM_NAME[32];
} S_PT_QUERY_PROGRAM;

typedef struct _S_PT_QUERY_PROGRAM_RESULT
{
	WCHAR PROGRAM_NAME[32];
	DWORD COST;
	WCHAR COMMENT[32];
} S_PT_QUERY_PROGRAM_RESULT;

