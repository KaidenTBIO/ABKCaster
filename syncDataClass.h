#ifndef CASTER_SYNCDATA
#define CASTER_SYNCDATA

#if _MSC_VER >= 1300
#	if !defined(_WINDOWS_)
#		if !defined(NO_WIN32_LEAN_AND_MEAN)
#		define WIN32_LEAN_AND_MEAN
#		define  _VCL_LEAN_AND_MEAN
#		endif
#	endif
#	include <windows.h>
#	include <iostream>
#else
#	include <windows.h>
#	include <iostream.h>
#endif

#include "const.h"
#include "etc.h"

#define sync_cycle_size 64

typedef struct{
	DWORD HPA;
	DWORD HPB;
	DWORD XA;
	DWORD XB;
}syncDataArrayStruct;

class syncDataSubClass{
	private:
	syncDataArrayStruct syncDataArray[ sync_cycle_size ];

	public:
	void SetSyncDataA( DWORD, DWORD, DWORD );
	void SetSyncDataB( DWORD, DWORD, DWORD );
	int GetSyncDataAddress( DWORD, syncDataArrayStruct** );
	int init();
};


class syncDataClass{
	private:
	syncDataSubClass Here;
	syncDataSubClass Away;

	public:
	void Print( DWORD );

	void SetSyncDataHere( DWORD, BYTE, DWORD, DWORD );	//gameTime, playerSide, HP, x
	void SetSyncDataHereA( DWORD, DWORD, DWORD );
	void SetSyncDataHereB( DWORD, DWORD, DWORD );

	void SetSyncDataAway( DWORD, BYTE, DWORD, DWORD );	//gameTime, playerSide, HP, x
	void SetSyncDataAwayA( DWORD, DWORD, DWORD );
	void SetSyncDataAwayB( DWORD, DWORD, DWORD );

	int TestSyncData( DWORD );
	int init();
};


#endif
