#ifndef CASTER_ETC
#define CASTER_ETC

#if _MSC_VER >1300
#	if !defined(_WINDOWS_)
#		if !defined(NO_WIN32_LEAN_AND_MEAN)
#		define WIN32_LEAN_AND_MEAN
#		define  _VCL_LEAN_AND_MEAN
#		endif
#	endif
#	include <windows.h>
#	include <process.h>
#	include <winsock2.h>
#else
#	include <windows.h>
#	include <process.h>
#	include <winsock2.h>
#endif

#include "const.h"

unsigned __stdcall boosterThread(void*);
unsigned __stdcall manageThread(void*);
unsigned __stdcall th075Thread(void*);
unsigned __stdcall recvThread(void*);
unsigned __stdcall sendThread(void*);

typedef struct{
	BYTE ID;
	BYTE phase;
	BYTE place;
	BYTE color;
}charInfoStruct;

typedef struct{
	charInfoStruct A;
	charInfoStruct B;
	BYTE phase;
	BYTE place;
	DWORD gameTime;
	BYTE playerSide;
	BYTE sessionID;
	BYTE sessionIDNext;
	BYTE sessionNo;
	BYTE terminalMode;
}gameInfoStruct;

typedef struct{
	DWORD Away;
	DWORD Root;
	DWORD Branch;
	DWORD subBranch;
	DWORD Leaf[4];
	DWORD Access;
	DWORD Standby[2];
}lastTimeStruct;

class sTaskClass{
	public:
	SOCKADDR_IN addr;
	int dest;
	int size;
	void* Address;
	BYTE data[ stask_buf_size ];
	WORD Flg;
	sTaskClass();
};

typedef struct{
	WORD Access;
	WORD Away;
	WORD Root;
	WORD Branch;
	WORD subBranch;
	WORD Leaf[4];
	WORD Other;
	WORD Ready;
}echoFlgStruct;

typedef struct{
	WORD argMode;
	char targetIP[80];
	WORD targetPort;
}argDataStruct;

#endif
