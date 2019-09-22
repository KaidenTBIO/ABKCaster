#include "syncDataClass.h"
using namespace std;


int syncDataSubClass::GetSyncDataAddress( DWORD gameTime, syncDataArrayStruct** Address ){
	if( gameTime%20 ) return 0x1;
	if( !Address ) return 1;
	gameTime = gameTime / 20;
	
	*Address = &syncDataArray[ gameTime%sync_cycle_size ];
	return 0;
}

void syncDataSubClass::SetSyncDataA( DWORD gameTime, DWORD HP, DWORD X ){
	if( gameTime%20 ) return;
	gameTime = gameTime / 20;
	
	syncDataArray[ gameTime%sync_cycle_size ].HPA = HP;
	syncDataArray[ gameTime%sync_cycle_size ].XA = X;
}

void syncDataSubClass::SetSyncDataB( DWORD gameTime, DWORD HP, DWORD X ){
	if( gameTime%20 ) return;
	gameTime = gameTime / 20;
	
	syncDataArray[ gameTime%sync_cycle_size ].HPB = HP;
	syncDataArray[ gameTime%sync_cycle_size ].XB = X;
}


int syncDataSubClass::init(){
	#if sync_cycle_size > 30
		//OK
	#else
		//BAD
		return 1;
	#endif
	
	memset( &syncDataArray[0], 0, sizeof( syncDataArray ) );
	
	
	return 0;
}
