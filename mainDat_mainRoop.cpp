#include "mainDatClass.h"
using namespace std;


int mainDatClass::mainRoop(){
	int Res;

	Res = WaitForIni();
	while( Res != 0xF ){
		Res = WaitForRoopFlg();
		if( Res == 0 ){
			if( charInit() == 0 ){
				if( SetCode() == 0 ){
					while( Res == 0 ){
						Res = WaitForMenu();
						if( Res == 0 ){
							Res = ManipMenu();
							if( Res == 0 ){
								Res = WaitForStart();
								if( Res == 0 ){
									Res = mainStep();
								}
							}
						}
					}
					//RemoveCode();
				}else{
					cout << "ERROR : SetCode() failed." << endl;
					roopFlg = 0;
				}
				charEnd();
			}else{
				cout << "ERROR : charInit() failed." << endl;
				cout << "This probably indicates an error initializing your pad." << endl;
				cout << "Make sure inputDevice is set to an appropiate value in config_caster.ini," << endl;
				cout << "and that your pad is correctly setup." << endl;
				roopFlg = 0;
			}
		}
		if( ( th075QuitFlg || !roopFlg ) && Res != 0xF ){
			Res = th075Quit();
		}
	}
	roopFlg = 0;
	fixCfgFlg=0;

	return 0;
}

int mainDatClass::WaitForIni(){
	#if debug_mode_mainRoop
		WaitForSingleObject( hPrintMutex, INFINITE );
		cout << "debug : WaitForIni()" << endl;
		ReleaseMutex( hPrintMutex );
	#endif

	//必要ない？
	DWORD deInfo;
	DWORD Counter = 0;
	for(;;){
		if( th075Roop( &deInfo ) ) return 0xF;
		if( deInfo == de_body ){
			Counter++;
			if( Counter > 10 ) break;
		}
	}
	return 0;
}

int mainDatClass::th075Quit(){
	#if debug_mode_mainRoop
		WaitForSingleObject( hPrintMutex, INFINITE );
		cout << "debug : th075Quit()" << endl;
		ReleaseMutex( hPrintMutex );
	#endif

	myInfo.A.phase = 0;
	myInfo.B.phase = 0;

	dataInfo.A.phase = 0;
	dataInfo.B.phase = 0;

	DWORD deInfo;

	BYTE menuFlg;
	BYTE gameMode;
	DWORD FlgA,FlgB;
	BYTE HPCountA,HPCountB;
	HWND hWnd;

	WORD Flg = 1;
	for(;;){
		if( th075Roop( &deInfo ) ) break;
		if( deInfo == de_body ){
			if( Flg ){
				hWnd = FindProcess();
				if( hWnd ){
					PostMessage(hWnd, WM_CLOSE, 0, 0);
					Flg = 0;
				}
			}
		}
	}
	return 0xF;
}

int mainDatClass::WaitForRoopFlg(){
	#if debug_mode_mainRoop
		WaitForSingleObject( hPrintMutex, INFINITE );
		cout << "debug : WaitForRoopFlg()" << endl;
		ReleaseMutex( hPrintMutex );
	#endif

	myInfo.phase = phase_default;
	myInfo.A.phase = 0;
	myInfo.B.phase = 0;

	dataInfo.A.phase = 0;
	dataInfo.B.phase = 0;

	DWORD deInfo;
	if( !roopFlg ){
		UnRockTime();
		for(;;){
			if( th075Roop( &deInfo ) ) return 0xF;
			if( roopFlg ) break;
		}
	}

	return 0;
}



int mainDatClass::WaitForMenu(){
	#if debug_mode_mainRoop
		WaitForSingleObject( hPrintMutex, INFINITE );
		cout << "debug : WaitForMenu()" << endl;
		ReleaseMutex( hPrintMutex );
	#endif

	DWORD deInfo;

	myInfo.phase = phase_default;

	myInfo.A.phase = 0;
	myInfo.B.phase = 0;

	dataInfo.A.phase = 0;
	dataInfo.B.phase = 0;

	//sessionNo
	if( myInfo.sessionID != myInfo.sessionIDNext ){
		if( myInfo.sessionNo < 255 ){
			myInfo.sessionNo = myInfo.sessionNo + 1;
		}else{
			myInfo.sessionNo = 1;
		}
		myInfo.sessionID = myInfo.sessionIDNext;
	}

	int InputA;
	int InputB;
	
	BYTE turboButton;
	for(;;){
		if( th075Roop( &deInfo ) ) {
			return 0xF;
		}
		if( !roopFlg ){
			UnRockTime();
			return 1;
		}
		if( deInfo == de_body ){
			// debug
			//InputA = datA.GetInput();
			InputA = 0;
			InputB = 0;

			DWORD gamePhase;
			ReadProcessMemory(hProcess, (void *)memLoc_gamePhase, &gamePhase, 4, NULL );
			
			if (gamePhase == gamePhase_postmatch) {
				if (autoNextFlg) {
					if (turboButton == 0) {
						InputA = key_A;
						turboButton = 1;
					} else {
						turboButton = 0;
					}
				} else {
					InputA = datA.GetInput();
				}
			}
			
			if (gamePhase == gamePhase_splash) {
				changeSpeed(18);
				if(turboButton==0) {
					InputA = key_A;
					turboButton=1;
				} else { turboButton=0; }
			}
			
			if (gamePhase == gamePhase_menu || gamePhase == gamePhase_intro) {
				changeSpeed(18);
				if (turboButton == 1) {
					turboButton = 0;
				} else {
					DWORD menuPointer;
					ReadProcessMemory(hProcess, (void *)memLoc_menuPointer, &menuPointer, 4, NULL);
					if (menuPointer == mainMenu_title || menuPointer == mainMenu_versus || menuPointer == mainMenu_intro) {
						InputA = key_A;
					} else if (menuPointer == mainMenu_menu) {
						BYTE selection;
						
						ReadProcessMemory(hProcess, (void *)memLoc_menuSelection, &selection, 1, NULL);
						
						if (selection != 1) {
							InputA = key_down;
						} else {
							InputA = key_A;
						}
					}
				}
			}
			
			if (gamePhase == gamePhase_charsel) {
				changeSpeed(16);
				break;
			}

			datA.SetBodyInput( InputA );
			datB.SetBodyInput( InputB );
		}
	}

	return 0;
}

int mainDatClass::WaitForStart(){
	#if debug_mode_mainRoop
		WaitForSingleObject( hPrintMutex, INFINITE );
		cout << "debug : WaitForStart()" << endl;
		ReleaseMutex( hPrintMutex );
	#endif
	BYTE turboButton=0;
	int InputA;
	DWORD deInfo;

	myInfo.phase = phase_read;
	dataInfo.phase = phase_read;

	for(;;){
		if( th075Roop( &deInfo ) ) return 0xF;
		if( !roopFlg ){
			UnRockTime();
			return 1;
		}
		if( deInfo == de_body ){
			//gameTimeを待つ
			DWORD gamePhase;
			ReadMemory( (void *)memLoc_gamePhase, &gamePhase, 4 );
			
			InputA = 0;
			if (gamePhase == gamePhase_loading || gamePhase == gamePhase_loading2) {
				if(turboButton==0) {
					InputA = key_A;
					turboButton=1;
				} else { turboButton=0; }
			} else if (gamePhase) {
				if (gamePhase != gamePhase_splash
				    && gamePhase != gamePhase_menu
				    && gamePhase != gamePhase_intro
				    && gamePhase != gamePhase_charsel
				    && gamePhase != gamePhase_loading
				    && gamePhase != gamePhase_loading2
				    && gamePhase != gamePhase_postmatch) {
				    	myInfo.gameTime = 1;
				    	break;
				}
			}
			datA.SetBodyInput( InputA );
		}

	}


	//要検討
	if( myInfo.terminalMode == mode_root || myInfo.terminalMode == mode_debug || myInfo.terminalMode == mode_broadcast ){
		if( inputData.Start( &myInfo ) ) return 1;
	}else if( myInfo.terminalMode == mode_branch || myInfo.terminalMode == mode_subbranch ){
		if( inputData.Start( &dataInfo ) ) return 1;
	}

	if( syncData.init() ) return 1;

	//要検討
	myInfo.phase = phase_battle;
	dataInfo.phase = phase_battle;
	{
		RockTime();

		if( myInfo.terminalMode == mode_root ){
			for(;;){
				if( th075Roop( &deInfo ) ) return 0xF;
				if( !roopFlg ){
					UnRockTime();
					return 1;
				}
				if( deInfo == de_body ){
					if( myInfo.playerSide == 0xA ){
						for(;;){
							myInfo.sessionIDNext = 1 + rand()%255;
							if( myInfo.sessionIDNext != myInfo.sessionID ) break;
						}
						break;
					}else{
						SendCmd( dest_away, cmd_session );
						if( enInfo.sessionID != enInfo.sessionIDNext ){
							myInfo.sessionIDNext = enInfo.sessionIDNext;
							break;
						}
					}
				}
			}
			dataInfo = myInfo;
		}
		if( myInfo.terminalMode == mode_broadcast || myInfo.terminalMode == mode_debug ){
			for(;;){
				myInfo.sessionIDNext = 1 + rand()%255;
				if( myInfo.sessionIDNext != myInfo.sessionID ) break;
			}
			dataInfo = myInfo;
		}
		if( myInfo.terminalMode == mode_branch || myInfo.terminalMode == mode_subbranch){
			for(;;){
				if( th075Roop( &deInfo ) ) return 0xF;
				if( !roopFlg ){
					UnRockTime();
					return 1;
				}
				if( deInfo == de_body ){
					SendCmd( dest_root, cmd_dataInfo );
					if( dataInfo.sessionID != dataInfo.sessionIDNext ){
						break;
					}
				}
			}
		}

		#if debug_mode_mainRoop
			WaitForSingleObject( hPrintMutex, INFINITE );
			cout << "debug : WaitForEnStart" << endl;
			ReleaseMutex( hPrintMutex );
		#endif
		if( myInfo.terminalMode == mode_root ){
			enInfo.gameTime = 0;
			myInfo.phase = phase_battle;
			for(;;){
				if( th075Roop( &deInfo ) ) return 0xF;
				if( !roopFlg ){
					UnRockTime();
					return 1;
				}
				if( deInfo == de_body ){
					//相手のgameTime==1を待つ
					if( enInfo.gameTime ) break;
					//if (enInfo.phase == phase_battle) break;

					//送信
					SendCmd( dest_away, cmd_gameInfo );
				}
			}
		}
		if( myInfo.terminalMode == mode_branch || myInfo.terminalMode == mode_subbranch){
			dataInfo.gameTime = 0;
			myInfo.phase = phase_battle;
			dataInfo.phase = phase_battle;
			for(;;){
				if( th075Roop( &deInfo ) ) return 0xF;
				if( !roopFlg ){
					UnRockTime();
					return 1;
				}
				if( deInfo == de_body ){
					if( dataInfo.gameTime > 10 ) break;
					//if( dataInfo.phase == phase_battle ) break;

					if( !Root.sin_addr.s_addr ) roopFlg = 0;
				}
			}
		}
		UnRockTime();
	}

	return 0;
}


