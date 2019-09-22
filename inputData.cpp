#include "inputDataClass.h"
using namespace std;

void inputDataClass::SetTime( WORD sessionNo, DWORD gameTime ){
	if( !sessionNo ) return;

	if( sessionNo == inputDataSub[0].dataInfo.sessionNo ){
		inputDataSub[0].dataInfo.gameTime = gameTime;

	}else if( sessionNo == inputDataSub[1].dataInfo.sessionNo ){
		inputDataSub[1].dataInfo.gameTime = gameTime;

	}else if( sessionNo == inputDataSub[2].dataInfo.sessionNo ){
		inputDataSub[2].dataInfo.gameTime = gameTime;
	}
}

DWORD inputDataClass::GetTime( WORD sessionNo ){
	if( !sessionNo ) return 0;

	if( sessionNo == inputDataSub[0].dataInfo.sessionNo ){
		return inputDataSub[0].dataInfo.gameTime;

	}else if( sessionNo == inputDataSub[1].dataInfo.sessionNo ){
		return inputDataSub[1].dataInfo.gameTime;

	}else if( sessionNo == inputDataSub[2].dataInfo.sessionNo ){
		return inputDataSub[2].dataInfo.gameTime;

	}
	return 0;
}

//SetInputData
int inputDataClass::SetInputDataArea( WORD sessionNo, DWORD Time, WORD* Address, WORD Size ){
	if( !sessionNo ) return 1;

	if( sessionNo == inputDataSub[0].dataInfo.sessionNo ){
		if( inputDataSub[0].SetInputDataArea( Time, Address, Size ) ) return 1;

	}else if( sessionNo == inputDataSub[1].dataInfo.sessionNo ){
		if( inputDataSub[1].SetInputDataArea( Time, Address, Size ) ) return 1;

	}else if( sessionNo == inputDataSub[2].dataInfo.sessionNo ){
		if( inputDataSub[2].SetInputDataArea( Time, Address, Size ) ) return 1;
	}else{
		return 1;
	}
	return 0;
}

void inputDataClass::SetInputData( WORD sessionNo, DWORD Time, WORD Side, WORD Input ){
	if( !sessionNo ) return;
	if( (DWORD)( Time / 2 ) * 2 != Time ) return;
	if( Side != 0xA && Side != 0xB ) return;
	if( Side == 0xB ) Time++;

	if( sessionNo == inputDataSub[0].dataInfo.sessionNo ){
		inputDataSub[0].SetInputDataSub( Time, Input);
	}else if( sessionNo == inputDataSub[1].dataInfo.sessionNo ){
		inputDataSub[1].SetInputDataSub( Time, Input );
	}else if( sessionNo == inputDataSub[2].dataInfo.sessionNo ){
		inputDataSub[2].SetInputDataSub( Time, Input );
	}
}

void inputDataClass::SetInputDataA( WORD sessionNo, DWORD Time, WORD Input ){
	if( !sessionNo ) return;
	if( (DWORD)( Time / 2 ) * 2 != Time ) return;

	if( sessionNo == inputDataSub[0].dataInfo.sessionNo ){
		inputDataSub[0].SetInputDataA( Time, Input );
	}else if( sessionNo == inputDataSub[1].dataInfo.sessionNo ){
		inputDataSub[1].SetInputDataA( Time, Input );
	}else if( sessionNo == inputDataSub[2].dataInfo.sessionNo ){
		inputDataSub[2].SetInputDataA( Time, Input );
	}
}

void inputDataClass::SetInputDataB( WORD sessionNo, DWORD Time, WORD Input ){
	if( !sessionNo ) return;
	if( (DWORD)( Time / 2 ) * 2 != Time ) return;

	if( sessionNo == inputDataSub[0].dataInfo.sessionNo ){
		inputDataSub[0].SetInputDataB( Time, Input );
	}else if( sessionNo == inputDataSub[1].dataInfo.sessionNo ){
		inputDataSub[1].SetInputDataB( Time, Input );
	}else if( sessionNo == inputDataSub[2].dataInfo.sessionNo ){
		inputDataSub[2].SetInputDataB( Time, Input );
	}
}

//GetInputData
WORD* inputDataClass::GetInputDataAddress( WORD sessionNo, DWORD Time, WORD Size ){
	if( !sessionNo ) return 0;
	if( sessionNo == inputDataSub[0].dataInfo.sessionNo ){
		return inputDataSub[0].GetInputDataAddress( Time, Size);

	}else if( sessionNo == inputDataSub[1].dataInfo.sessionNo ){
		return inputDataSub[1].GetInputDataAddress( Time, Size );

	}else if( sessionNo == inputDataSub[2].dataInfo.sessionNo ){
		return inputDataSub[2].GetInputDataAddress( Time, Size );
	}
	return 0;
}

int inputDataClass::GetInputData( WORD sessionNo, DWORD Time, WORD Side, WORD* Input){
	if( !sessionNo ) return 1;
	if( Side != 0xA && Side != 0xB ) return 1;
	if( Side == 0xB ) Time++;

	if( sessionNo == inputDataSub[0].dataInfo.sessionNo ){
		if( inputDataSub[0].GetInputDataSub( Time, Input ) ) return 1;
	}else if( sessionNo == inputDataSub[1].dataInfo.sessionNo ){
		if( inputDataSub[1].GetInputDataSub( Time, Input ) ) return 1;
	}else if( sessionNo == inputDataSub[2].dataInfo.sessionNo ){
		if( inputDataSub[2].GetInputDataSub( Time, Input ) ) return 1;
	}else{
		return 1;
	}
	return 0;
}
int inputDataClass::GetInputDataA( WORD sessionNo, DWORD Time, WORD* Input){
	if( !sessionNo ) return 1;
	if( sessionNo == inputDataSub[0].dataInfo.sessionNo ){
		if( inputDataSub[0].GetInputDataA( Time, Input ) ) return 1;
	}else if( sessionNo == inputDataSub[1].dataInfo.sessionNo ){
		if( inputDataSub[1].GetInputDataA( Time, Input ) ) return 1;
	}else if( sessionNo == inputDataSub[2].dataInfo.sessionNo ){
		if( inputDataSub[2].GetInputDataA( Time, Input ) ) return 1;
	}else{
		return 1;
	}
	return 0;
}
int inputDataClass::GetInputDataB( WORD sessionNo, DWORD Time, WORD* Input){
	if( !sessionNo ) return 1;
	if( sessionNo == inputDataSub[0].dataInfo.sessionNo ){
		if( inputDataSub[0].GetInputDataB( Time, Input ) ) return 1;

	}else if( sessionNo == inputDataSub[1].dataInfo.sessionNo ){
		if( inputDataSub[1].GetInputDataB( Time, Input ) ) return 1;

	}else if( sessionNo == inputDataSub[2].dataInfo.sessionNo ){
		if( inputDataSub[2].GetInputDataB( Time, Input ) ) return 1;
	}else{
		return 1;
	}
	return 0;
}

int inputDataClass::Start( gameInfoStruct* gameInfo ){
	if( !gameInfo ) return 1;

	//Indexのチェック
	if( !(prev == 0 || prev == 1 || prev == 2)
	 || !(now  == 0 || now  == 1 || now  == 2)
	 || !(next == 0 || next == 1 || next == 2) ){
		if( init() ) return 1;
	}

	//次のデータが使えるか
	if( inputDataSub[ next ].dataInfo.sessionNo != 0 && inputDataSub[ next ].dataInfo.sessionNo != gameInfo->sessionNo ){
		if( init() ) return 1;
	}


	if( inputDataSub[ next ].dataInfo.sessionNo == 0 ){
		//初回
		inputDataSub[ now ].dataInfo = *gameInfo;
		if( inputDataSub[ now ].dataInfo.sessionNo == 0xFF ){
			inputDataSub[ next ].dataInfo.sessionNo = 1;
		}else{
			inputDataSub[ next ].dataInfo.sessionNo = inputDataSub[ now ].dataInfo.sessionNo + 1;
		}
//		inputDataSub[ next ].dataInfo.sessionID = gameInfo->sessionIDNext;
	}else{
		//二回目以降
		inputDataSub[ prev ].init();


		WORD Temp = prev;
		prev = now;
		now = next;
		next = Temp;

		inputDataSub[ now ].dataInfo = *gameInfo;
		if( inputDataSub[ now ].dataInfo.sessionNo == 0xFF ){
			inputDataSub[ next ].dataInfo.sessionNo = 1;
		}else{
			inputDataSub[ next ].dataInfo.sessionNo = inputDataSub[ now ].dataInfo.sessionNo + 1;
		}
	}

	//debug
//	cout << "debug : " << (WORD)inputDataSub[ now ].dataInfo.sessionNo << ", " << prev << " " << now << " " << next << endl;

	return 0;
}

int inputDataClass::init(){
	#if inputBuf_size
		//OK
	#else
		//BAD
		return 1;
	#endif

	if( inputDataSub[0].init() || inputDataSub[1].init() || inputDataSub[2].init() ) return 1;

	now  = 0;
	next = 1;
	prev = 2;

	return 0;
}
