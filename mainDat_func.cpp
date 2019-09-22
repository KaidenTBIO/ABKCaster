#include "mainDatClass.h"
#include <mmsystem.h>
#include <direct.h>
using namespace std;

#define rand_show 0

void mainDatClass::printDate(ostream *stream) {
    SYSTEMTIME time;
    if (systemTimeFlg) {
        GetSystemTime(&time);
    } else {
        GetLocalTime(&time);
    }
    *stream << dec << time.wYear << '/';

    if (time.wMonth < 10) {
        *stream << '0';
    }
    *stream << time.wMonth << '/';
    if (time.wDay < 10) {
        *stream << '0';
    }
    *stream << time.wDay;
}

void mainDatClass::printTime(ostream *stream) {
    SYSTEMTIME time;
    if (systemTimeFlg) {
        GetSystemTime(&time);
    } else {
        GetLocalTime(&time);
    }
    *stream << dec;
    if (time.wHour < 10) {
        *stream << '0';
    }
    *stream << time.wHour << ':';
    if (time.wMinute < 10) {
        *stream << '0';
    }
    *stream << time.wMinute << ':';
    if (time.wSecond < 10) {
        *stream << '0';
    }
    *stream << time.wSecond;
}

const char *mainDatClass::getCharacterName(int ch) {
	switch(ch&0xf) {
	case 0x0 : 	return "Akatsuki";
	case 0x1 : 	return "Mycale";
	case 0x2 : 	return "Sai";
	case 0x3 : 	return "Kanae";
	case 0x4 : 	return "Fritz";
	case 0x5 : 	return "Marilyn";
	case 0x6 : 	return "Wei";
	case 0x7 : 	return "Anonym";
	case 0x8 : 	return "E-Soldat";
	case 0x9 : 	return "Adler";
	case 0xA : 	return "Blitztank";
	case 0xB : 	return "Murakumo";
	default:	return "Unknown";
	}
}

const char *mainDatClass::getCharacterShortName(int ch) {
	switch(ch&0xf) {
	case 0x0 : 	return "Aka";
	case 0x1 : 	return "Myc";
	case 0x2 : 	return "Sai";
	case 0x3 : 	return "Kan";
	case 0x4 : 	return "Fri";
	case 0x5 : 	return "Mar";
	case 0x6 : 	return "Wei";
	case 0x7 : 	return "Ano";
	case 0x8 : 	return "E-S";
	case 0x9 : 	return "Adl";
	case 0xA : 	return "Tnk";
	case 0xB : 	return "Mur";
	default:	return "Unk";
	}
}

void mainDatClass::cleanString(char *string, bool spaces_okay) {
	char *c = string;

	while (*c) {
		if ((*c<'A' || *c>'Z')
		    && (*c<'a' || *c>'z')
		    && (*c<'0' || *c>'9')
		    && !(*c=='.' || *c=='-' || *c == '_' || *c == '\'')
		    && (*c==' ' && !spaces_okay)) {
			*c='_';
		}

		c++;
	}
}

void mainDatClass::runAutoSave(){
	if (autoSaveFlg) {
		char new_replay_filename[1000];
		int n = 1;
		bool has_n = 0;
		while (1) {
			strcpy(new_replay_filename, "replay/");
			char *c = new_replay_filename + strlen(new_replay_filename);
			const char *src = replayFilenameFormat;
			while (*src) {
				if (*src == '%') {
					src++;
					if (*src == 'A' || *src == 'B') {
						int side;
						const char *str;
						bool isTemp;
						if (*src == 'A') {
							side = 0xA;
							str = p1PlayerName;
							isTemp = p1TempName;
						} else {
							side = 0xB;
							str = p2PlayerName;
							isTemp = p2TempName;
						}
						if (myInfo.terminalMode == mode_root && (!*str || isTemp)) {
							if (!unknownNameFlg) {
								if (myInfo.playerSide == side) {
									str = "Me";
								} else {
									str = inet_ntoa(Away.sin_addr);
								}
							} else {
								str = "Unknown";
							}
						}
						
						int len = strlen(str);
						memcpy(c, str, len+1);
						cleanString(c, 1);
						c += len;
					} else if (*src == '%') {
						*c++ = '%';
					} else if (*src == 'n') {
						sprintf(c, "%3.3d", n);
						c += strlen(c);
						has_n = 1;
					} else if (*src == 'd') {
						SYSTEMTIME time;
						if (systemTimeFlg) {
							GetSystemTime(&time);
						} else {
							GetLocalTime(&time);
						}
						sprintf(c, "%4.4d%2.2d%2.2d", time.wYear, time.wMonth, time.wDay);
						c += strlen(c);
					} else if (*src == 'D') {
						SYSTEMTIME time;
						if (systemTimeFlg) {
							GetSystemTime(&time);
						} else {
							GetLocalTime(&time);
						}
						sprintf(c, "%2.2d%2.2d%2.2d", time.wYear%100, time.wMonth, time.wDay);
						c += strlen(c);
					} else if (*src == 't') {
						SYSTEMTIME time;
						if (systemTimeFlg) {
							GetSystemTime(&time);
						} else {
							GetLocalTime(&time);
						}
						sprintf(c, "%2.2d%2.2d%2.2d", time.wHour, time.wMinute, time.wSecond);
						c += strlen(c);
					} else if (*src == 'T') {
						SYSTEMTIME time;
						if (systemTimeFlg) {
							GetSystemTime(&time);
						} else {
							GetLocalTime(&time);
						}
						sprintf(c, "%2.2d%2.2d", time.wHour, time.wMinute);
						c += strlen(c);
					} else if (*src == '1' || *src == '2') {
						WORD character = lastCharacterA;
						if (*src == '2') {
							character = lastCharacterB;
						}

						const char *str = getCharacterName(character);

						strcpy(c, str);
						c += strlen(c);
					} else if (*src == '3') {
						sprintf(c, "%d", (lastCharacterA&0xf)+1);
						c += strlen(c);
					} else if (*src == '4') {
						sprintf(c, "%d", (lastCharacterB&0xf)+1);
						c += strlen(c);
					} else if (*src == '5' || *src == '6') {
						WORD character = lastCharacterA;
						if (*src == '6') {
							character = lastCharacterB;
						}

						const char *str = getCharacterShortName(character);

						strcpy(c, str);
						c += strlen(c);
					} else if (*src == 'm') {
						sprintf(c, "%d", lastGameTime/60);
						c += strlen(c);
					} else if (*src == 'M') {
						sprintf(c, "%1.2f", (double)lastGameTime/60.0);
						c += strlen(c);
					} else if (*src == 'f') {
						sprintf(c, "%d", lastGameTime);
						c += strlen(c);
					}

					if (*src != '\0') {
						src++;
					}
				} else {
					*c++ = *src++;
				}
			}
			*c = '\0';

			// eh, whatever.
			DWORD fileAttr;
			fileAttr = GetFileAttributes(new_replay_filename);

			if (fileAttr == 0xFFFFFFFF) {
				break;
			}
			n++;

			if (!has_n) {
				cout << "Error: Replay filename already exists - Is your format lacking a %n or %t?" << endl;
				n = 0;
				break;
			}
		}

		if (n != 0) {
			strcat(new_replay_filename, ".rep");
			
			int l = strlen(new_replay_filename);
			
			WriteMemory((void *)0x18f8238, new_replay_filename, l+1);
		}
	}
}

HWND tmp_hWnd;

int mainDatClass::TestPort( SOCKADDR_IN* TargetTemp ){
	if( !TargetTemp ) return status_error;

	int Res;
	SOCKET sTemp;
	SOCKADDR_IN Target;
	SOCKADDR_IN HereTemp;
	memset( &HereTemp, 0, sizeof(SOCKADDR_IN) );

	WaitForSingleObject( hMutex, INFINITE );
	Target = *TargetTemp;
	ReleaseMutex( hMutex );



	HereTemp.sin_family = AF_INET;
	HereTemp.sin_addr.s_addr = htonl( INADDR_ANY );
	HereTemp.sin_port = htons( 10000 + rand()%1000 );

	Res = status_error;

	sTemp = socket(AF_INET , SOCK_DGRAM , 0);
	if( sTemp != INVALID_SOCKET ){
		if( bind( sTemp, (SOCKADDR*)&HereTemp, sizeof(SOCKADDR_IN)) >= 0 ){

			BYTE data[8];
			data[0] = cmd_version;
			data[1] = cmd_space_1;
			data[2] = cmd_space_2;
			data[3] = cmd_space_3;
			data[4] = cmd_testport;
			*(WORD*)&data[5] = myPort;

			testPortFlg = 0;
			int Counter = 0;
			for(;;){
				sendto( sTemp, (const char*)data, 7, 0, (SOCKADDR*)&Target, sizeof(SOCKADDR_IN) );
				Sleep(200);
				if( testPortFlg ){
					Res = status_ok;
					break;
				}
				if( Counter > 10 ){
					Res = status_bad;
					break;
				}
				if( GetEsc() ){
					break;
				}
				Counter++;
			}
		}
		closesocket(sTemp);
	}
	return Res;
}


typedef struct{
	WORD Port;
	SOCKET s;
}echoThStruct;

unsigned __stdcall echoTh( void* Address ){
	if( !Address ) return 1;
	echoThStruct* echoThData = (echoThStruct*)Address;

	SOCKADDR_IN addr;
	int size;
	int addrSize = sizeof(SOCKADDR_IN);
	size = recvfrom( echoThData->s, NULL, NULL, 0, (SOCKADDR*)&addr, &addrSize);
	if( size < 0 ){
		echoThData->Port = 0;
		return 1;
	}
	echoThData->Port = addr.sin_port;
	return 0;
}

int GetMyPortSub( SOCKET* s, WORD* myPortTemp ){
	if( !myPortTemp ) return 1;
	*myPortTemp = 0;

	DWORD res;
	echoThStruct echoThData;
	HANDLE hEchoTh;
	SOCKADDR_IN Echo;
	memset( &Echo, 0, sizeof(SOCKADDR_IN) );
	Echo.sin_family = AF_INET;
	Echo.sin_addr.s_addr = inet_addr( "127.0.0.1" );

	echoThData.Port = 0;
	echoThData.s = socket(AF_INET , SOCK_DGRAM , 0);
	if( echoThData.s == INVALID_SOCKET ) return 1;

	int Counter = 0;
	for(;;){
		Echo.sin_port = 10000 + rand()%10000;
		if( bind( echoThData.s, (SOCKADDR*)&Echo, sizeof(Echo) ) >= 0 ) break;
		if( Counter > 100 ) {
			closesocket(echoThData.s);
			return 1;
		}
		Counter++;
		Sleep( 1 );
	}


	hEchoTh = (HANDLE)_beginthreadex(NULL, 0, echoTh, &echoThData, 0, NULL);
	if( !hEchoTh ) return 1;

	Counter = 0;
	for(;;){
		sendto( *s, NULL, 0, 0, (SOCKADDR*)&Echo, sizeof(Echo) );

		res = WaitForSingleObject( hEchoTh, 10 );
		if( echoThData.Port ){
			*myPortTemp = echoThData.Port;
			break;
		}
		if( res == WAIT_OBJECT_0 || res == WAIT_FAILED ){
			closesocket(echoThData.s);
			return 1;
		}
		if( Counter > 20 ) return 1;
		Counter++;
	}
	closesocket(echoThData.s);
	Sleep(10);
	CloseHandle(hEchoTh);


	return 0;
}

int mainDatClass::GetMyPort(){
	WORD myPortTemp;
	if( GetMyPortSub( &s, &myPortTemp ) ){
//		cout << "Port : BAD ( UDP." << myPort << " Open failed )" << endl;
		cout << "Port : Unknown" << endl;
		myPort = 0;
		return 1;
	}
	myPort = ntohs( myPortTemp );
	cout << "Port : Using UDP." << dec << myPort << endl;
	return 0;
}


int mainDatClass::GetDelay( BYTE dest, float* delayTemp ){
	#if debug_mode_func
		cout << "debug : GetDelay()" << endl;
	#endif
	clock_t pTime;
	delayTimeObsNo = 0;
	DWORD timeTemp;


	//取り扱う時間の精度を設定
	TIMECAPS timeCaps;
	WORD timeCapsFlg = 0;
	if( timeGetDevCaps( &timeCaps, sizeof(timeCaps) ) == TIMERR_NOERROR ){
		if( timeBeginPeriod( timeCaps.wPeriodMin ) == TIMERR_NOERROR ){
			timeCapsFlg = 1;
		}
	}

	for(;;){
		timeTemp = timeGetTime();
		SendCmd( dest, cmd_delayobs, &timeTemp, 4 );
		if( delayTimeObsNo > 4 ) break;
		if( GetEsc() ) return 1;
		Sleep(50);
	}


	//時間の精度の設定を戻す
	if( timeCapsFlg ){
		if( TIMERR_NOERROR != timeEndPeriod( timeCaps.wPeriodMin ) ) return 1;
	}


	*delayTemp = ( delayTimeObs[ 0 ] + delayTimeObs[ 1 ] + delayTimeObs[ 2 ] + delayTimeObs[ 3 ] + delayTimeObs[ 4 ] ) /  5 ;
	return 0;
}

int mainDatClass::GetRand(){
    enRandNo = 0xff;
    enRand = 0;
	for(;;){
        if (enRandNo != 0xff) {
            break;
        }

		SendCmd( dest_root, cmd_rand );
		if( GetEsc() ) return 1;
		Sleep(200);
	}

	myRand = enRand;

	return 0;
}

int mainDatClass::GetPlayerSide(){
	#if debug_mode_func
		cout << "debug : GetPlayerSide()" << endl;
	#endif
	myRandNo = 1;
	if( playerSideFlg == 1 ){
		myRand = 0xFF;
	}else if( playerSideFlg == 2 ){
		myRand = 1;
	}else{
		myRand = (BYTE)( 1 + rand()%254 );
	}
	BYTE myRandTemp = myRand;
	BYTE currentRand;

	myInfo.playerSide = 0;
	enInfo.playerSide = 0;

	enRandNo = 0;
	enRand = 0;
	for(;;){
//		cout << (WORD)myRandNo << "." << (WORD)myRand << endl;
//		if( enInfo.playerSide ) break;
		if( myRandNo == enRandNo ){

			if( enRand == myRand ){
				if( myRandNo > 10 ) srand( (unsigned)time( NULL ) );

				if( playerSideFlg && playerSideHostFlg && enRandNo < 10 ){
					if( playerSideFlg == 1 ){
						myRand = 0xFF;
					}else{
						myRand = 1;
					}
				}else if( playerSideFlg && !playerSideHostFlg && enRandNo < 5 ){
					if( playerSideFlg == 1 ){
						myRand = 0xFF;
					}else{
						myRand = 1;
					}
				}else{
					while( myRandTemp == myRand ){
						myRand = (BYTE)( 1 + rand() % 254 );
					}
				}

				if( myRandNo > 250 ){
					return 1;
				}else{
					myRandNo = myRandNo + 1;
				}
				myRandTemp = myRand;
			}else{
				if( myRand > enRand ){
					myInfo.playerSide = 0xA;
					enInfo.playerSide = 0xB;
					currentRand = myRand;
				}else{
					myInfo.playerSide = 0xB;
					enInfo.playerSide = 0xA;
					currentRand = enRand;
				}
				break;
			}
		}else if( myRandNo < enRandNo ){
			if( playerSideFlg && playerSideHostFlg && enRandNo < 10 ){
				if( playerSideFlg == 1 ){
					myRand = 0xFF;
				}else{
					myRand = 1;
				}
			}else if( playerSideFlg && !playerSideHostFlg && enRandNo < 5 ){
				if( playerSideFlg == 1 ){
					myRand = 0xFF;
				}else{
					myRand = 1;
				}
			}else{
				while( myRandTemp == myRand ){
					myRand = (BYTE)( 1 + rand() % 254 );
				}
			}
			myRandNo = enRandNo;
			myRandTemp = myRand;
		}
		SendCmd( dest_away, cmd_rand );
//		SendCmd( dest_away, cmd_playerside );
		if( GetEsc() ) return 1;
		Sleep(50);
	}

	if( !myInfo.playerSide ){
		if( enInfo.playerSide == 0xA ){
			myInfo.playerSide = 0xB;
		}else if( enInfo.playerSide == 0xB ){
			myInfo.playerSide = 0xA;
		}
	}
	//要検討
	enInfo.playerSide = 0;
	for(;;){
		if( enInfo.playerSide ) break;
		SendCmd( dest_away, cmd_playerside );
		if( GetEsc() ) return 1;
		Sleep(100);
	}

	myRand = currentRand;
	return 0;
}

int mainDatClass::GetEsc(){

	if( escSelectFlg ){
		HWND  hForWnd;
		DWORD PID;

		hForWnd = GetForegroundWindow();
		if( hForWnd ){
			GetWindowThreadProcessId( hForWnd , &PID );
            if( GetCurrentProcessId() == PID ){
				if(GetKeyState(VK_ESCAPE)<0) return 1;
			}else if( th075Flg ){
				if( hForWnd == FindProcess() ){
					if(GetKeyState(VK_ESCAPE)<0) return 1;
				}
			}
		}else{
			if(GetKeyState(VK_ESCAPE)<0) return 1;
		}
	}else{
		if(GetKeyState(VK_ESCAPE)<0) return 1;
	}
	return 0;
}
BOOL CALLBACK EnumWindowsProc(HWND hwnd, LPARAM lParam) {
	char name[sizeof(proc_id)];
	if(hwnd==0) { return true; }
	GetWindowText(hwnd, name, sizeof(proc_id));
	/*cout << sizeof(proc_id) << ":" << proc_id << " | " << name << endl;
	Sleep(100);*/
	if(strcmp(name, proc_id) == 0) { tmp_hWnd=hwnd; }
	return true;
}

HWND mainDatClass::FindProcess() {
	tmp_hWnd=0;
	EnumWindows(EnumWindowsProc, NULL);
	return tmp_hWnd;
}

int mainDatClass::WriteCode( void* Address, BYTE code ){
	if( !th075Flg ) return 1;
	WriteProcessMemory( hProcess , Address , &code  , 1 , NULL );
	return 0;
}

int mainDatClass::UnRockTime(){
	BYTE code[5];

	if( !th075Flg ) return 1;
	code[0]=0x6A;
	code[1]=0x00;
	code[2]=0x6A;
	code[3]=0x00;
	code[4]=0x6A;

	WriteProcessMemory(hProcess, (void*)memLoc_rockTime, &code, 5, NULL );

	FlushInstructionCache(pi.hProcess, NULL, 0);

	rockFlg = 0;
	return 0;
}
int mainDatClass::RockTime(){
	BYTE code[5];

	if( !th075Flg ) return 1;
	code[0]=0xE9;
	code[1]=0xDE;
	code[2]=0x00;
	code[3]=0x00;
	code[4]=0x00;

	WriteProcessMemory(hProcess, (void*)memLoc_rockTime, &code, 5, NULL );

	FlushInstructionCache(pi.hProcess, NULL, 0);

	rockFlg = 1;
	return 0;
}

int mainDatClass::SetBodyBreakPoint(){
	if( !th075Flg ) return 1;

	BYTE code1[2];
	code1[0] = 0xCC;
	code1[1] = 0xCC;

	WriteProcessMemory(hProcess, (void *)body_int3_address, code1, 2, NULL);

	return 0;
}

//Char
int mainDatClass::SetCharBreakPoint(){
	if( !th075Flg ) return 1;
	BYTE code[2];
	code[0] = 0xCC;
	code[1] = 0x90;

	WriteProcessMemory(hProcess, (void*)char_int3_address, code, 2, NULL);
	return 0;
}



int mainDatClass::SetCode(){
	if( !th075Flg ) return 1;

	const char inputLoop_code[6] = { 0xcc, 0xe9, 0x8f, 0x00, 0x00, 0x00 };
	WriteMemory((void*)memLoc_inputLoop, (void *)inputLoop_code, 6);
	
	WriteCode((void*)memLoc_inputReadCall, 0x90);
	WriteCode((void*)(memLoc_inputReadCall+1), 0x90);
	WriteCode((void*)(memLoc_inputReadCall+2), 0x90);
	WriteCode((void*)(memLoc_inputReadCall+3), 0x90);
	WriteCode((void*)(memLoc_inputReadCall+4), 0x90);
	
	// kill frameskip
	WriteCode((void*)0x403fa7, 0x90);
	WriteCode((void*)0x403fa8, 0x90);
	
	// time-rng int3
	WriteCode((void*)0x4028c8, 0xcc);
	WriteCode((void*)0x4028c9, 0x90);
	WriteCode((void*)0x4028ca, 0x90);
	WriteCode((void*)0x4028cb, 0x90);
	WriteCode((void*)0x4028cc, 0x90);
	
	// kill config save
	WriteCode((void*)0x42e740, 0xc3);
	
	// force-unlock everything
	const DWORD unlock_code = 0xffffffff;
	WriteMemory((void*)0x4e9598, (void*)&unlock_code, 4);
	
	// change replay call to generate int3
	WriteCode((void*)0x403856, 0x85);
	
	// adjust timing loop
	static const char timingcode[12] = {
		0x4f, 0x90,
		0x89, 0x3d, 0xdc, 0x8f, 0xbf, 0x01,
		0x33, 0xc0,
		0xeb, 0x64
	};
	WriteProcessMemory(pi.hProcess, (void*)0x404084, (void*)timingcode, 12, 0);

	return 0;
}
int mainDatClass::RemoveCode(){
	if( !th075Flg ) return 1;

	//DirectInputを有効にする
	WriteCode( (void*)char_int3_address, 0x83 );

	return 0;
}


void mainDatClass::runKeybind(int key) {
    switch(key) {
    case KEY_AUTOSAVE_ON: {
        cout << "setting : replaySave on" << endl;
        
        unsigned char replayState;
        
        ReadMemory( (void*)0x4e9588, &replayState, 1);
        replayState |= 1;
        WriteMemory( (void*)0x4e9588, &replayState, 1);
        
        break;
        }
    case KEY_AUTOSAVE_OFF: {
        cout << "setting : replaySave off" << endl;

        unsigned char replayState;
        
        ReadMemory( (void*)0x4e9588, &replayState, 1);
        replayState &= ~1;
        WriteMemory( (void*)0x4e9588, &replayState, 1);
        
        break;
        }
    case KEY_AUTOSAVE_TOGGLE: {
        unsigned char replayState;
        
        ReadMemory( (void*)0x4e9588, &replayState, 1);
        
        if (replayState & 1) {
            runKeybind(KEY_AUTOSAVE_OFF);
        } else {
            runKeybind(KEY_AUTOSAVE_ON);
        }
        break;
        }
    case KEY_ROUNDCOUNT_CYCLE:
        roundShowFlg = (roundShowFlg+1)%3;
        cout << "setting : RoundShow " << roundShowFlg << " : ";
        if (roundShowFlg == 1) {
            cout << "Print end of session summary." << endl;
        } else if (roundShowFlg == 2) {
            cout << "Print all match info." << endl;
        } else {
            cout << "Print nothing." << endl;
        }
        break;
    case KEY_ALWAYSONTOP_TOGGLE:
        toggleWindowTopFlg = 1;
        break;
    default:
        // anything else is handled by the specific chunks of code that use them
        break;
    }
}

void mainDatClass::updateKeybinds() {
    if (!hWnd || GetForegroundWindow() != hWnd) {
        for (int i = 0; i < KEY_COUNT; ++i) {
            keystate[i] = 0;
        }

        return;
    }

    for (int i = 0; i < KEY_COUNT; ++i) {
        WORD key = keybinds[i];
        if (key != 0) {
            if (GetKeyState(key) < 0) {
                if (keystate[i] < 2) {
                    keystate[i]++;
                }
                if (keystate[i] == 1) {
                    runKeybind(i);
                }
            } else {
                keystate[i] = 0;
            }
        }
    }
}


int mainDatClass::ReadMemory(void* Address, void* data, DWORD size){
	DWORD	sizeRet;
	if( !th075Flg ) return 1;
	ReadProcessMemory( hProcess , Address , data , size , &sizeRet );
	if(sizeRet != size) return 2;
	return 0;
}

int mainDatClass::WriteMemory(void* Address, void* data, DWORD size){
	DWORD	sizeRet;
	if( !th075Flg ) return 1;
	WriteProcessMemory( hProcess , Address , data , size , &sizeRet );
	if(sizeRet != size) return 2;
	return 0;
}

//データ送信
int mainDatClass::SendData(int dest, void* Address, DWORD size){
	return SendDataSub( dest, Address, size, task_main );
}
int mainDatClass::SendData(SOCKADDR_IN* addr, void* Address, DWORD size){
	return SendDataSub( addr, Address, size, task_main );
}

int mainDatClass::SendDataR(int dest, void* Address, DWORD size){
	return SendDataSub( dest, Address, size, task_recv );
}
int mainDatClass::SendDataR(SOCKADDR_IN* addr, void* Address, DWORD size){
	return SendDataSub( addr, Address, size, task_recv );
}

int mainDatClass::SendDataSub( int dest, void* Address, DWORD size, WORD Flg ){
	if(size > stask_buf_size) return 1;

	int startValue = 0;
	int endValue = 0;
	if( Flg == task_main ){
		startValue = 0;
		endValue = 19;
	}else if( Flg == task_recv ){
		startValue = 20;
		endValue = 39;
	}else if( Flg == task_manage ){
		startValue = 40;
		endValue = 49;
	}

	int Counter;
	for(Counter=startValue; Counter<=endValue; Counter++){
		if(sTask[Counter].Flg == 0){
			sTask[Counter].dest = dest;
			memcpy( sTask[Counter].data, Address, size);
			sTask[Counter].size = size;
			sTask[Counter].Flg = stask_data;

			SetEvent(hSendEvent);
			break;
		}
		if(Counter==endValue){
			return 1;
		}
	}
	return 0;
}

int mainDatClass::SendDataSub( SOCKADDR_IN* addr, void* Address, DWORD size, WORD Flg ){
	if(size > stask_buf_size) return 1;

	int startValue = 0;
	int endValue = 0;
	if( Flg == task_main ){
		startValue = 0;
		endValue = 19;
	}else if( Flg == task_recv ){
		startValue = 20;
		endValue = 39;
	}else if( Flg == task_manage ){
		startValue = 40;
		endValue = 49;
	}

	int Counter;
	for(Counter=startValue; Counter<=endValue; Counter++){
		if(sTask[Counter].Flg == 0){
			sTask[Counter].dest = dest_addr;
			WaitForSingleObject( hMutex, INFINITE );
			sTask[Counter].addr = *addr;
			ReleaseMutex( hMutex );

			memcpy( sTask[Counter].data, Address, size);
			sTask[Counter].size = size;
			sTask[Counter].Flg = stask_data;

			SetEvent(hSendEvent);
			break;
		}
		if(Counter==endValue){
			return 1;
		}
	}
	return 0;
}


//連結データ送信
int mainDatClass::SendData(int dest, void* Address, DWORD size, void* Address2, DWORD size2){
	return SendDataSub( dest, Address, size, Address2, size2, task_main );
}
int mainDatClass::SendData(SOCKADDR_IN* addr, void* Address, DWORD size, void* Address2, DWORD size2){
	return SendDataSub( addr, Address, size, Address2, size2, task_main );
}

int mainDatClass::SendDataR(int dest, void* Address, DWORD size, void* Address2, DWORD size2){
	return SendDataSub( dest, Address, size, Address2, size2, task_recv );
}
int mainDatClass::SendDataR(SOCKADDR_IN* addr, void* Address, DWORD size, void* Address2, DWORD size2){
	return SendDataSub( addr, Address, size, Address2, size2, task_recv );
}
int mainDatClass::SendDataR(int dest, void* Address, DWORD size, void* Address2, DWORD size2, void* Address3, DWORD size3 ){
	return SendDataSub( dest, Address, size, Address2, size2, Address3, size3, task_recv );
}
int mainDatClass::SendDataR(SOCKADDR_IN* addr, void* Address, DWORD size, void* Address2, DWORD size2, void* Address3, DWORD size3 ){
	return SendDataSub( addr, Address, size, Address2, size2, Address3, size3, task_recv );
}

int mainDatClass::SendDataSub( int dest, void* Address, DWORD size, void* Address2, DWORD size2, void* Address3, DWORD size3, WORD Flg ){
	if(size + size2 + size3 > stask_buf_size) return 1;

	int startValue = 0;
	int endValue = 0;
	if( Flg == task_main ){
		startValue = 0;
		endValue = 19;
	}else if( Flg == task_recv ){
		startValue = 20;
		endValue = 39;
	}else if( Flg == task_manage ){
		startValue = 40;
		endValue = 49;
	}

	int Counter;
	for(Counter=startValue; Counter<=endValue; Counter++){
		if(sTask[Counter].Flg == 0){
			sTask[Counter].dest = dest;
			memcpy( sTask[Counter].data, Address, size);
			memcpy( sTask[Counter].data + size, Address2, size2);
			memcpy( sTask[Counter].data + size + size2, Address3, size3);
			sTask[Counter].size = size + size2 + size3;
			sTask[Counter].Flg = stask_data;

			SetEvent(hSendEvent);
			break;
		}
		if(Counter==endValue){
			return 1;
		}
	}
	return 0;
}

int mainDatClass::SendDataSub( SOCKADDR_IN* addr, void* Address, DWORD size, void* Address2, DWORD size2, void* Address3, DWORD size3, WORD Flg ){
	if(size + size2 + size3 > stask_buf_size) return 1;

	int startValue = 0;
	int endValue = 0;
	if( Flg == task_main ){
		startValue = 0;
		endValue = 19;
	}else if( Flg == task_recv ){
		startValue = 20;
		endValue = 39;
	}else if( Flg == task_manage ){
		startValue = 40;
		endValue = 49;
	}

	int Counter;
	for(Counter=startValue; Counter<=endValue; Counter++){
		if(sTask[Counter].Flg == 0){
			sTask[Counter].dest = dest_addr;

			WaitForSingleObject( hMutex, INFINITE );
			sTask[Counter].addr = *addr;
			ReleaseMutex( hMutex );
			memcpy( sTask[Counter].data, Address, size);
			memcpy( sTask[Counter].data + size, Address2, size2);
			memcpy( sTask[Counter].data + size + size2, Address3, size3);
			sTask[Counter].size = size + size2 + size3;
			sTask[Counter].Flg = stask_data;

			SetEvent(hSendEvent);
			break;
		}
		if(Counter==endValue){
			return 1;
		}
	}
	return 0;
}


int mainDatClass::SendDataSub( int dest, void* Address, DWORD size, void* Address2, DWORD size2, WORD Flg ){
	if(size + size2 > stask_buf_size) return 1;

	int startValue = 0;
	int endValue = 0;
	if( Flg == task_main ){
		startValue = 0;
		endValue = 19;
	}else if( Flg == task_recv ){
		startValue = 20;
		endValue = 39;
	}else if( Flg == task_manage ){
		startValue = 40;
		endValue = 49;
	}

	int Counter;
	for(Counter=startValue; Counter<=endValue; Counter++){
		if(sTask[Counter].Flg == 0){
			sTask[Counter].dest = dest;
			memcpy( sTask[Counter].data, Address, size);
			memcpy( sTask[Counter].data + size, Address2, size2);
			sTask[Counter].size = size + size2;
			sTask[Counter].Flg = stask_data;

			SetEvent(hSendEvent);
			break;
		}
		if(Counter==endValue){
			return 1;
		}
	}
	return 0;
}

int mainDatClass::SendDataSub( SOCKADDR_IN* addr, void* Address, DWORD size, void* Address2, DWORD size2, WORD Flg ){
	if(size + size2 > stask_buf_size) return 1;

	int startValue = 0;
	int endValue = 0;
	if( Flg == task_main ){
		startValue = 0;
		endValue = 19;
	}else if( Flg == task_recv ){
		startValue = 20;
		endValue = 39;
	}else if( Flg == task_manage ){
		startValue = 40;
		endValue = 49;
	}

	int Counter;
	for(Counter=startValue; Counter<=endValue; Counter++){
		if(sTask[Counter].Flg == 0){
			sTask[Counter].dest = dest_addr;

			WaitForSingleObject( hMutex, INFINITE );
			sTask[Counter].addr = *addr;
			ReleaseMutex( hMutex );
			memcpy( sTask[Counter].data, Address, size);
			memcpy( sTask[Counter].data + size, Address2, size2);
			sTask[Counter].size = size + size2;
			sTask[Counter].Flg = stask_data;

			SetEvent(hSendEvent);
			break;
		}
		if(Counter==endValue){
			return 1;
		}
	}
	return 0;
}

//アドレス
int mainDatClass::SendArea(int dest, void* Address, DWORD size){
	return SendAreaSub( dest, Address, size, task_main );
}
int mainDatClass::SendArea(SOCKADDR_IN* addr, void* Address, DWORD size){
	return SendAreaSub( addr, Address, size, task_main );
}

int mainDatClass::SendAreaR(int dest, void* Address, DWORD size){
	return SendAreaSub( dest, Address, size, task_recv );
}
int mainDatClass::SendAreaR(SOCKADDR_IN* addr, void* Address, DWORD size){
	return SendAreaSub( addr, Address, size, task_recv );
}

int mainDatClass::SendAreaSub( int dest, void* Address, DWORD size, WORD Flg ){
	if(size > stask_buf_size) return 1;

	int startValue = 0;
	int endValue = 0;
	if( Flg == task_main ){
		startValue = 0;
		endValue = 19;
	}else if( Flg == task_recv ){
		startValue = 20;
		endValue = 39;
	}else if( Flg == task_manage ){
		startValue = 40;
		endValue = 49;
	}

	int Counter;
	for(Counter=startValue; Counter<=endValue; Counter++){
		if(sTask[Counter].Flg == 0){
			sTask[Counter].dest = dest;
			sTask[Counter].Address = Address;
			sTask[Counter].size = size;
			sTask[Counter].Flg = stask_area;

			SetEvent(hSendEvent);
			break;
		}
		if(Counter == endValue){
			return 1;
		}
	}
	return 0;
}

int mainDatClass::SendAreaSub( SOCKADDR_IN* addr, void* Address, DWORD size, WORD Flg ){
	if(size > stask_buf_size) return 1;

	int startValue = 0;
	int endValue = 0;
	if( Flg == task_main ){
		startValue = 0;
		endValue = 19;
	}else if( Flg == task_recv ){
		startValue = 20;
		endValue = 39;
	}else if( Flg == task_manage ){
		startValue = 40;
		endValue = 49;
	}

	int Counter;
	for(Counter=startValue; Counter<=endValue; Counter++){
		if(sTask[Counter].Flg == 0){
			sTask[Counter].dest = dest_addr;
			WaitForSingleObject( hMutex, INFINITE );
			sTask[Counter].addr = *addr;
			ReleaseMutex( hMutex );

			sTask[Counter].Address = Address;
			sTask[Counter].size = size;
			sTask[Counter].Flg = stask_area;

			SetEvent(hSendEvent);
			break;
		}
		if(Counter==endValue){
			return 1;
		}
	}
	return 0;
}

//SendCmd()
int mainDatClass::SendCmd( int dest, BYTE Cmd ){
	BYTE data[5];
	data[0] = cmd_version;
	data[1] = cmd_space_1;
	data[2] = cmd_space_2;
	data[3] = cmd_space_3;
	data[4] = Cmd;
	if( SendData( dest, data, 5 ) ) return 1;
	return 0;
}

int mainDatClass::SendCmd( SOCKADDR_IN* addr, BYTE Cmd ){
	BYTE data[5];
	data[0] = cmd_version;
	data[1] = cmd_space_1;
	data[2] = cmd_space_2;
	data[3] = cmd_space_3;
	data[4] = Cmd;
	if( SendData( addr, data, 5 ) ) return 1;
	return 0;
}

int mainDatClass::SendCmdR( int dest, BYTE Cmd ){
	BYTE data[5];
	data[0] = cmd_version;
	data[1] = cmd_space_1;
	data[2] = cmd_space_2;
	data[3] = cmd_space_3;
	data[4] = Cmd;
	if( SendDataR( dest, data, 5 ) ) return 1;
	return 0;
}

int mainDatClass::SendCmdR( SOCKADDR_IN* addr, BYTE Cmd ){
	BYTE data[5];
	data[0] = cmd_version;
	data[1] = cmd_space_1;
	data[2] = cmd_space_2;
	data[3] = cmd_space_3;
	data[4] = Cmd;
	if( SendDataR( addr, data, 5 ) ) return 1;
	return 0;
}

//SendCmd+
int mainDatClass::SendCmd( int dest, BYTE Cmd, void* Address, DWORD size ){
	BYTE data[5];
	data[0] = cmd_version;
	data[1] = cmd_space_1;
	data[2] = cmd_space_2;
	data[3] = cmd_space_3;
	data[4] = Cmd;
	if( SendData( dest, data, 5, Address, size ) ) return 1;
	return 0;
}

int mainDatClass::SendCmd( SOCKADDR_IN* addr, BYTE Cmd, void* Address, DWORD size ){
	BYTE data[5];
	data[0] = cmd_version;
	data[1] = cmd_space_1;
	data[2] = cmd_space_2;
	data[3] = cmd_space_3;
	data[4] = Cmd;
	if( SendData( addr, data, 5, Address, size ) ) return 1;
	return 0;
}

int mainDatClass::SendCmdR( int dest, BYTE Cmd, void* Address, DWORD size ){
	BYTE data[5];
	data[0] = cmd_version;
	data[1] = cmd_space_1;
	data[2] = cmd_space_2;
	data[3] = cmd_space_3;
	data[4] = Cmd;
	if( SendDataR( dest, data, 5, Address, size ) ) return 1;
	return 0;
}

int mainDatClass::SendCmdR( SOCKADDR_IN* addr, BYTE Cmd, void* Address, DWORD size ){
	BYTE data[5];
	data[0] = cmd_version;
	data[1] = cmd_space_1;
	data[2] = cmd_space_2;
	data[3] = cmd_space_3;
	data[4] = Cmd;
	if( SendDataR( addr, data, 5, Address, size ) ) return 1;
	return 0;
}

//3
int mainDatClass::SendCmdR( int dest, BYTE Cmd, void* Address2, DWORD size2, void* Address3, DWORD size3 ){
	BYTE data[5];
	data[0] = cmd_version;
	data[1] = cmd_space_1;
	data[2] = cmd_space_2;
	data[3] = cmd_space_3;
	data[4] = Cmd;
	if( SendDataR( dest, data, 5, Address2, size2, Address3, size3 ) ) return 1;
	return 0;
}

int mainDatClass::SendCmdR( SOCKADDR_IN* addr, BYTE Cmd, void* Address2, DWORD size2, void* Address3, DWORD size3 ){
	BYTE data[5];
	data[0] = cmd_version;
	data[1] = cmd_space_1;
	data[2] = cmd_space_2;
	data[3] = cmd_space_3;
	data[4] = Cmd;
	if( SendDataR( addr, data, 5, Address2, size2, Address3, size3 ) ) return 1;
	return 0;
}

int mainDatClass::SendCmdM( int dest, BYTE Cmd ){
	BYTE data[5];
	data[0] = cmd_version;
	data[1] = cmd_space_1;
	data[2] = cmd_space_2;
	data[3] = cmd_space_3;
	data[4] = Cmd;
	if( SendDataSub( dest, data, 5, task_manage ) ) return 1;
	return 0;
}

int mainDatClass::SendCmdM( SOCKADDR_IN* addr, BYTE Cmd ){
	BYTE data[5];
	data[0] = cmd_version;
	data[1] = cmd_space_1;
	data[2] = cmd_space_2;
	data[3] = cmd_space_3;
	data[4] = Cmd;
	if( SendDataSub( addr, data, 5, task_manage ) ) return 1;
	return 0;
}
