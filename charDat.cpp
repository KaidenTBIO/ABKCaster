//THIS FILE WAS SHAMELESSLY STOLEN (with some modifications) FROM CASTER

#include "charDatClass.h"
#include "const.h"
#include <shlwapi.h>
using namespace std;


charDatClass::charDatClass(){
	Button = &joyStatus.Button1;
	diFlg = 0;

	memset( inputBufBody, 0, sizeof( inputBufBody ) );
	memset( inputBufChar, 0, sizeof( inputBufChar ) );
	memset( &joyStatus, 0, sizeof( joyStatus ) );
	memset( keyStatus, 0, sizeof( keyStatus ) );

	storedCharInput = 0;
	storedBodyInput = 0;
	
	hProcess = 0;
	th075Flg = 0;
}

charDatClass::~charDatClass(){
	//none
}

void charDatClass::end(){
	if(diFlg){
		device->Unacquire();
		device->Release();
		di->Release();
		diFlg = 0;
	}
}

int charDatClass::init_int(int n) {
	if( !(*th075Flg) ) return 1;
	
	FILE *file = fopen("pad.cfg","rb");
	if (!file) {
		return 1;
	}
	
	fseek(file, 3 + (n*12*8), SEEK_SET);
	
	int input_block[12*2];
	fread(input_block, sizeof(input_block), 1, file);
	
	fclose(file);
	
	if (input_block[1] == 1 && !forceJoystick) {
		inputDeviceType = 0xff;
	} else {
		inputDeviceType = n;
		
		for (int i; i < 24; i += 2) {
			int j = 0;
			unsigned int k = input_block[i];
			if (input_block[i+1] == 1) {
				input_block[i] = 31;
				continue;
			}
			if (k == 0xffffffff) {
				input_block[i] = 31;
				continue;
			}
			
			k >>= 4;
			
			while (j < 32 && !(k&1)) {
				k >>= 1;
				++j;
			}
			if (j >= 32) {
				j = 31;
			}

			input_block[i] = j;
		}
	}
	
	keyIniRight	= input_block[ 0];
	keyIniLeft	= input_block[ 2];
	keyIniDown	= input_block[ 4];
	keyIniUp	= input_block[ 6];
	keyIniA		= input_block[ 8];
	keyIniB		= input_block[10];
	keyIniC		= input_block[12];
	keyIniStart	= input_block[14];
	keyIniAB	= input_block[18];
	keyIniBC	= input_block[20];
	keyIniABC	= input_block[22];

	if( diInit() ){
		diFlg = 0;
		return 1;
	}

	diFlg = 1;
	return 0;
}

int charDatClass::init2p(){
	return init_int(1);
}

int charDatClass::init(){
	return init_int(0);
}

int charDatClass::GetInput(){
	int Input = 0;

	if( !diFlg ) return 0;

	if(playerSide==0xA) {
		// Keyboard first
		if(inputDeviceType==0xFF) {
			if( device->GetDeviceState(256, keyStatus ) ){
				cout << "GetDeviceState Error" << endl;
			}

			if (keyStatus[keyIniUp] & 0x80) {
				Input |= key_up;
			}
			if (keyStatus[keyIniDown] & 0x80) {
				Input |= key_down;
			}
			if (keyStatus[keyIniLeft] & 0x80) {
				Input |= key_left;
			}
			if (keyStatus[keyIniRight] & 0x80) {
				Input |= key_right;
			}
			if (keyStatus[keyIniA] & 0x80) {
				Input |= key_A;
			}
			if (keyStatus[keyIniB] & 0x80) {
				Input |= key_B;
			}
			if (keyStatus[keyIniC] & 0x80) {
				Input |= key_C;
			}
			if (keyStatus[keyIniAB] & 0x80) {
				Input |= key_A | key_B;
			}
			if (keyStatus[keyIniBC] & 0x80) {
				Input |= key_B | key_C;
			}
			if (keyStatus[keyIniABC] & 0x80) {
				Input |= key_A | key_B | key_C;
			}
			
			/*
			static int pressed = 0;
			static int count = 0;

			if (playerSide == 0xA && (keyStatus[0x1E] & 0x80)) {
				if (!pressed) {
					pressed = 1;
					cout << "Blah" << endl;
					
					char *foobar;
					foobar = new char[0x100000];
					ReadProcessMemory(*hProcess, (void *)0xf00000, (void *)foobar, 0x100000, 0);
					
					char filename[256];
					sprintf(filename, "datadump2.%3.3d",count);
					count++;
					
					FILE *file = fopen(filename, "wb");
					if (file) {
						fwrite(foobar, 0x100000, 1, file);
						fclose(file);
					}
				} 
			} else {
				pressed = 0;
			}
			 */
		} else {
			//Now joypad

			if ( isPolledDevice )	device->Poll();
			if( device->GetDeviceState(80, &joyStatus) ){
				cout << "GetDeviceState Error" << endl;
			}
			
			if (joyStatus.X == 0 && joyStatus.Y == 0 && joyStatus.P1 != -1) {
				if (joyStatus.P1 <= povSensitivity || joyStatus.P1 >= 36000-povSensitivity) {
					joyStatus.Y = -1;
				} else if (joyStatus.P1 >= (18000-povSensitivity) && joyStatus.P1 <= (18000+povSensitivity)) {
					joyStatus.Y = 1;
				}
				if (joyStatus.P1 >= (9000-povSensitivity) && joyStatus.P1 <= (9000+povSensitivity)) {
					joyStatus.X = 1;
				} else if (joyStatus.P1 >= (27000-povSensitivity) && joyStatus.P1 <= (27000+povSensitivity)) {
					joyStatus.X = -1;
				}
			}
			
			if (joyStatus.Y < 0) {
				Input |= key_up;
			} else if (joyStatus.Y > 0) {
				Input |= key_down;
			}

			if (joyStatus.X < 0) {
				Input |= key_left;
			} else if (joyStatus.X > 0) {
				Input |= key_right;
			}

			if (Button[keyIniA] & 0x80) {
				Input |= key_A;
			}
			if (Button[keyIniB] & 0x80) {
				Input |= key_B;
			}
			if (Button[keyIniC] & 0x80) {
				Input |= key_C;
			}
			if (Button[keyIniAB] & 0x80) {
				Input |= key_A | key_B;
			}
			if (Button[keyIniBC] & 0x80) {
				Input |= key_B | key_C;
			}
			if (Button[keyIniABC] & 0x80) {
				Input |= key_A | key_B | key_C;
			}
		}
	} else {
				// Keyboard first
		if(inputDeviceType==0xFF) {
			if( device->GetDeviceState(256, keyStatus ) ){
				cout << "GetDeviceState Error" << endl;
			}
			
			if (keyStatus[keyIniUp] & 0x80) {
				Input |= key_up;
			}
			if (keyStatus[keyIniDown] & 0x80) {
				Input |= key_down;
			}
			if (keyStatus[keyIniLeft] & 0x80) {
				Input |= key_left;
			}
			if (keyStatus[keyIniRight] & 0x80) {
				Input |= key_right;
			}
			if (keyStatus[keyIniA] & 0x80) {
				Input |= key_A;
			}
			if (keyStatus[keyIniB] & 0x80) {
				Input |= key_B;
			}
			if (keyStatus[keyIniC] & 0x80) {
				Input |= key_C;
			}
			if (keyStatus[keyIniAB] & 0x80) {
				Input |= key_A | key_B;
			}
			if (keyStatus[keyIniBC] & 0x80) {
				Input |= key_B | key_C;
			}
			if (keyStatus[keyIniABC] & 0x80) {
				Input |= key_A | key_B | key_C;
			}
		} else {
			//Now joypad

			if ( isPolledDevice )	device->Poll();
			if( device->GetDeviceState(80, &joyStatus) ){
				cout << "GetDeviceState Error" << endl;
			}
			
			if (joyStatus.X == 0 && joyStatus.Y == 0 && joyStatus.P1 != -1) {
				if (joyStatus.P1 <= povSensitivity || joyStatus.P1 >= 36000-povSensitivity) {
					joyStatus.Y = -1;
				} else if (joyStatus.P1 >= (18000-povSensitivity) && joyStatus.P1 <= (18000+povSensitivity)) {
					joyStatus.Y = 1;
				}
				if (joyStatus.P1 >= (9000-povSensitivity) && joyStatus.P1 <= (9000+povSensitivity)) {
					joyStatus.X = 1;
				} else if (joyStatus.P1 >= (27000-povSensitivity) && joyStatus.P1 <= (27000+povSensitivity)) {
					joyStatus.X = -1;
				}
			}
			
			if (joyStatus.Y < 0) {
				Input |= key_up;
			} else if (joyStatus.Y > 0) {
				Input |= key_down;
			}

			if (joyStatus.X < 0) {
				Input |= key_left;
			} else if (joyStatus.X > 0) {
				Input |= key_right;
			}

			if (Button[keyIniA] & 0x80) {
				Input |= key_A;
			}
			if (Button[keyIniB] & 0x80) {
				Input |= key_B;
			}
			if (Button[keyIniC] & 0x80) {
				Input |= key_C;
			}
			if (Button[keyIniAB] & 0x80) {
				Input |= key_A | key_B;
			}
			if (Button[keyIniBC] & 0x80) {
				Input |= key_B | key_C;
			}
			if (Button[keyIniABC] & 0x80) {
				Input |= key_A | key_B | key_C;
			}
		}
	}

	return Input;
}

void charDatClass::writeBodyInput( ){
	if( !th075Flg || !(*th075Flg) ) return;
	if( !hProcess || !(*hProcess) ) return;
	
	if(playerSide==0xA){
		WriteProcessMemory(*hProcess,(void*)(memLoc_p1Input+0x14), &lastBodyInput, 4, NULL );
		WriteProcessMemory(*hProcess,(void*)(memLoc_p1Input), &storedBodyInput, 4, NULL );
	}else{
		WriteProcessMemory(*hProcess,(void*)(memLoc_p2Input+0x14), &lastBodyInput, 4, NULL );
		WriteProcessMemory(*hProcess,(void*)(memLoc_p2Input), &storedBodyInput, 4, NULL );
	}
}

void charDatClass::writeCharInput( ){
	if( !th075Flg || !(*th075Flg) ) return;
	if( !hProcess || !(*hProcess) ) return;
	
	if(playerSide==0xA){
		WriteProcessMemory(*hProcess,(void*)(memLoc_p1Input+0x14), &lastCharInput, 4, NULL );
		WriteProcessMemory(*hProcess,(void*)(memLoc_p1Input), &storedCharInput, 4, NULL );
	}else{
		WriteProcessMemory(*hProcess,(void*)(memLoc_p2Input+0x14), &lastCharInput, 4, NULL );
		WriteProcessMemory(*hProcess,(void*)(memLoc_p2Input), &storedCharInput, 4, NULL );
	}
}

int charDatClass::SetInput( int Input ){
	SetBodyInput( Input );
	SetCharInput( Input );

	return 0;
}

void charDatClass::storeBodyInput() {
	lastBodyInput = storedBodyInput;
}

void charDatClass::storeCharInput() {
	lastCharInput = storedCharInput;
}

int charDatClass::SetBodyInput( int Input ){
	//lastBodyInput = storedBodyInput;
	storedBodyInput = Input;
	
	return 0;
}

int charDatClass::SetCharInput( int Input ){
	//lastCharInput = storedCharInput;
	storedCharInput = Input;
	
	return 0;
}

