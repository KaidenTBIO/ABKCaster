#include "inputDataClass.h"
using namespace std;


int inputDataSubClass::init(){
	if( !data ) return 1;
	memset( &dataInfo, 0, sizeof( gameInfoStruct ) );

	//�O�̂���
	Sleep(100);

	memset( data, 0xFF, inputBuf_size * 2 );
	return 0;
}

inputDataSubClass::inputDataSubClass(){
	memset( &dataInfo, 0, sizeof(dataInfo) );
	data = (WORD*)malloc( inputBuf_size * 2);
}

inputDataSubClass::~inputDataSubClass(){
	if( data ){
		free( data );
		data = NULL;
	}
}

//void *memcpy(void *dest, void *src, size_t size);
int inputDataSubClass::SetInputDataArea( DWORD Time, WORD* Address, WORD Size ){
	//�v����
	if( Time >= inputBuf_size ) return 1;
	if( (ULONGLONG)Time + (ULONGLONG)Size >= inputBuf_size ) return 1;
	memcpy( &data[ Time ], Address, Size );
	return 0;
}
WORD* inputDataSubClass::GetInputDataAddress( DWORD Time, WORD Size ){
	//�v����
	if( Time >= inputBuf_size ) return 0;
	if( (ULONGLONG)Time + (ULONGLONG)Size >= inputBuf_size ) return 0;
	if( data[ Time + Size ] == 0xFFFF ) return 0;

	return &data[ Time ];
}


//SetInput
//�b��
void inputDataSubClass::SetInputDataSub( DWORD Time, WORD Input ){
	if( Time < inputBuf_size ){
		if( data[ Time ] == 0xFFFF ){
			if( Input == 0xFFFF ) Input = 0;
			data[ Time ] = Input;
		}
	}
}
void inputDataSubClass::SetInputDataA( DWORD Time, WORD Input ){
	SetInputDataSub( Time, Input );
}

void inputDataSubClass::SetInputDataB( DWORD Time, WORD Input ){
	SetInputDataSub( Time + 1, Input );
}

//GetInput
//�b��
int inputDataSubClass::GetInputDataSub( DWORD Time, WORD* Input ){
	if( Time >= inputBuf_size ) return 1;
	if( data[ Time ] == 0xFFFF ) return 1;
	if( Input ) *Input = data[ Time ];
	return 0;
}

int inputDataSubClass::GetInputDataA( DWORD Time, WORD* Input ){
	if( GetInputDataSub( Time, Input ) ) return 1;
	return 0;
}

int inputDataSubClass::GetInputDataB( DWORD Time, WORD* Input ){
	if( GetInputDataSub( Time + 1, Input ) ) return 1;
	return 0;
}
