#include "mainDatClass.h"
using namespace std;

void mainDatClass::changeSpeed(int shift) {
	WriteCode((void *)0x40407d, shift);
}

int mainDatClass::mainStep(){
	#if debug_mode_mainRoop
		WaitForSingleObject( hPrintMutex, INFINITE );
		cout << "debug : mainStep()" << endl;
		ReleaseMutex( hPrintMutex );
	#endif

	UnRockTime();

	DWORD deInfo;
	DWORD gameTime;
	DWORD gameTimeNoCast = 0;
	WORD InputA;
	WORD InputB = 0;
	DWORD pauseFlg;
	DWORD FlgA;
	DWORD FlgB;
	BYTE  sendBufBody[40];
	WORD* sendBuf = (WORD *)&sendBufBody[6];
	BYTE  castBufBody[40];
	WORD* castBufA = (WORD *)&castBufBody[6];
	WORD* castBufB = (WORD *)&castBufBody[18];
	memset( sendBufBody, 0, sizeof( sendBufBody ) );
	memset( castBufBody, 0, sizeof( castBufBody ) );
	int diInputA;
	int diInputB;
	WORD fastFlg = 0;
	HWND hWnd = FindProcess();

	myInfo.phase = phase_battle;
	dataInfo.phase = phase_battle;

	BYTE sessionNo;
	BYTE sessionID;
	
	//要検討
	if( myInfo.terminalMode == mode_root || myInfo.terminalMode == mode_debug || myInfo.terminalMode == mode_broadcast ){
		sessionNo = myInfo.sessionNo;
		sessionID = myInfo.sessionID;
	}else if( myInfo.terminalMode == mode_branch || myInfo.terminalMode == mode_subbranch ){
		sessionNo = dataInfo.sessionNo;
		sessionID = dataInfo.sessionID;
	}

	sendBufBody[0] = sessionNo;
	sendBufBody[1] = sessionID;

	castBufBody[0] = sessionNo;
	castBufBody[1] = sessionID;

	DWORD menuAddress;
	DWORD gamePhase;

	BYTE turboKey;


	//緩和
	DWORD lastGameTick = 0;

	DWORD reqTime = 0;
	DWORD roopCounterReq = 0;
	//DWORD roopCounterKey = 0;
	DWORD roopCounterSend = 0;

	//先読み実験
	DWORD preReqTime = 0;

	//sync
	DWORD HPA;
	DWORD HPB;
	DWORD XA;	//* 64 and float->DWORD
	DWORD XB;
	WORD deSyncShowFlg = 1;

	DWORD baseAddress;
	prevPhase=0;

	BYTE roundsA;
	BYTE roundsB;
	DWORD gameTimeStart;
	gameTimeStart = 0;
	ReadMemory( (void*)memLoc_gameTime, &gameTimeStart, 4 );
	gameTime = 0;

	lastCharacterA = myInfo.A.ID&0xf;
	lastCharacterB = myInfo.B.ID&0xf;
	
	for(;;){
		if( th075Roop( &deInfo ) ) return 0xF;
		if( !roopFlg ){
			if( fastFlg ) changeSpeed(16);
			UnRockTime();
			return 1;
		}
		if( deInfo == de_body ){
			//戦闘中の同期など
			ReadMemory( (void*)memLoc_gamePhase, &gamePhase, 4 );
			
			DWORD p1Data, p2Data;

			ReadMemory( (void*)memLoc_p1Data, &p1Data, 4 );
			ReadMemory( (void*)memLoc_p2Data, &p2Data, 4 );
			
			int isMatchPhase = 0;
			if (gamePhase != gamePhase_splash
			    && gamePhase != gamePhase_menu
			    && gamePhase != gamePhase_intro
			    && gamePhase != gamePhase_charsel
			    && gamePhase != gamePhase_loading
			    && gamePhase != gamePhase_loading2
			    && gamePhase != gamePhase_postmatch) {
				isMatchPhase = 1;
			}
			
			if (!p1Data || !p2Data) {
				isMatchPhase = 0;
			}

			if ( !isMatchPhase ) {
				// Round over.

				// AutoSave.
				//runAutoSave();

				// run Session Log

				roundsWonA += roundsA;
				roundsWonB += roundsB;

				if (roundsA > roundsB) {
					winsA++;
				} else if (roundsB > roundsA) {
					winsB++;
				}

				lastGameTime = gameTime/2;
				totalGameTime += lastGameTime;

				for (int i = 0; i < 2; ++i) {
					ostream *stream;
					if (i == 0) {
						if (roundShowFlg < 2) {
							continue;
						}
						stream = &cout;
					} else if (i == 1) {
						if (!sessionLogFlg || !sessionLogFile) {
							continue;
						}
						stream = &sessionLogFile;

						printDate(stream);
						*stream << ' ';
					}

					printTime(stream);
					*stream << ' ';

					if (roundsA > roundsB) {
						*stream << "Player 1 wins match ";
					} else if (roundsB > roundsA) {
						*stream << "Player 2 wins match ";
					} else {
						*stream << "Draw match ";
					}
					*stream << (int)roundsA << "-" << (int)roundsB << " : ";

					for (int i = 2; i <= 3; ++i) {
						const charInfoStruct *c;
						if (i == 2) {
							c = &myInfo.A;
						} else {
							c = &myInfo.B;
						}

						*stream << getCharacterName(c->ID);

						if (i == 2) {
							*stream << " vs ";
						} else {
							if (gameTime > 0) {
								*stream << " (" << dec << lastGameTime/60 << "s)" << endl;
							} else {
								*stream << endl;
							}
						}
					}
				}

				if (sessionLogFile) {
					sessionLogFile.flush();
				}

				if( lessCastFlg ){
					if( gameTimeNoCast >= 8 ){
						inputData.GetInputData( sessionNo, gameTimeNoCast - 8, 0xA, &castBufA[5] );
						inputData.GetInputData( sessionNo, gameTimeNoCast - 6, 0xA, &castBufA[4] );
						inputData.GetInputData( sessionNo, gameTimeNoCast - 4, 0xA, &castBufA[3] );
						inputData.GetInputData( sessionNo, gameTimeNoCast - 2, 0xA, &castBufA[2] );
						inputData.GetInputData( sessionNo, gameTimeNoCast, 0xA, &castBufA[1] );
						inputData.GetInputData( sessionNo, gameTimeNoCast + 2, 0xA, &castBufA[0] );
						inputData.GetInputData( sessionNo, gameTimeNoCast - 8, 0xB, &castBufB[5] );
						inputData.GetInputData( sessionNo, gameTimeNoCast - 6, 0xB, &castBufB[4] );
						inputData.GetInputData( sessionNo, gameTimeNoCast - 4, 0xB, &castBufB[3] );
						inputData.GetInputData( sessionNo, gameTimeNoCast - 2, 0xB, &castBufB[2] );
						inputData.GetInputData( sessionNo, gameTimeNoCast, 0xB, &castBufB[1] );
						inputData.GetInputData( sessionNo, gameTimeNoCast + 2, 0xB, &castBufB[0] );
						*(DWORD*)&castBufBody[2] = gameTimeNoCast + 2;
						SendCmd( dest_leaf, cmd_cast, castBufBody, 6 + 12 + 12 );
					}
				}

				if( myInfo.terminalMode == mode_branch || myInfo.terminalMode == mode_subbranch ){
					//観戦しててタイムアウトしたときはroopFlgが1のままのため
					if( !Root.sin_addr.s_addr ){
						roopFlg = 0;
					}
				}

				//要検討
				myInfo.phase = phase_default;
				dataInfo.phase = phase_default;

				UnRockTime();
				datA.SetInput( diInputA );
				datB.SetInput( diInputB );
				break;
			}

			pauseFlg = 0;

			ReadMemory( (void*)memLoc_gameTime, &gameTime, 4 );
			gameTime -= gameTimeStart;
			gameTime *= 2;
			
			lastGameTime = gameTime/2;
			
			//sync data
			if( gameTime > 100 ){
				float fXA, fXB;
				
				HPA = 0;
				HPB = 0;
				XA = 0;
				XB = 0;

				ReadMemory( (void*)(p1Data + 0x94) , &HPA , 2 );
				ReadMemory( (void*)(p1Data + 0x14) , &fXA , 4 );

				ReadMemory( (void*)(p2Data + 0x94) , &HPB , 2 );
				ReadMemory( (void*)(p2Data + 0x14) , &fXB , 4 );
				
				XA = (int)fXA;
				XB = (int)fXB;

				syncData.SetSyncDataHereA( gameTime, HPA, XA );
				syncData.SetSyncDataHereB( gameTime, HPB, XB );
			}

			ReadMemory( (void*)(p1Data + 0x3C), &roundsA, 1 );
			ReadMemory( (void*)(p2Data + 0x3C), &roundsB, 1 );

			if( gameTime > 800 && !(gameTime%20) ){
				int Res;
				Res = syncData.TestSyncData( gameTime - 200 );

				if( Res == 0xF ){
					if( deSyncShowFlg ){
						deSyncShowFlg = 0;

						syncData.Print( gameTime - 200 );

						cout << "ERROR : deSync" << endl;
						if( deSyncSoundFlg ){
							if( strcmp( nowDir, "fail" ) && strlen( nowDir ) < 180 ){
								char path[200];
								strcpy( path, nowDir );
								strcat( path, "\\desync.wav\0" );
								if( !_access( path, 0 ) ){
									PlaySound( path, NULL, SND_ASYNC );
								}else if( deSyncSoundFlg == 2 ){
									strcpy( path, nowDir );
									strcat( path, "\\sound.wav\0" );
									if( !_access( path, 0 ) ){
										PlaySound( path, NULL, SND_ASYNC );
									}
								}
							}
						}
//						roopFlg = 0;
					}
				}
			}


			if( (DWORD)( gameTime / 2 ) * 2 != gameTime ) gameTime--;
			if( myInfo.terminalMode == mode_root || myInfo.terminalMode == mode_broadcast || myInfo.terminalMode == mode_debug ){
				dataInfo.gameTime = gameTime;
				inputData.SetTime( sessionNo, gameTime );
			}
			myInfo.gameTime = gameTime;

			//diInput
			InputA = 0;
			InputB = 0;
			diInputA = datA.GetInput();
			diInputB = datB.GetInput();

			// let me chat on irc, dammit
			//if( diInputA == key_P ){
				if( datA.inputDeviceType == 0xFF ){
					if( hWnd != GetForegroundWindow() ){
						diInputA = 0;
					}
				}
			//}
			//if( diInputB == key_P ){
				if( datB.inputDeviceType == 0xFF ){
					if( hWnd != GetForegroundWindow() ){
						diInputB = 0;
					}
				}
			//}
			/*if( diInputA == key_P ){
				//if( gameTime <= 400 ){
					diInputA = 0;
				//}
			}
			if( diInputB == key_P ){
				//if( gameTime <= 400 ){
					diInputB = 0;
				//}
			}*/

			if( myInfo.terminalMode == mode_branch || myInfo.terminalMode == mode_subbranch ){
				//早送り
				if( rockFlg ){
					if( fastFlg ){
						changeSpeed(16);
						fastFlg = 0;
					}
				}else{
					if( fastFlg ){
						if( gameTime + 30 > inputData.GetTime( sessionNo ) ){
							changeSpeed(16);
							fastFlg = 0;
						}else if( gameTime + 120 < inputData.GetTime( sessionNo ) ){
							if (!noFastFlg) changeSpeed(17);
							fastFlg = 1;
						}
					}else{
						if( gameTime + 120 < inputData.GetTime( sessionNo ) ){
							if (!noFastFlg) changeSpeed(17);
							fastFlg = 1;
						}else if( gameTime + 60 < inputData.GetTime( sessionNo ) ){
							if (!noFastFlg) changeSpeed(16);
							fastFlg = 1;
						}
					}
				}
			}

			//先読み
			//要検討
			if( myInfo.terminalMode == mode_branch || myInfo.terminalMode == mode_subbranch ){
				if( gameTime >= preReqTime + 60 ){
					if( inputData.GetTime( sessionNo ) >= gameTime + 300 ){
						if( !inputData.GetInputDataA( sessionNo, gameTime + 2, NULL ) && !inputData.GetInputDataB( sessionNo, gameTime + 2, NULL ) ){
							if( inputData.GetInputDataA( sessionNo, gameTime + 62, NULL ) && inputData.GetInputDataB( sessionNo, gameTime + 62, NULL ) ){
								//送信
								if( zlibFlg ){
									BYTE data[7];
									data[0] = sessionNo;
									data[1] = sessionID;
									*(DWORD*)&data[2] = gameTime + 62;
									data[6] = 0;	//仕様を拡張するため
									SendCmd( dest_root, cmd_inputdata_req, data, 7 );
								}else{
									BYTE data[6];
									data[0] = sessionNo;
									data[1] = sessionID;
									*(DWORD*)&data[2] = gameTime + 62;
									SendCmd( dest_root, cmd_inputdata_req, data, 6 );
								}

								preReqTime = gameTime;

//								cout << "debug : preReqTime " << preReqTime << endl;
							}
						}
					}
				}
			}

			//bodyInput
			if( gameTime <= 30 ){
				diInputA = 0;
				diInputB = 0;
			}

			if (keystate[KEY_NOFAST_TOGGLE] == 1) {
				noFastFlg = !noFastFlg;
				if (noFastFlg) {
					cout << "setting : noFast ON" << endl;
				} else {
					cout << "setting : noFast OFF" << endl;
				}

				if (fastFlg) {
					if (noFastFlg) {
						changeSpeed(16);
					} else {
						changeSpeed(17);
					}
				}
			}

			if( myInfo.terminalMode == mode_root || myInfo.terminalMode == mode_debug ){
				if( myInfo.terminalMode == mode_debug ) { //&& boosterFlg ){
					inputData.SetInputData( sessionNo, gameTime + 2, 0xB, 0 ); // boosterInput );
				}

				if( inputData.GetInputData( sessionNo, gameTime + delayTime + 2, myInfo.playerSide, NULL ) ){
					inputData.SetInputData( sessionNo, gameTime + delayTime + 2, myInfo.playerSide, diInputA );

					//データ送信
					if( gameTime + delayTime - 8 >= 0 ){
						inputData.GetInputData( sessionNo, gameTime + delayTime - 8, myInfo.playerSide, &sendBuf[5] );
					}else{
						sendBuf[5] = 0;
					}
					if( gameTime + delayTime - 6 >= 0 ){
						inputData.GetInputData( sessionNo, gameTime + delayTime - 6, myInfo.playerSide, &sendBuf[4] );
					}else{
						sendBuf[4] = 0;
					}
					if( gameTime + delayTime - 4 >= 0 ){
						inputData.GetInputData( sessionNo, gameTime + delayTime - 4, myInfo.playerSide, &sendBuf[3] );
					}else{
						sendBuf[3] = 0;
					}
					if( gameTime + delayTime - 2 >= 0 ){
						inputData.GetInputData( sessionNo, gameTime + delayTime - 2, myInfo.playerSide, &sendBuf[2] );
					}else{
						sendBuf[2] = 0;
					}
					if( gameTime + delayTime >= 0 ){
						inputData.GetInputData( sessionNo, gameTime + delayTime , myInfo.playerSide, &sendBuf[1] );
					}else{
						sendBuf[1] = 0;
					}
					inputData.GetInputData( sessionNo, gameTime + delayTime + 2, myInfo.playerSide, &sendBuf[0] );

					*(DWORD*)&sendBufBody[2] = gameTime + delayTime + 2;

					if( myInfo.terminalMode == mode_debug) { // && boosterFlg ){
						//none
						//vs th075Booster
					}else{
						if( gameTime%20 ){
							SendCmd( dest_away, cmd_sendinput, sendBufBody, 6 + 12 );
						}else{
							//send with syncData
//							cout << "debug : send syncData" << endl;
							*(DWORD*)&sendBufBody[20] = gameTime;
							*(DWORD*)&sendBufBody[24] = HPA;
							*(DWORD*)&sendBufBody[28] = XA;
							*(DWORD*)&sendBufBody[32] = HPB;
							*(DWORD*)&sendBufBody[36] = XB;

							SendCmd( dest_away, cmd_sendinput, sendBufBody, 40 );
						}
					}
				}
			}else if( myInfo.terminalMode == mode_broadcast ){
				if( inputData.GetInputData( sessionNo, gameTime + delayTimeA + 2, 0xA, NULL ) ){
					inputData.SetInputData( sessionNo, gameTime + delayTimeA + 2, 0xA, diInputA );
				}
				if( inputData.GetInputData( sessionNo, gameTime + delayTimeB + 2, 0xB, NULL ) ){
					inputData.SetInputData( sessionNo, gameTime + delayTimeB + 2, 0xB, diInputB );
				}
			}


			//if( !roopCounterKey ){
				//戦闘終了後に自動で進む
				/*
				if( myInfo.terminalMode == mode_branch || myInfo.terminalMode == mode_subbranch || myInfo.terminalMode == mode_debug ){
					if( GetKeyState( 219 ) < 0 ){	//[
						if( autoNextFlg ){
							if( hWnd == GetForegroundWindow() ){
								cout << "debug : AutoNext OFF" << endl;
								autoNextFlg = 0;
							}
						}
					}else if( GetKeyState( 221 ) < 0 ){	//]
						if( !autoNextFlg ){
							if( hWnd == GetForegroundWindow() ){
								cout << "debug : AutoNext ON" << endl;
								autoNextFlg = 1;
							}
						}
					}
				}
				 */

				//delayTime変更
				if( myInfo.terminalMode == mode_root || myInfo.terminalMode == mode_debug ){
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
								if( delayTime < delayTimeNew ){
									//データの空きを埋める必要がある
									WORD delayTimeStep;
									for(delayTimeStep = delayTime; delayTimeStep <= delayTimeNew; delayTimeStep += 2){
										if( inputData.GetInputData( sessionNo, gameTime + delayTimeStep + 2, myInfo.playerSide, NULL ) ){
											inputData.SetInputData( sessionNo, gameTime + delayTimeStep + 2, myInfo.playerSide, diInputA );
										}
									}
									delayTime = delayTimeNew;

									//データ送信
									if( gameTime + delayTime - 8 >= 0 ){
										inputData.GetInputData( sessionNo, gameTime + delayTime - 8, myInfo.playerSide, &sendBuf[5] );
									}else{
										sendBuf[5] = 0;
									}
									if( gameTime + delayTime - 6 >= 0 ){
										inputData.GetInputData( sessionNo, gameTime + delayTime - 6, myInfo.playerSide, &sendBuf[4] );
									}else{
										sendBuf[4] = 0;
									}
									if( gameTime + delayTime - 4 >= 0 ){
										inputData.GetInputData( sessionNo, gameTime + delayTime - 4, myInfo.playerSide, &sendBuf[3] );
									}else{
										sendBuf[3] = 0;
									}
									if( gameTime + delayTime - 2 >= 0 ){
										inputData.GetInputData( sessionNo, gameTime + delayTime - 2, myInfo.playerSide, &sendBuf[2] );
									}else{
										sendBuf[2] = 0;
									}
									if( gameTime + delayTime >= 0 ){
										inputData.GetInputData( sessionNo, gameTime + delayTime , myInfo.playerSide, &sendBuf[1] );
									}else{
										sendBuf[1] = 0;
									}
									inputData.GetInputData( sessionNo, gameTime + delayTime + 2, myInfo.playerSide, &sendBuf[0] );

									*(DWORD*)&sendBufBody[2] = gameTime + delayTime + 2;
									SendCmd( dest_away, cmd_sendinput, sendBufBody, 6 + 12 );

								}else{
									//none
									delayTime = delayTimeNew;
								}
								cout << "debug : Buffer Margin ( delayTime ) " << delayTime / 2 << endl;
							}
						}
					}
				}
			//}
			//roopCounterKey++;
			//if( roopCounterKey > 5 ) roopCounterKey = 0;

			if( pauseFlg ){
				//ポーズ中
				//ポーズから復帰するとき進んでしまうが
				//UnRockしないと次の処理に行かないためしょうがない
				UnRockTime();
				if( fastFlg ){
					changeSpeed(16);
					fastFlg = 0;
				}


				//要検討
			}else if( rockFlg ){
				//ロック中
				if( gameTime <= 30 ){
					UnRockTime();
				}else{
					if( diInputA == 0xFFFF || diInputB == 0xFFFF ){	//0xFF == key_P
						//ポーズにする
						//フォアグラウンドのときだけ入力
						//要検討
						//余分な入力が入る余地がある
						UnRockTime();
					}else{
						if( !inputData.GetInputDataA( sessionNo, gameTime + 2, NULL ) && !inputData.GetInputDataB( sessionNo, gameTime + 2, NULL ) ){
							//通常へ
							UnRockTime();
						}else{
							//データが届いていないとき
							if( myInfo.terminalMode == mode_root || myInfo.terminalMode == mode_debug){
								BYTE mySide = 0;
								BYTE enSide = 0;
								if( myInfo.playerSide == 0xA ){
									mySide = 0xA;
									enSide = 0xB;
								}else if( myInfo.playerSide == 0xB ){
									mySide = 0xB;
									enSide = 0xA;
								}
								if( mySide ){
									if( inputData.GetInputData( sessionNo, gameTime + 2, enSide, NULL ) ){
										roopCounterSend++;
										if( roopCounterSend > 3 ){
											BYTE data[6];
											data[0] = sessionNo;
											data[1] = sessionID;
											*(DWORD*)&data[2] = gameTime + 2;
											SendCmd( dest_away, cmd_input_req, data, 6 );

											roopCounterSend = 0;
										}
									}
									if( inputData.GetInputData( sessionNo, gameTime + 2, mySide, NULL ) ){
										//破綻への対処
										cout << "ERROR : recover" << endl;
										inputData.SetInputData( sessionNo, gameTime + 2, mySide, 0 );
									}
								}
							}else if( myInfo.terminalMode == mode_branch || myInfo.terminalMode == mode_subbranch ){

								//観戦しててタイムアウトしたときはroopFlgが1のままのため
								if( !Root.sin_addr.s_addr ){
									roopFlg = 0;
								}else{
									if( reqTime == gameTime + 2 ){
										if( roopCounterReq > 15 ){
											if( zlibFlg ){
												BYTE data[7];
												data[0] = sessionNo;
												data[1] = sessionID;
												*(DWORD*)&data[2] = gameTime + 2;
												data[6] = 0;	//仕様を拡張するため
												SendCmd( dest_root, cmd_inputdata_req, data, 7 );
											}else{
												BYTE data[6];
												data[0] = sessionNo;
												data[1] = sessionID;
												*(DWORD*)&data[2] = gameTime + 2;
												SendCmd( dest_root, cmd_inputdata_req, data, 6 );
											}

											reqTime = gameTime + 2;
											roopCounterReq = 0;


//											cout << "debug : cmd_inputdata_req " << gameTime + 2 << endl;
										}
										roopCounterReq++;
									}else{
										//応急措置
										reqTime = gameTime + 2;
									}
								}
							}else if( myInfo.terminalMode == mode_broadcast ){
								//破綻への対処
								inputData.SetInputDataA( sessionNo, gameTime + 2, diInputA );
								inputData.SetInputDataB( sessionNo, gameTime + 2, diInputB );
							}
						}
					}
				}
			}else{
				if( diInputA == 0xFFFF || diInputB == 0xFFFF ){
					//none
				}else{
					if( !inputData.GetInputDataA( sessionNo, gameTime + 2, NULL ) && !inputData.GetInputDataB( sessionNo, gameTime + 2, NULL ) ){
						//none
					}else{
						if( gameTime > 30 ){
							if( perfectFreezeFlg ){
//								cout << "debug : PF" << endl;
								int Counter = 0;
								for(;;){
									if( !roopFlg ){
										if( fastFlg ) changeSpeed(16);
										UnRockTime();
										return 1;
									}
									Sleep(1);
									if( !inputData.GetInputDataA( sessionNo, gameTime + 2, NULL )
									 && !inputData.GetInputDataB( sessionNo, gameTime + 2, NULL ) ){
										break;
									}
									if( myInfo.terminalMode == mode_root || myInfo.terminalMode == mode_debug){
										if( Counter > 16 ){
											BYTE mySide = 0;
											BYTE enSide = 0;
											if( myInfo.playerSide == 0xA ){
												mySide = 0xA;
												enSide = 0xB;
											}else if( myInfo.playerSide == 0xB ){
												mySide = 0xB;
												enSide = 0xA;
											}
											if( mySide ){
												if( inputData.GetInputData( sessionNo, gameTime + 2, enSide, NULL ) ){
													roopCounterSend++;
													if( roopCounterSend > 3 ){
														BYTE data[6];
														data[0] = sessionNo;
														data[1] = sessionID;
														*(DWORD*)&data[2] = gameTime + 2;
														SendCmd( dest_away, cmd_input_req, data, 6 );

														roopCounterSend = 0;
													}
												}
												if( inputData.GetInputData( sessionNo, gameTime + 2, mySide, NULL ) ){
													//破綻への対処
													cout << "ERROR : recover" << endl;
													inputData.SetInputData( sessionNo, gameTime + 2, mySide, 0 );
												}
											}
											Counter = 0;
										}
									}else if( myInfo.terminalMode == mode_branch || myInfo.terminalMode == mode_subbranch ){
										if( Counter > 300 ){
											//観戦しててタイムアウトしたときはroopFlgが1のままのため
											if( !Root.sin_addr.s_addr ){
												roopFlg = 0;
											}else{
												if( zlibFlg ){
													BYTE data[7];
													data[0] = sessionNo;
													data[1] = sessionID;
													*(DWORD*)&data[2] = gameTime + 2;
													data[6] = 0;	//仕様を拡張するため
													SendCmd( dest_root, cmd_inputdata_req, data, 7 );
												}else{
													BYTE data[6];
													data[0] = sessionNo;
													data[1] = sessionID;
													*(DWORD*)&data[2] = gameTime + 2;
													SendCmd( dest_root, cmd_inputdata_req, data, 6 );
												}
											}
											Counter = 0;
										}
									}else if( myInfo.terminalMode == mode_broadcast ){
										//破綻への対処
										inputData.SetInputDataA( sessionNo, gameTime + 2, diInputA );
										inputData.SetInputDataB( sessionNo, gameTime + 2, diInputB );
										break;
									}
									Counter++;
								}
							}else{
								RockTime();
							}
						}else{
							//none
						}
					}
				}
			}

			if( rockFlg ){
				diInputA = 0;
				diInputB = 0;
			}

			//bodyの入力
			//datA.SetBodyInput( diInputA );
			//datB.SetBodyInput( diInputB );

		}else if( deInfo == de_char ){
			
			ReadMemory( (void*)memLoc_gameTime, &gameTime, 4 );

			gameTime -= gameTimeStart;

			gameTime *= 2;
			if( (DWORD)( gameTime / 2 ) * 2 != gameTime ) gameTime--;


			DWORD n =gameTime - lastGameTick;

			if (n > 2 && gameTime > 4) {
			    //cout << "Error: Dropped frame: " << dec << gameTime << " != " << dec << lastGameTick << endl;
			}
			
			if (n == 0) {
				continue;
			}

			lastGameTick = gameTime;

			//test
			roopCounterSend = 0;
			roopCounterReq = 0;

			//対処（ポーズの後など）
			if( gameTime > 30 ){
				if( inputData.GetInputData( sessionNo, gameTime + 2, 0xA, NULL ) ){
					WORD inputTemp;
					inputData.GetInputData( sessionNo, gameTime, 0xA, &inputTemp );
					inputData.SetInputData( sessionNo, gameTime + 2, 0xA, inputTemp );
					cout << "ERROR : inputData check" << endl;
				}
				if( inputData.GetInputData( sessionNo, gameTime + 2, 0xB, NULL ) ){
					WORD inputTemp;
					inputData.GetInputData( sessionNo, gameTime, 0xB, &inputTemp );
					inputData.SetInputData( sessionNo, gameTime + 2, 0xB, inputTemp );
					cout << "ERROR : inputData check" << endl;
				}
			}

			//観戦データ送信頻度減少
			//現状では環境が悪い
			if( !lessCastFlg || gameTime / 2 & 1 ){

				//発信
				//side A
				if( gameTime - 8 >= 0 ){
					inputData.GetInputData( sessionNo, gameTime - 8, 0xA, &castBufA[5] );
				}else{
					castBufA[5] = 0;
				}
				if( gameTime - 6 >= 0 ){
					inputData.GetInputData( sessionNo, gameTime - 6, 0xA, &castBufA[4] );
				}else{
					castBufA[4] = 0;
				}
				if( gameTime - 4 >= 0 ){
					inputData.GetInputData( sessionNo, gameTime - 4, 0xA, &castBufA[3] );
				}else{
					castBufA[3] = 0;
				}
				if( gameTime - 2 >= 0 ){
					inputData.GetInputData( sessionNo, gameTime - 2, 0xA, &castBufA[2] );
				}else{
					castBufA[2] = 0;
				}
				if( gameTime >= 0 ){
					inputData.GetInputData( sessionNo, gameTime, 0xA, &castBufA[1] );
				}else{
					castBufA[1] = 0;
				}
				inputData.GetInputData( sessionNo, gameTime + 2, 0xA, &castBufA[0] );

				//side B
				if( gameTime - 8 >= 0 ){
					inputData.GetInputData( sessionNo, gameTime - 8, 0xB, &castBufB[5] );
				}else{
					castBufB[5] = 0;
				}
				if( gameTime - 6 >= 0 ){
					inputData.GetInputData( sessionNo, gameTime - 6, 0xB, &castBufB[4] );
				}else{
					castBufB[4] = 0;
				}
				if( gameTime - 4 >= 0 ){
					inputData.GetInputData( sessionNo, gameTime - 4, 0xB, &castBufB[3] );
				}else{
					castBufB[3] = 0;
				}
				if( gameTime - 2 >= 0 ){
					inputData.GetInputData( sessionNo, gameTime - 2, 0xB, &castBufB[2] );
				}else{
					castBufB[2] = 0;
				}
				if( gameTime >= 0 ){
					inputData.GetInputData( sessionNo, gameTime, 0xB, &castBufB[1] );
				}else{
					castBufB[1] = 0;
				}
				inputData.GetInputData( sessionNo, gameTime + 2, 0xB, &castBufB[0] );
				*(DWORD*)&castBufBody[2] = gameTime + 2;
				SendCmd( dest_leaf, cmd_cast, castBufBody, 6 + (12 * 2) );
			}else{
				gameTimeNoCast = gameTime;
			}

			//入力
			if( gameTime <=30 ){
				InputA = 0;
				InputB = 0;
			}else{
				inputData.GetInputDataA( sessionNo, gameTime + 2, &InputA );
				inputData.GetInputDataB( sessionNo, gameTime + 2, &InputB );
			}
			
			datA.SetInput( InputA );
			datB.SetInput( InputB );
			datA.writeCharInput();
			datB.writeCharInput();
			datA.storeCharInput();
			datB.storeCharInput();
		}
	}
	if( fastFlg ) changeSpeed(16);
	UnRockTime();

	myInfo.A.phase = 0;
	myInfo.B.phase = 0;

	dataInfo.A.phase = 0;
	dataInfo.B.phase = 0;

	dataInfo.phase = phase_default;

	gameTime = 0;

	myRand++;

	return 0;
}

