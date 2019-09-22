#include "syncDataClass.h"
using namespace std;

void syncDataClass::Print(DWORD gameTime) {
	syncDataArrayStruct* HereData;
	syncDataArrayStruct* AwayData;
	if( Here.GetSyncDataAddress(gameTime, &HereData ) || Away.GetSyncDataAddress(gameTime, &AwayData ) ) return;

    cout << "(" << dec << HereData->HPA << " - " << HereData->HPB << " , " << HereData->XA << " - " << HereData->XB << ") != "
         << "(" << AwayData->HPA << " - " << AwayData->HPB << " , " << AwayData->XA << " - " << AwayData->XB << ")" << endl;
}

int syncDataClass::TestSyncData(DWORD gameTime ){
	if( gameTime%20 ) return 3;

	syncDataArrayStruct* HereData;
	syncDataArrayStruct* AwayData;
	if( Here.GetSyncDataAddress(gameTime, &HereData ) || Away.GetSyncDataAddress(gameTime, &AwayData ) ) return 2;

	if( !( HereData->HPA && AwayData->HPA && HereData->HPB && AwayData->HPB && HereData->XA && AwayData->XA && HereData->XB && AwayData->XB ) ) return 1;

	if ( abs(AwayData->HPA - HereData->HPA) > 4 ) return 0xF;
	if ( abs(AwayData->HPB - HereData->HPB) > 4 ) return 0xF;
	if ( HereData->XA != AwayData->XA ) return 0xF;
	if ( HereData->XB != AwayData->XB ) return 0xF;

	return 0;
}

//Here
void syncDataClass::SetSyncDataHere( DWORD gameTime, BYTE playerSide, DWORD HP, DWORD X ){

	if( playerSide == 0xA ){
		Here.SetSyncDataA( gameTime, HP, X );

		Here.SetSyncDataA( gameTime + 400, 0, 0 );
		Away.SetSyncDataA( gameTime + 400, 0, 0 );
	}else if( playerSide == 0xB ){
		Here.SetSyncDataB( gameTime, HP, X );

		Here.SetSyncDataB( gameTime + 400, 0, 0 );
		Away.SetSyncDataB( gameTime + 400, 0, 0 );
	}
}

void syncDataClass::SetSyncDataHereA( DWORD gameTime, DWORD HP, DWORD X ){
	Here.SetSyncDataA( gameTime, HP, X );

	Here.SetSyncDataA( gameTime + 400, 0, 0 );
	Away.SetSyncDataA( gameTime + 400, 0, 0 );
}

void syncDataClass::SetSyncDataHereB( DWORD gameTime, DWORD HP, DWORD X ){
	Here.SetSyncDataB( gameTime, HP, X );

	Here.SetSyncDataB( gameTime + 400, 0, 0 );
	Away.SetSyncDataB( gameTime + 400, 0, 0 );
}

//Away
void syncDataClass::SetSyncDataAway( DWORD gameTime, BYTE playerSide, DWORD HP, DWORD X ){
	if( playerSide == 0xA ) Away.SetSyncDataA( gameTime, HP, X );
	if( playerSide == 0xB ) Away.SetSyncDataB( gameTime, HP, X );
}

void syncDataClass::SetSyncDataAwayA( DWORD gameTime, DWORD HP, DWORD X ){
	Away.SetSyncDataA( gameTime, HP, X );
}

void syncDataClass::SetSyncDataAwayB( DWORD gameTime, DWORD HP, DWORD X ){
	Away.SetSyncDataB( gameTime, HP, X );
}



int syncDataClass::init(){
	//init
	if( Here.init() || Away.init() ) return 1;

	return 0;
}
