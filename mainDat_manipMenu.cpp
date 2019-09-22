#include "mainDatClass.h"
using namespace std;

void mainDatClass::fixOptions() {
}

static BYTE ManipMenuSub( charInfoStruct* now, charInfoStruct* dest){
	if (dest->phase < 1 || dest->phase > 5) {
		return 0;
	}
	
	if(dest->phase >= now->phase){
		switch( now->phase ){
		case 1 : {
				int dest_ID = dest->ID&0xf;
				int now_ID = now->ID&0xf;
				if( dest_ID != now_ID ){
					int n = dest_ID - now_ID;
					if (n < 0) {
						n += 12;
					}
					if (n > 6) {
						return key_up;
					} else {
						return key_down;
					}
				}
				
				int dest_color = dest->color&0xf;
				int now_color = now->color&0xf;
				if (dest_color != now_color) {
					int n = dest_color - now_color;
					if (n < 0) {
						n += 7;
					}
					
					if (n > 3) {
						return key_left;
					}
					return key_right;
				}
				
				if (dest->phase > now->phase) {
					return key_A;
				}
				return 0;
			}
		case 2 : {
			// waiting on other player
			int dest_ID = dest->ID&0xf;
			int now_ID = now->ID&0xf;
			if (dest_ID != now_ID) {
				return key_B;
			}
			
			return 0;
			}
		case 3 :
			//place
			{
				int dest_ID = dest->ID&0xf;
				int now_ID = now->ID&0xf;
				int dest_color = dest->color&0xf;
				int now_color = now->color&0xf;
				
				if (dest_ID != now_ID || dest_color != now_color) {
					return key_B;
				}
				
				int sel = (now->color&0xc0)>>6;
				
				int d_time = (dest->place&0xe0)>>5;
				int n_time = (now->place&0xe0)>>5;
				
				if (d_time != n_time) {
					if (sel == 0) {
						return key_down;
					} else if (sel == 2) {
						return key_up;
					}
					
					if (d_time > n_time) {
						return key_right;
					} else {
						return key_left;
					}
				}
				
				int d_rounds = (dest->ID&0xe0)>>5;
				int n_rounds = (now->ID&0xe0)>>5;
				
				if (d_rounds != n_rounds) {
					if (sel != 2) {
						return key_down;
					}
					if (d_rounds > n_rounds) {
						return key_right;
					} else {
						return key_left;
					}
				}
				
				int dp = dest->place&0xf;
				int np = now->place&0xf;
				if (dp != np) {
					if (sel != 0) {
						return key_up;
					}
					
					int n = dp - np;
					if (n < 0) {
						n += 0xc;
					}
					if (n < 0x6) {
						return key_right;
					} else {
						return key_left;
					}
				}
			
				if (dest->phase > now->phase) {
					if (sel != 0) {
						return key_up;
					}
					
					return key_A;
				}
				return 0;
			}
		}
	}

	if(dest->phase < now->phase){
		if (dest->phase == 2) {
			int dest_ID = dest->ID&0xf;
			int now_ID = now->ID&0xf;
			int dest_color = dest->color&0xf;
			int now_color = now->color&0xf;
			
			if (dest_ID == now_ID && dest_color == now_color) {
				return 0;
			}
		}
		
		return key_B;
	}
	
	return 0;
}



int mainDatClass::ManipMenu(){
	#if debug_mode_mainRoop
		WaitForSingleObject( hPrintMutex, INFINITE );
		cout << "debug : ManipMenu()" << endl;
		ReleaseMutex( hPrintMutex );
	#endif

	memset( &enInfo, 0, sizeof(enInfo));

	DWORD gamePhase = 0;

	WORD pushFlg = 0;
	BYTE InputA;
	BYTE InputB;

	myInfo.A.phase = 1;
	myInfo.B.phase = 1;

	enInfo.A.phase = 1;
	enInfo.B.phase = 1;

	enInfo.A.ID = 0x12;
	enInfo.B.ID = 0x17;

	dataInfo.A.phase = 1;
	dataInfo.B.phase = 1;

	datA.SetBodyInput( 0 );
	datB.SetBodyInput( 0 );

	myInfo.gameTime = 0;
	enInfo.gameTime = 0;
	dataInfo.gameTime = 0;

	DWORD roopCounter = 0;

	HWND hWnd = FindProcess();

	DWORD keyCancelCounter = 0;

	DWORD deInfo;
	
	DWORD initCount = 0;

	srand (time(NULL));
	
	for(;;){
		if( th075Roop( &deInfo ) ) return 0xF;
		if( !roopFlg ){
			UnRockTime();
			return 1;
		}
		if( deInfo == de_body ){
			myInfo.phase = phase_menu;
			InputA = 0;
			InputB = 0;

			ReadProcessMemory( hProcess, (void*)memLoc_gamePhase , &gamePhase , 4 , NULL );
			
			if (gamePhase == 0) {
				continue;
			}
			
			if (gamePhase == gamePhase_menu || gamePhase == gamePhase_intro) {
			    datA.SetInput( 0 );
			    datB.SetInput( 0 );
			    return 1;
			}

			BYTE time_limit = 0;
			ReadProcessMemory( hProcess, (void*)0x4ef278, &time_limit, 1, NULL );

			if (gamePhase == gamePhase_loading
			    || gamePhase == gamePhase_loading2
			    || (gamePhase != gamePhase_splash
			        && gamePhase != gamePhase_charsel
			        && gamePhase != gamePhase_postmatch)) {
			    if (myInfo.place == 0xC) {
			    	// in case of random select, read the
			    	// real stage value in.
			    	// remap it properly too.
			    	static const BYTE place_lookup[0xc] = {
			    		1, 0, 2, 3, 4, 5, 8, 7, 6, 10, 11, 9
			    	};
			    	BYTE my_place;
				ReadProcessMemory( hProcess, (void*)0x4EF2A4, &my_place , 1 , NULL );
				
				if (my_place < 0 || my_place > 0xb) {
					my_place = 0;
				}
				
				my_place = place_lookup[my_place];
				
				myInfo.place = my_place;
				
				myInfo.A.place = (myInfo.place&0xf) | 0x10 | (time_limit<<5);
				myInfo.B.place = (myInfo.place&0xf) | 0x10 | (time_limit<<5);
			
			    }
			    myInfo.phase = 5;
			    break;
			}
			
			if (gamePhase != gamePhase_charsel) {
			    continue;
			}
			
			if (initCount < 70) {
			    initCount++;
			    continue;
			}
			
			BYTE  lockin;
			ReadProcessMemory( hProcess, (void*)0xfbb878, &lockin, 1, NULL );
			if (lockin) {
				myInfo.A.phase = 2;
			} else {
				myInfo.A.phase = 1;
			}
			ReadProcessMemory( hProcess, (void*)0xfbb88c, &lockin, 1, NULL );
			if (lockin) {
				myInfo.B.phase = 2;
			} else {
				myInfo.B.phase = 1;
			}

			BYTE submenu = 0;

			if (myInfo.A.phase == 2 && myInfo.B.phase == 2) {
				myInfo.A.phase = 3;
				myInfo.B.phase = 3;
				
				ReadProcessMemory( hProcess, (void*)0xfbeb7c, &submenu, 1, NULL );
			}
			
			BYTE rounds = 0;
			ReadProcessMemory( hProcess, (void*)0x4ef284, &rounds, 1, NULL );
			
			BYTE my_ID, my_color;
			ReadProcessMemory( hProcess, (void*)0xFBEB18, &my_ID , 1 , NULL );
			ReadProcessMemory( hProcess, (void*)0xFBEB40, &my_color, 1, NULL);
			
			if (my_ID == 0xC && myInfo.A.phase > 1) {
				ReadProcessMemory( hProcess, (void*)0xFC804A, &my_ID, 1, NULL );
			}
			
			myInfo.A.ID = (my_ID&0xf) | 0x10 | (rounds<<5);
			
			myInfo.A.color = (my_color&0xf) | 0x10 | (submenu<<6);
			
			ReadProcessMemory( hProcess, (void*)0xFBEB1C, &my_ID , 1 , NULL );
			ReadProcessMemory( hProcess, (void*)0xFBEB44, &my_color, 1, NULL);

			if (my_ID == 0xC && myInfo.B.phase > 1) {
				ReadProcessMemory( hProcess, (void*)0xFC984A, &my_ID, 1, NULL );
			}
			
			myInfo.B.ID = (my_ID&0xf) | 0x10 | (rounds<<5);
			myInfo.B.color = (my_color&0xf) | 0x10 | (submenu<<6);
			
			ReadProcessMemory( hProcess, (void*)0xFBEB3C, &myInfo.place , 1 , NULL );
			// 4EF2A4 - real stage, but only after choosing, and needs to be remapped.
			
			myInfo.A.place = (myInfo.place&0xf) | 0x10 | (time_limit<<5);
			myInfo.B.place = (myInfo.place&0xf) | 0x10 | (time_limit<<5);
			
			fixOptions();
			if( myInfo.terminalMode == mode_broadcast || myInfo.terminalMode == mode_root || myInfo.terminalMode == mode_debug ){
				dataInfo = myInfo;
			}


				if( myInfo.terminalMode == mode_root || myInfo.terminalMode == mode_debug ){
					//ルートの場合
					if( enInfo.phase == phase_battle || enInfo.phase == phase_read ){
						enInfo.A.phase = 5;
						enInfo.B.phase = 5;
					}

					//データを要求
					if (myInfo.terminalMode == mode_debug) {
						enInfo = myInfo;
					} else {
						SendCmd( dest_away, cmd_gameInfo );
					}

					//delay
						WORD delayTimeNew = 0;
						if (keystate[KEY_DELAY1] == 1) delayTimeNew = 2;
						if (keystate[KEY_DELAY2] == 1) delayTimeNew = 4;
						if (keystate[KEY_DELAY3] == 1) delayTimeNew = 6;
						if (keystate[KEY_DELAY4] == 1) delayTimeNew = 8;
						if (keystate[KEY_DELAY5] == 1) delayTimeNew = 10;
						if (keystate[KEY_DELAY6] == 1) delayTimeNew = 12;
						if (keystate[KEY_DELAY7] == 1) delayTimeNew = 14;
						if (keystate[KEY_DELAY8] == 1) delayTimeNew = 16;
						if (keystate[KEY_DELAY9] == 1) delayTimeNew = 18;
						if (keystate[KEY_DELAY10] == 1) delayTimeNew = 20;
						if( delayTimeNew ){
							if( hWnd == GetForegroundWindow() ){
								if( delayTime != delayTimeNew ){
									delayTime = delayTimeNew;
									cout << "debug : Buffer Margin ( delayTime ) " << delayTime / 2 << endl;
								}
							}
						}



					//if(menuFlg == 3 && battleFlgA == 0 && battleFlgB == 0 ){
					if(1) {
						if( myInfo.playerSide == 0xA ){
							InputA = datA.GetInput();

							//前面かどうか
							if( datA.inputDeviceType == 0xFF ){
								if( hWnd != GetForegroundWindow() ){
									InputA = 0;
								}
							}
							if( myInfo.terminalMode == mode_debug ){
								if( myInfo.A.phase > 1 && myInfo.B.phase > 1 ){
									if( keyCancelCounter > 40 ){
										//none
									}else{
										InputA = 0;
										keyCancelCounter++;
									}
								}
							}

							InputB = ManipMenuSub( &myInfo.B, &enInfo.B );
							if( myInfo.B.phase == 3 && InputB != key_B ){
								InputB = 0;
							}
						}
						if( myInfo.playerSide == 0xB ){
							//テスト
							/*
							if( enInfo.A.place > MAX_STAGES-1 ){
								enInfo.A.place = 0;
							}
							 */
							if(enInfo.A.phase>0) {
								InputA = ManipMenuSub( &myInfo.A, &enInfo.A );
							}

							InputB = datA.GetInput();

							//前面かどうか
							if( datA.inputDeviceType == 0xFF ){
								if( hWnd != GetForegroundWindow() ){
									InputB = 0;
								}
							}

							if( myInfo.B.phase == 3 && InputB != key_B ){
								InputB = 0;
							}
						}

						if( myInfo.A.phase == 3 && myInfo.B.phase == 3 && myInfo.sessionNo == enInfo.sessionNo){
							//検証する
							if( myInfo.A.ID == enInfo.A.ID && myInfo.B.ID == enInfo.B.ID && myInfo.place == enInfo.place)
							{
								if(enInfo.phase == phase_battle || enInfo.phase == phase_read) { InputA = key_A; }
							}
						}
					}else{
						datA.SetInput( 0 );
						datB.SetInput( 0 );

						if( myInfo.playerSide == 0xA ){
							break;
						}else{
							//検証する
							if(myInfo.A.ID == enInfo.A.ID && myInfo.B.ID == enInfo.B.ID && myInfo.place == enInfo.place)
							{
								//次のステップへ
								break;
							}else{
								return 1;
							}
						}
					}


					//連打修正
					if( pushFlg ){
						if( myInfo.playerSide == 0xA ) InputB = 0;
						if( myInfo.playerSide == 0xB ) InputA = 0;
					}

					//sessionNo
					if( myInfo.sessionNo != enInfo.sessionNo ){
						if( myInfo.playerSide == 0xA ){
							InputB = 0;
						}
						if( myInfo.playerSide == 0xB ){
							InputA = 0;
						}
					}

				}else if( myInfo.terminalMode == mode_branch || myInfo.terminalMode == mode_subbranch ){
					//転送の場合
					if( dataInfo.phase == phase_battle || dataInfo.phase == phase_read ){
						dataInfo.A.phase = 5;
						dataInfo.B.phase = 5;
					}

					    if (0) { // gameMode == 8 || gameMode == 1) {
                            break;
					    }


					//データを要求
					if( !roopCounter ){
						SendCmd( dest_root, cmd_dataInfo );
					}
					roopCounter++;
					if( roopCounter > 6 ) roopCounter = 0;

					if( 1 ) { // gameMode==0 ){
						InputA = ManipMenuSub( &myInfo.A, &dataInfo.A );
						InputB = ManipMenuSub( &myInfo.B, &dataInfo.B );

						if( myInfo.B.phase == 3 && InputB != key_B ){
							InputB = 0;
						}
						//検証する
						if( (dataInfo.phase == phase_battle || dataInfo.phase == phase_read ) && ( myInfo.A.phase == 3 && myInfo.B.phase == 3 ) ){
							if( myInfo.A.ID == dataInfo.A.ID && myInfo.B.ID == dataInfo.B.ID && myInfo.place == dataInfo.place)
							{
								InputA = key_A;
							}
						}
					}else{
						datA.SetInput( 0 );
						datB.SetInput( 0 );

						//検証する
						if( myInfo.A.ID == dataInfo.A.ID && myInfo.B.ID == dataInfo.B.ID && myInfo.place == dataInfo.place)
						{
							//次のステップへ
							break;
						}else{
							return 1;
						}
					}

					//連打修正
					if( pushFlg ){
						InputA = 0;
						InputB = 0;
					}
				}else if( myInfo.terminalMode == mode_broadcast ){
					//ブロードキャストの場合
					if( 1 ) { // gameMode==0 ){
						InputA = datA.GetInput();
						if( datA.inputDeviceType == 0xFF ){
							if( hWnd != GetForegroundWindow() ){
								InputA = 0;
							}
						}
						InputB = datB.GetInput();
						if( datB.inputDeviceType == 0xFF ){
							if( hWnd != GetForegroundWindow() ){
								InputB = 0;
							}
						}
					}else{
						//次のステップへ
						break;
					}
				}

				if( myInfo.terminalMode == mode_debug ) { // && !practiceModeFlg){
					if( myInfo.A.phase > 1 && myInfo.B.phase < 2 ){
						if( keyCancelCounter > 20 ){
							InputB = InputA;
							if( myInfo.B.phase == 1 && InputA & key_B ){
								//none
								InputB = 0; // I'm pretty sure we need this?
							}else{
								InputA = 0;
							}
						}else{
							keyCancelCounter++;
						}
					}else{
						if( myInfo.A.phase > 1 && myInfo.B.phase > 1 ){
							//none
						}else{
							//InputB = datB.GetInput();
							keyCancelCounter = 0;
						}
					}
				}

				if( pushFlg ){
					pushFlg--;
				}else{
					pushFlg = 5;
				}

			datA.SetBodyInput( InputA );
			datB.SetBodyInput( InputB );

			//LastInputA = InputA;
			//LastInputB = InputB;

		}
	}
	return 0;
}
