#include "mainDatClass.h"
//#include "booster/boosterDatClass.h"
#include "zlib.h"
#include <float.h>
using namespace std;

unsigned __stdcall manageThread(void* Address){
	if( !Address ) return 1;
	mainDatClass* dat = (mainDatClass*)Address;
	#if debug_mode_thread
		WaitForSingleObject( dat->hPrintMutex, INFINITE );
		cout << "debug : manageThread start" << endl;
		ReleaseMutex( dat->hPrintMutex );
	#endif

	//タイムアウト管理など
	lastTimeStruct* lastTime = &(dat->lastTime);
	DWORD* nowTime = &(dat->nowTime);
	int size = sizeof(SOCKADDR_IN);
	DWORD Counter;

	//cmd_echoではないほうがよい

	for(;;){
		*nowTime = timeGetTime();

		if( dat->Away.sin_addr.s_addr && lastTime->Away ){
			if( dat->toughModeFlg ){
				//TIMEOUTを無視
				if( lastTime->Away + 8000 < *nowTime ){
					dat->SendCmdM( dest_away, cmd_continue );
				}
			}else if( lastTime->Away + timeout_time < *nowTime ){
				WaitForSingleObject( dat->hMutex, INFINITE );
				memset( &(dat->Away), 0, size );
				ReleaseMutex( dat->hMutex );

				cout << "ERROR : TIMEOUT ( Away )" << endl;

				dat->roopFlg = 0;
			}else if( lastTime->Away + 8000 < *nowTime ){
				dat->SendCmdM( dest_away, cmd_continue );
			}
		}
		if( dat->Root.sin_addr.s_addr && lastTime->Root ){
			if( dat->toughModeFlg ){
				//TIMEOUTを無視
				if( lastTime->Root + 8000 < *nowTime ){
					dat->SendCmdM( dest_root, cmd_continue );
				}
			}else if( lastTime->Root + timeout_time < *nowTime ){
				WaitForSingleObject( dat->hMutex, INFINITE );
				memset( &(dat->Root), 0, size );
				ReleaseMutex( dat->hMutex );

				//要検討
				if( dat->roopFlg ){
					cout << "ERROR : TIMEOUT ( Root )" << endl;
				}

				//観戦してて、データが残っているときの対処
				if( dat->myInfo.phase == phase_battle && ( dat->myInfo.terminalMode == mode_branch || dat->myInfo.terminalMode == mode_subbranch ) ){
					//none
				}else{
					dat->roopFlg = 0;
				}
			}else if( lastTime->Root + 8000 < *nowTime ){
				dat->SendCmdM( dest_root, cmd_continue );
			}
		}
		for( Counter = 0; Counter<4; Counter++ ){
			if( dat->Leaf[ Counter ].sin_addr.s_addr && lastTime->Leaf[ Counter ] ){
				if( lastTime->Leaf[ Counter ] + timeout_time < *nowTime ){
					WaitForSingleObject( dat->hMutex, INFINITE );
					memset( &(dat->Leaf[ Counter ]), 0, size );
					ReleaseMutex( dat->hMutex );
				}else if( lastTime->Leaf[ Counter ] + 4000 < *nowTime ){
					dat->SendCmdM( &(dat->Leaf[ Counter ]), cmd_continue );
				}
			}
		}
		if( dat->Access.sin_addr.s_addr && lastTime->Access ){
			if( dat->toughModeFlg ){
				//TIMEOUTを無視
			}else if( lastTime->Access + timeout_time < *nowTime ){
				WaitForSingleObject( dat->hMutex, INFINITE );
				memset( &(dat->Access), 0, size );
				ReleaseMutex( dat->hMutex );

				cout << "ERROR : TIMEOUT ( Access )" << endl;

			}else if( lastTime->Access + 4000 < *nowTime ){
				dat->SendCmdM( dest_access, cmd_continue );
			}
		}
		if( dat->Standby[0].sin_addr.s_addr ){
			if( lastTime->Standby[0] + 3000 < *nowTime ){
				WaitForSingleObject( dat->hMutex, INFINITE );
				memset( &(dat->Standby[0]), 0, size );
				memset( &(dat->Standby[1]), 0, size );
				ReleaseMutex( dat->hMutex );
			}
		}
		if( dat->Standby[1].sin_addr.s_addr ){
			if( lastTime->Standby[1] + 3000 < *nowTime ){
				WaitForSingleObject( dat->hMutex, INFINITE );
				memset( &(dat->Standby[0]), 0, size );
				memset( &(dat->Standby[1]), 0, size );
				ReleaseMutex( dat->hMutex );
			}
		}
		Sleep(500);
		if( !(dat->continueFlg) ) break;
	}

	#if debug_mode_thread
		WaitForSingleObject( dat->hPrintMutex, INFINITE );
		cout << "debug : manageThread end" << endl;
		ReleaseMutex( dat->hPrintMutex );
	#endif
	return 0;
}

unsigned __stdcall th075Thread(void* Address){
	mainDatClass* dat = (mainDatClass*)Address;
	if( !dat ) return 1;
	#if debug_mode_thread
		WaitForSingleObject( dat->hPrintMutex, INFINITE );
		cout << "debug : th075th start" << endl;
		ReleaseMutex( dat->hPrintMutex );
	#endif

	unsigned int cw;
	dat->deInitFlg = 0;
	{
		STARTUPINFO si;
		ZeroMemory( &si, sizeof(si) );
		si.cb = sizeof( si );

		ZeroMemory( &dat->pi, sizeof( dat->pi ) );

		dat->priorityFlg = 0;
		dat->windowModeFlg = 0;
		dat->stageLimitCancelFlg = 0;
		if( strcmp( dat->iniPath, "fail" ) ){
			//position
			if( GetPrivateProfileInt( "POSITION", "onoff", 0, dat->iniPath ) ){
				int x,y;

				x = GetPrivateProfileInt( "POSITION", "x", 0xFFFFFFF, dat->iniPath );
				y = GetPrivateProfileInt( "POSITION", "y", 0xFFFFFFF, dat->iniPath );
				if( x != 0xFFFFFFF && y != 0xFFFFFFF ){
					if( x >  2000 ) x = 0;
					if( x < -1000 ) x = 0;

					if( y >  2000 ) y = 0;
					if( y < -1000 ) y = 0;

					si.dwX = x;
					si.dwY = y;
					si.dwFlags |= STARTF_USEPOSITION;
				}
			}

			//priorityFlg
			dat->priorityFlg = (WORD)GetPrivateProfileInt( "MAIN", "priority", 0, dat->iniPath );

			//windowMode
			dat->windowModeFlg = (WORD)GetPrivateProfileInt( "MAIN", "windowMode", 1, dat->iniPath );

			//stageLimitCancel
			dat->stageLimitCancelFlg = (WORD)GetPrivateProfileInt( "MAIN", "stageLimitCancel", 0, dat->iniPath );
		}


		{
			char path[200];
			memset( path, 0, sizeof( path ) );

			if( strcmp( th075_path, "akatsukibk.exe" ) ){
				//path指定のときのため
				strcpy( path, th075_path );
			}else{
				if( strcmp( dat->nowDir, "fail" ) && strlen( dat->nowDir ) < 180 ){
					strcpy( path ,dat->nowDir );
					strcat( path, "\\akatsukibk.exe\0" );
				}else{
					strcpy( path, "fail\0" );
				}
			}

			if( strcmp( path, "fail" ) ){
				if( !CreateProcess( path, NULL, NULL, NULL, FALSE, DEBUG_PROCESS, NULL, NULL, &si, &(dat->pi) ) ){
					cout << "ERROR : akatsukibk start failed." << endl;
					cout << "Make sure ABKCaster is in the same folder as akatsukibk.exe." << endl;
					cout << "Also, stop any applications that use GameGuard." << endl;
					dat->hProcess = NULL;
					if( dat->hCheckEvent ){
						SetEvent(dat->hCheckEvent);
					}
					return 1;
				}
			}else{
				cout << "ERROR : ABK path set failed." <<endl;
			}
		}
		dat->hProcess = OpenProcess(PROCESS_ALL_ACCESS, false, dat->pi.dwProcessId);
		if (!dat->hProcess) {
			dat->hProcess = dat->pi.hProcess;
		}
		if( dat->hCheckEvent ){
			SetEvent(dat->hCheckEvent);
		}
	}


	//mainRoop
	dat->mainRoop();


	CloseHandle(dat->pi.hThread);
	dat->pi.hThread = NULL;

	CloseHandle(dat->pi.hProcess);
	dat->pi.hProcess = NULL;

	#if debug_mode_thread
		WaitForSingleObject( dat->hPrintMutex, INFINITE );
		cout << "debug : th075th end" << endl;
		ReleaseMutex( dat->hPrintMutex );
	#endif
	return 0;
}


unsigned __stdcall sendThread(void* Address){
	mainDatClass* dat = (mainDatClass*)Address;
	if( !dat ) return 1;

	#if debug_mode_thread
		WaitForSingleObject( dat->hPrintMutex, INFINITE );
		cout << "debug : sendThread start" << endl;
		ReleaseMutex( dat->hPrintMutex );
	#endif


	int Counter;
	sTaskClass* sTask;
	SOCKADDR_IN addr;
	int addrSize = sizeof(SOCKADDR_IN);
	while( dat->continueFlg ){
		for(Counter=0; Counter<50; Counter++){
			if(dat->sTask[Counter].Flg){
//				cout << "sTask." << Counter << " send" << endl;
				sTask = &( dat->sTask[Counter] );

				if( sTask->dest == dest_away ){
					WaitForSingleObject( dat->hMutex, INFINITE );
					addr = dat->Away;
					ReleaseMutex( dat->hMutex );
					if( addr.sin_addr.s_addr ){
						if(sTask->Flg == stask_data) sendto(dat->s, (const char*)sTask->data, sTask->size, 0, (SOCKADDR*)&addr, addrSize);
						if(sTask->Flg == stask_area) sendto(dat->s, (const char*)sTask->Address, sTask->size, 0, (SOCKADDR*)&addr, addrSize);
					}

				}else if( sTask->dest == dest_root ){
					WaitForSingleObject( dat->hMutex, INFINITE );
					addr = dat->Root;
					ReleaseMutex( dat->hMutex );
					if( addr.sin_addr.s_addr ){
						if(sTask->Flg == stask_data) sendto(dat->s, (const char*)sTask->data, sTask->size, 0, (SOCKADDR*)&addr, addrSize);
						if(sTask->Flg == stask_area) sendto(dat->s, (const char*)sTask->Address, sTask->size, 0, (SOCKADDR*)&addr, addrSize);
					}

				}else if( sTask->dest == dest_branch ){
					WaitForSingleObject( dat->hMutex, INFINITE );
					addr = dat->Branch;
					ReleaseMutex( dat->hMutex );
					if( addr.sin_addr.s_addr ){
						if(sTask->Flg == stask_data) sendto(dat->s, (const char*)sTask->data, sTask->size, 0, (SOCKADDR*)&addr, addrSize);
						if(sTask->Flg == stask_area) sendto(dat->s, (const char*)sTask->Address, sTask->size, 0, (SOCKADDR*)&addr, addrSize);
					}

				}else if( sTask->dest == dest_subbranch ){
					WaitForSingleObject( dat->hMutex, INFINITE );
					addr = dat->subBranch;
					ReleaseMutex( dat->hMutex );
					if( addr.sin_addr.s_addr ){
						if(sTask->Flg == stask_data) sendto(dat->s, (const char*)sTask->data, sTask->size, 0, (SOCKADDR*)&addr, addrSize);
						if(sTask->Flg == stask_area) sendto(dat->s, (const char*)sTask->Address, sTask->size, 0, (SOCKADDR*)&addr, addrSize);
					}

				}else if( sTask->dest == dest_leaf ){
					int leafCounter;
					for(leafCounter = 0; leafCounter<4; leafCounter++){
						WaitForSingleObject( dat->hMutex, INFINITE );
						addr = dat->Leaf[ leafCounter ];
						ReleaseMutex( dat->hMutex );
						if( addr.sin_addr.s_addr ){
							if(sTask->Flg == stask_data) sendto(dat->s, (const char*)sTask->data, sTask->size, 0, (SOCKADDR*)&addr, addrSize);
							if(sTask->Flg == stask_area) sendto(dat->s, (const char*)sTask->Address, sTask->size, 0, (SOCKADDR*)&addr, addrSize);
						}
					}

				}else if( sTask->dest == dest_addr ){
//					addr = sTask->addr;
					if( sTask->addr.sin_addr.s_addr ){
						if(sTask->Flg == stask_data) sendto(dat->s, (const char*)sTask->data, sTask->size, 0, (SOCKADDR*)&(sTask->addr), addrSize);
						if(sTask->Flg == stask_area) sendto(dat->s, (const char*)sTask->Address, sTask->size, 0, (SOCKADDR*)&(sTask->addr), addrSize);
					}
				}else if( sTask->dest == dest_access ){
					WaitForSingleObject( dat->hMutex, INFINITE );
					addr = dat->Access;
					ReleaseMutex( dat->hMutex );
					if( addr.sin_addr.s_addr ){
						if(sTask->Flg == stask_data) sendto(dat->s, (const char*)sTask->data, sTask->size, 0, (SOCKADDR*)&addr, addrSize);
						if(sTask->Flg == stask_area) sendto(dat->s, (const char*)sTask->Address, sTask->size, 0, (SOCKADDR*)&addr, addrSize);
					}
				}
				sTask->Flg = 0;
			}
		}
		WaitForSingleObject( dat->hSendEvent, INFINITE );
	}

	#if debug_mode_thread
		WaitForSingleObject( dat->hPrintMutex, INFINITE );
		cout << "debug : sendThread end" << endl;
		ReleaseMutex( dat->hPrintMutex );
	#endif
	return 0;
}


//ローカルIPを弾く
//未テスト
//127.0.0.1→7F 00 00 01
int TestIP( DWORD IP ){
	if( !IP ) return 1;
	BYTE* ip = (BYTE*)&IP;

//	cout << (WORD)ip[0] << (WORD)ip[1] << (WORD)ip[2] << (WORD)ip[3] << endl;

	//10.0.0.0～10.255.255.255
	if( ip[0] == 10 ) return 1;

	//172.16.0.0～172.31.255.255
	if( ip[0] == 172 && ip[1] >= 16 && ip[1] <= 31 ) return 1;

	//192.168.0.0～192.168.255.255
	if( ip[0] == 192 && ip[1] == 168 ) return 1;

	//127.0.0.1
	if( IP == 0x100007F ) return 1;

	return 0;
}


//addrの中身を弄るときはMUTEXを取得する
unsigned __stdcall recvThread(void* Address){
	mainDatClass* dat = (mainDatClass*)Address;
	if(!dat) return 1;

	#if debug_mode_thread
		WaitForSingleObject( dat->hPrintMutex, INFINITE );
		cout << "debug : rcvThread start" << endl;
		ReleaseMutex( dat->hPrintMutex );
	#endif



	//バージョン情報もあったほうがいい
	int	addrSize = sizeof(SOCKADDR_IN);
	int	size;
	SOCKADDR_IN addr;

	BYTE data[1024];

	BYTE	recvBuf[ recv_buf_size ];
	BYTE* recvData = &recvBuf[5];
	BYTE* command = &recvBuf[4];
	lastTimeStruct* lastTime = &(dat->lastTime);
	DWORD* nowTime = &(dat->nowTime);

	DWORD reqTime[4][2];
	memset( reqTime, 0, sizeof(reqTime) );

	DWORD transTime = 0;
	DWORD readyTime = 0;

	//z_buf
	BYTE* zBuf = (BYTE*)malloc( z_buf_size );
	if( !zBuf ){
		return 1;
	}

	//cmd_exit
	DWORD cmdExitTime[20][2];
	memset( cmdExitTime, 0, sizeof( cmdExitTime ) );

	while( dat->continueFlg ){
		size = recvfrom( dat->s, (char*)recvBuf, recv_buf_size, 0, (SOCKADDR*)&addr, &addrSize);

		if( size < 0) {
//			cout << "Error at recvfrom()" << endl;
			//相手のポートが受信状態でないとき等もエラーになる

			//要検討
			if( addr.sin_addr.s_addr == 0 ){
//				cout << "SOCKET : END" << endl;
//				dat->roopFlg = 0;

				//手違いのときのための対処
				if( dat->continueFlg ){
					Sleep(2);
				}

			}else if( addr.sin_addr.s_addr == dat->Root.sin_addr.s_addr ){
				if( dat->toughModeFlg ){
					//エラーを無視する
				}else{
					WaitForSingleObject( dat->hMutex, INFINITE );
					memset( &(dat->Root), 0, sizeof(SOCKADDR_IN));
					ReleaseMutex( dat->hMutex );

					cout << "ERROR : RecvFrom ( Root )" << endl;
					dat->roopFlg = 0;
				}
			}else if( addr.sin_addr.s_addr == dat->Away.sin_addr.s_addr ){
				if( dat->toughModeFlg ){
					//エラーを無視する
				}else{
					WaitForSingleObject( dat->hMutex, INFINITE );
					memset( &(dat->Away), 0, sizeof(SOCKADDR_IN));
					ReleaseMutex( dat->hMutex );

					cout << "ERROR : RecvFrom ( Away )" << endl;
					dat->roopFlg = 0;
				}
			}else if( addr.sin_addr.s_addr == dat->Leaf[0].sin_addr.s_addr ){
				WaitForSingleObject( dat->hMutex, INFINITE );
				memset( &(dat->Leaf[0]), 0, sizeof(SOCKADDR_IN));
				ReleaseMutex( dat->hMutex );
			}else if( addr.sin_addr.s_addr == dat->Leaf[1].sin_addr.s_addr ){
				WaitForSingleObject( dat->hMutex, INFINITE );
				memset( &(dat->Leaf[1]), 0, sizeof(SOCKADDR_IN));
				ReleaseMutex( dat->hMutex );
			}else if( addr.sin_addr.s_addr == dat->Leaf[2].sin_addr.s_addr ){
				WaitForSingleObject( dat->hMutex, INFINITE );
				memset( &(dat->Leaf[2]), 0, sizeof(SOCKADDR_IN));
				ReleaseMutex( dat->hMutex );
			}else if( addr.sin_addr.s_addr == dat->Leaf[3].sin_addr.s_addr ){
				WaitForSingleObject( dat->hMutex, INFINITE );
				memset( &(dat->Leaf[3]), 0, sizeof(SOCKADDR_IN));
				ReleaseMutex( dat->hMutex );
			}else if( addr.sin_addr.s_addr == dat->Access.sin_addr.s_addr ){
				if( dat->toughModeFlg ){
					//エラーを無視する
				}else{
					WaitForSingleObject( dat->hMutex, INFINITE );
					memset( &(dat->Access), 0, sizeof(SOCKADDR_IN));
					ReleaseMutex( dat->hMutex );

					cout << "ERROR : RecvFrom ( Access )" << endl;
				}
			}
		}else{
			if(size > recv_buf_size) size = 0;
			if( size ){
				//要検討
//				if(size > 100) size = 100;
//				cout << "recv" << endl;

				//debug
//				if( TestIP( addr.sin_addr.s_addr ) ) cout << "debug : Local IP" << endl;


				if( recvBuf[0] == cmd_version && recvBuf[1] == cmd_space_1 && recvBuf[2] == cmd_space_2 && recvBuf[3] == cmd_space_3 ){
					if( addr.sin_addr.s_addr == dat->Away.sin_addr.s_addr){
						//対戦相手からの通信
						if( *command != cmd_exit ){
							lastTime->Away = *nowTime;
						}

						switch( *command ){
						case cmd_exit :
							dat->roopFlg = 0;
							//cout << "Received cmd_exit." << endl;
							break;
						case cmd_continue :
							dat->SendCmdR( dest_away, res_continue );
							break;
						case cmd_echo :
							dat->SendCmdR( dest_away, res_echo );
							break;
						case res_echo :
							dat->echoFlg.Away = 1;
							break;
						case res_access :
							dat->accessFlg = recvData[0];
							if (size >= 11 && !memcmp(&recvData[1], mbcaster_id, 5)) {
								dat->remoteProtocolVersion = recvData[6];
							}
							if (dat->remoteProtocolVersion >= 1 && recvData[7] > 0 && !dat->namesLocked) {
								int l = recvData[8];
								if (l >= 0 && l <= 20) {
									memcpy(dat->p2PlayerName, &recvData[9], l);
									dat->p2PlayerName[l] = 0;
									dat->cleanString(dat->p2PlayerName, 1);
								}
							}
							break;
						case cmd_access :
							data[0] = status_ok;
							dat->SendCmdR( dest_away, res_access, data, 1 );
							dat->accessFlg = status_ok;
							break;
						case cmd_sendinput :
							//戦闘中の入力データを格納する
							{
								BYTE enSide = 0;
								if( dat->myInfo.playerSide == 0xA ){
									enSide = 0xB;
								}else if( dat->myInfo.playerSide == 0xB ){
									enSide = 0xA;
								}
								if( enSide ){
									DWORD gameTimeTemp = *(DWORD*)&recvData[2];
									WORD *inputBuf = (WORD *)&recvData[6];
									if( gameTimeTemp < 30 ){
										dat->inputData.SetInputData( recvData[0], gameTimeTemp , enSide, inputBuf[0] );
									}else{
										dat->inputData.SetInputData( recvData[0], gameTimeTemp     , enSide, inputBuf[0] );
										dat->inputData.SetInputData( recvData[0], gameTimeTemp - 2 , enSide, inputBuf[1] );
										dat->inputData.SetInputData( recvData[0], gameTimeTemp - 4 , enSide, inputBuf[2] );
										dat->inputData.SetInputData( recvData[0], gameTimeTemp - 6 , enSide, inputBuf[3] );
										dat->inputData.SetInputData( recvData[0], gameTimeTemp - 8 , enSide, inputBuf[4] );
										dat->inputData.SetInputData( recvData[0], gameTimeTemp - 10 , enSide, inputBuf[5] );
									}

									gameTimeTemp = *(DWORD*)&recvData[20];
									if( size > 18 && !( gameTimeTemp%20 ) ){
										//set syncData
//										cout << "debug : set syncData Away" << endl;
										dat->syncData.SetSyncDataAwayA( gameTimeTemp, *(DWORD*)&recvData[24], *(DWORD*)&recvData[28] );
										dat->syncData.SetSyncDataAwayB( gameTimeTemp, *(DWORD*)&recvData[32], *(DWORD*)&recvData[36] );
									}
								}
							}
							break;
						case res_input_req :
							//戦闘中の入力データを格納する
							{
								BYTE enSide = 0;
								if( dat->myInfo.playerSide == 0xA ){
									enSide = 0xB;
								}else if( dat->myInfo.playerSide == 0xB ){
									enSide = 0xA;
								}
								if( enSide ){
									dat->inputData.SetInputData( recvData[0], *(DWORD*)&recvData[2] , enSide, *(WORD*)&recvData[6] );
								}
							}
							break;
						case cmd_input_req :
							//入力を返信する
							{
								BYTE mySide = dat->myInfo.playerSide;
								if( !( dat->inputData.GetInputData( recvData[0], *(DWORD*)&recvData[2], mySide, (WORD *)&data[6] ) ) ){
									data[0] = recvData[0];
									data[1] = recvData[1];
									*(DWORD*)&data[2] = *(DWORD*)&recvData[2];
									dat->SendCmdR( dest_away, res_input_req, data, 8 );
								}
							}
							break;
						case res_time :
							//相手の時間を格納
							dat->enInfo.gameTime = *(DWORD*)recvData;
							break;
						case cmd_time :
							//時間を送る
							dat->SendCmdR( dest_away, res_time, &( dat->myInfo.gameTime ), 4 );
							break;
						case res_gameInfo :
							//メニューの情報を格納
							memcpy( &(dat->enInfo), recvData, sizeof(dat->enInfo) );
							break;
						case cmd_gameInfo :
							//メニューの情報を送る
							dat->SendCmdR( dest_away, res_gameInfo, &(dat->myInfo), sizeof(dat->myInfo) );
							break;
						case cmd_delay :
							*(WORD*)&data[0] = dat->delayTime;
							*(WORD*)&data[2] = (WORD)(dat->delay * 10);
							dat->SendCmdR( dest_away, res_delay, data, 4 );
							break;
						case res_delay :
							//delayObsも表示させたい
							dat->delayTime = *(WORD*)recvData;
							dat->delay = ( (float)*(WORD*)&recvData[2] ) / 10;
							break;
						case cmd_delayobs :
							dat->SendCmdR( dest_away, res_delayobs, recvData, 4 );
							break;
						case res_delayobs :
							if( dat->delayTimeObsNo < 5 ){
								DWORD timeTemp = timeGetTime();
								dat->delayTimeObs[ dat->delayTimeObsNo ] = (float)(timeTemp - *(DWORD*)recvData );
								dat->delayTimeObsNo = dat->delayTimeObsNo + 1;
							}
							break;
						case cmd_rand :	//できれば細かいデータはまとめる方向で
							data[0] = dat->myRandNo;
							data[1] = dat->myRand;
							dat->SendCmdR( dest_away, res_rand, data, 2 );
							break;
						case res_rand :
							dat->enRandNo = recvData[0];
							dat->enRand = recvData[1];
							break;
						case cmd_playerside :
							dat->SendCmdR( dest_away, res_playerside, &( dat->myInfo.playerSide), 1 );
							break;
						case res_playerside :
							dat->enInfo.playerSide = recvData[0];
							break;
						case cmd_session :
							data[0] = dat->myInfo.sessionNo;
							data[1] = dat->myInfo.sessionID;
							data[2] = dat->myInfo.sessionIDNext;
							dat->SendCmdR( dest_away, res_session, data, 3 );
							break;
						case res_session :
							dat->enInfo.sessionNo = recvData[0];
							dat->enInfo.sessionID = recvData[1];
							dat->enInfo.sessionIDNext = recvData[2];
							break;
						case cmd_initflg :
							dat->SendCmdR( dest_away, res_initflg, &( dat->myInitFlg ), 1 );
							break;
						case res_initflg :
							dat->enInitFlg = recvData[0];
							break;
						case cmd_echoes :
							dat->SendCmdR( &addr, res_echoes, &(dat->myPort), 2 );
							break;
						case res_echoes :
							WaitForSingleObject( dat->hMutex, INFINITE );
							addr.sin_port = htons( *(WORD*)&recvData[ sizeof(SOCKADDR_IN) ] );
							dat->Away = addr;
							ReleaseMutex( dat->hMutex );
							dat->echoFlg.Away = 1;

//							cout << "debug : res_echoes" << endl;
							break;
						}
					}else if( addr.sin_addr.s_addr == dat->Leaf[0].sin_addr.s_addr
						|| addr.sin_addr.s_addr == dat->Leaf[1].sin_addr.s_addr
						|| addr.sin_addr.s_addr == dat->Leaf[2].sin_addr.s_addr
						|| addr.sin_addr.s_addr == dat->Leaf[3].sin_addr.s_addr
					){
						//Leaf
						BYTE Index = 0;
						if( addr.sin_addr.s_addr == dat->Leaf[0].sin_addr.s_addr ){
							Index = 0;
						}else if( addr.sin_addr.s_addr == dat->Leaf[1].sin_addr.s_addr ){
							Index = 1;
						}else if( addr.sin_addr.s_addr == dat->Leaf[2].sin_addr.s_addr ){
							Index = 2;
						}else if( addr.sin_addr.s_addr == dat->Leaf[3].sin_addr.s_addr ){
							Index = 3;
						}
						if( *command != cmd_exit ){
							lastTime->Leaf[ Index ] = *nowTime;
						}

						switch( *command ){
						case cmd_exit :
							WaitForSingleObject( dat->hMutex, INFINITE );
							memset( &(dat->Leaf[ Index ]), 0, sizeof(SOCKADDR_IN));
							ReleaseMutex( dat->hMutex );
							break;
						case cmd_continue :
							dat->SendCmdR( &addr, res_continue );
							break;
						case cmd_delayobs :
							dat->SendCmdR( &addr, res_delayobs, recvData, 4 );
							break;
						case res_echo :
							dat->echoFlg.Leaf[ Index ] = 1;
							break;
						case cmd_echo :
							dat->SendCmdR( &addr, res_echo );
							break;
						case cmd_access :
							data[0] = status_bad;
							dat->SendCmdR( &addr, res_access, data, 1 );
							break;
						case cmd_join :
							data[0] = status_ok;
							dat->SendCmdR( &addr, res_join, data, 1 );
							break;
						case cmd_inputdata_req :
							//両サイドの情報を送る
//							cout << "cmd_inputdata_req : " << (WORD)recvData[0] << ", " << *(DWORD*)&recvData[2] << endl;
							{
								DWORD gameTimeTemp = *(DWORD*)&recvData[2];
								//抑制テスト
								if( reqTime[ Index ][0] == gameTimeTemp && reqTime[ Index ][1] + 950 > *nowTime ){
									//抑制
									//要検討
								}else{
									if( !( dat->inputData.GetInputDataA( recvData[0], gameTimeTemp, (WORD *)&data[6] ) )
									 && !( dat->inputData.GetInputDataB( recvData[0], gameTimeTemp, (WORD *)&data[8] ) )
									){
										if( dat->zlibFlg ){
											//zlib

											if( size == 11 ){
												//old
												WORD* buf = dat->inputData.GetInputDataAddress( recvData[0], gameTimeTemp , 60);
												if( buf ){
													data[0] = recvData[0];
													data[1] = recvData[1];
													*(DWORD*)&data[2] = gameTimeTemp;
													memcpy( data + 6,  buf, 120 );
													dat->SendCmdR( &addr, res_inputdata_area, data, 126 );
												}else{
													data[0] = recvData[0];
													data[1] = recvData[1];
													*(DWORD*)&data[2] = gameTimeTemp;
													dat->SendCmdR( &addr, res_inputdata_req, data, 10 );
												}
											}else if( size == 12 && recvData[6] == 0 ){
												//new
												WORD* Buf;
												WORD dataSize = 0;

												Buf = dat->inputData.GetInputDataAddress( recvData[0], gameTimeTemp , 180);
												if( Buf && dat->inputData.GetTime( recvData[0] ) >= gameTimeTemp + 180 ){
													dataSize = 360;
												}else{
													Buf = dat->inputData.GetInputDataAddress( recvData[0], gameTimeTemp , 60);
													if( Buf && dat->inputData.GetTime( recvData[0] ) >= gameTimeTemp + 60){
														dataSize = 120;
													}else{
														data[0] = recvData[0];
														data[1] = recvData[1];
														*(DWORD*)&data[2] = gameTimeTemp;
														dat->SendCmdR( &addr, res_inputdata_req, data, 10 );
													}
												}

												if( Buf && dataSize ){
													//zlib使用
													unsigned long bufSize = 1014;
													if( compress( &data[10], &bufSize, (char *)Buf, dataSize ) == Z_OK ){
														//send
														data[0] = recvData[0];
														data[1] = recvData[1];
														*(DWORD*)&data[2] = gameTimeTemp;
														*(WORD*)&data[6] = bufSize;
														*(WORD*)&data[8] = dataSize;

														dat->SendCmdR( &addr, res_inputdata_z, data, 10 + bufSize );

														//debug
//														cout << "zSize : " << bufSize << endl;
													}else{
														//zlib不使用
														data[0] = recvData[0];
														data[1] = recvData[1];
														*(DWORD*)&data[2] = gameTimeTemp;
														memcpy( data + 6,  Buf, 120 );
														dat->SendCmdR( &addr, res_inputdata_area, data, 126 );
													}
												}
											}
										}else{
											//no zlib
											WORD* buf = dat->inputData.GetInputDataAddress( recvData[0], gameTimeTemp , 60);
											if( buf ){
												data[0] = recvData[0];
												data[1] = recvData[1];
												*(DWORD*)&data[2] = gameTimeTemp;
												memcpy( data + 6,  buf, 120 );
												dat->SendCmdR( &addr, res_inputdata_area, data, 126 );
											}else{
												data[0] = recvData[0];
												data[1] = recvData[1];
												*(DWORD*)&data[2] = gameTimeTemp;
												dat->SendCmdR( &addr, res_inputdata_req, data, 10 );
											}
										}
									}
									reqTime[ Index ][0] = gameTimeTemp;
									reqTime[ Index ][1] = *nowTime;
								}
							}
							break;
						case cmd_dataInfo :
							dat->SendCmdR( &addr, res_dataInfo, &(dat->dataInfo), sizeof(dat->dataInfo), &(dat->myInfo.terminalMode), 1 );
							break;
						case cmd_rand :	//できれば細かいデータはまとめる方向で
							data[0] = dat->myRandNo;
							data[1] = dat->myRand;
							dat->SendCmdR( &addr, res_rand, data, 2 );
							break;
						}
					}else if( addr.sin_addr.s_addr == dat->Root.sin_addr.s_addr ){
						//Rootからの通信
						if( *command != res_dataInfo && *command != cmd_exit ){
							lastTime->Root = *nowTime;
						}

						switch( *command ){
						case cmd_exit :
							WaitForSingleObject( dat->hMutex, INFINITE );
							memset( &(dat->Root), 0, sizeof(SOCKADDR_IN));
							ReleaseMutex( dat->hMutex );

							//観戦してて、データが残っているときの対処
							if( dat->myInfo.phase == phase_battle && ( dat->myInfo.terminalMode == mode_branch || dat->myInfo.terminalMode == mode_subbranch ) ){
								//none
							}else{
								dat->roopFlg = 0;
							}
							break;
						case cmd_addr_leaf :
//							cout << "debug : cmd_addr_leaf ( Root )" << endl;
							if( transTime + 200 < *nowTime ){
								//nowTimeは500刻み
								SOCKADDR_IN addrTemp = *(SOCKADDR_IN*)recvData;
								dat->SendCmdR( &addrTemp, cmd_echo );
								if( size > sizeof( gameInfoStruct ) + 6 ){
									if( *(WORD*)recvData[ sizeof( gameInfoStruct ) ] ){
										addrTemp.sin_port = htons( *(WORD*)recvData[ sizeof( gameInfoStruct ) ] );
										dat->SendCmdR( &addrTemp, cmd_seek_leaf );
									}
								}
								transTime = *nowTime;
							}
							break;
						case cmd_seek_leaf :
							WaitForSingleObject( dat->hMutex, INFINITE );
							dat->Root.sin_port = addr.sin_port;
							ReleaseMutex( dat->hMutex );
							break;
						case cmd_continue :
							dat->SendCmdR( dest_root, res_continue );
							break;
						case cmd_delayobs :
							dat->SendCmdR( dest_root, res_delayobs, recvData, 4 );
							break;
						case cmd_echo :
							dat->SendCmdR( dest_root, res_echo );
							break;
						case res_echo :
							dat->echoFlg.Root = 1;
							break;
						case res_dataInfo :
							//メニューの情報を格納
							memcpy( &(dat->dataInfo), recvData, sizeof(dat->dataInfo) );
							if( size > 37 ){
								dat->targetMode = recvData[ sizeof(dat->dataInfo) ];
							}

							dat->hasRemote = 0;
							if (size >= 43) {
								int n = sizeof(dat->dataInfo) + 1;
								if (!memcmp(recvData+n, mbcaster_id, 5)) {
									dat->remoteProtocolVersion = recvData[n+5];
									n += 6;

									if (recvData[n++] != 0) {
										dat->hasRemote = 1;
										memcpy(&dat->Remote, recvData+n, sizeof(dat->Remote));
										n += sizeof(dat->Remote);
									}

									if (!dat->namesLocked) {
										int c = recvData[n];
										n++;
										if (c < 3) {
											dat->nPlayers = c;
											for (int i = 1; i <= c; ++i) {
												int l = recvData[n++];
												if (l > 20) {
													dat->nPlayers = 0;
													break;
												}
												if (i == 1) {
													dat->p1PlayerName[l] = '\0';
													memcpy(dat->p1PlayerName, recvData+n, l);
													dat->cleanString(dat->p1PlayerName, 1);
												} else {
													dat->p2PlayerName[l] = '\0';
													memcpy(dat->p2PlayerName, recvData+n, l);
													dat->cleanString(dat->p2PlayerName, 1);
												}

												n += l;
											}
										}
									}
								}
							}

							//要検討
							//Leafへの伝達で使う（観戦中かどうか）
							if( dat->roopFlg ) dat->dataInfo.terminalMode = dat->myInfo.terminalMode;
							break;
						case res_join :
							dat->joinRes = recvData[0];
							break;
						case cmd_cast :
//							cout << "cmd_cast recv : " << (WORD)recvData[0] << ", " << *(DWORD*)&recvData[2] << endl;

							{
								DWORD gameTimeTemp = *(DWORD*)&recvData[2];
								//要検討
								dat->dataInfo.gameTime = gameTimeTemp;
								dat->inputData.SetTime( recvData[0], gameTimeTemp );

								WORD* castBufA = (WORD *)&recvData[6];
								WORD* castBufB = (WORD *)&recvData[6 + 12];

//								cout << "cast recv : " << gameTimeTemp << " : " << (WORD)castBufA[0] << " : " << (WORD)recvData[4] << endl;
								dat->inputData.SetInputDataA( recvData[0], gameTimeTemp, castBufA[0] );
								dat->inputData.SetInputDataB( recvData[0], gameTimeTemp, castBufB[0] );
								if( gameTimeTemp > 30 ){
									dat->inputData.SetInputDataA( recvData[0], gameTimeTemp - 2, castBufA[1] );
									dat->inputData.SetInputDataA( recvData[0], gameTimeTemp - 4, castBufA[2] );
									dat->inputData.SetInputDataA( recvData[0], gameTimeTemp - 6, castBufA[3] );
									dat->inputData.SetInputDataA( recvData[0], gameTimeTemp - 8, castBufA[4] );
									dat->inputData.SetInputDataA( recvData[0], gameTimeTemp - 10, castBufA[5] );

									dat->inputData.SetInputDataB( recvData[0], gameTimeTemp - 2, castBufB[1] );
									dat->inputData.SetInputDataB( recvData[0], gameTimeTemp - 4, castBufB[2] );
									dat->inputData.SetInputDataB( recvData[0], gameTimeTemp - 6, castBufB[3] );
									dat->inputData.SetInputDataB( recvData[0], gameTimeTemp - 8, castBufB[4] );
									dat->inputData.SetInputDataB( recvData[0], gameTimeTemp - 10, castBufB[5] );
								}
							}
							break;
						case res_inputdata_req :
							{
//								cout << "res_inputdata_req" << endl;
								DWORD gameTimeTemp = *(DWORD*)&recvData[2];
								dat->inputData.SetInputDataA( recvData[0], gameTimeTemp, *(WORD *)&recvData[6] );
								dat->inputData.SetInputDataB( recvData[0], gameTimeTemp, *(WORD *)&recvData[8] );
							}
							break;
						case res_inputdata_area :
							{
//								cout << "res_inputdata_area" << endl;
								DWORD gameTimeTemp = *(DWORD*)&recvData[2];
								dat->inputData.SetInputDataArea( recvData[0], gameTimeTemp, (WORD *)&recvData[6], 120 );
							}
							break;
						case res_inputdata_z :
							//実験
//							cout << "res_inputdata_z" << endl;
							if( dat->zlibFlg ){
								if( *(WORD*)&recvData[8] < z_buf_size ){
									if( *(WORD*)&recvData[6] < 1024 ){
										DWORD gameTimeTemp = *(DWORD*)&recvData[2];
										unsigned long bufSize = z_buf_size;
										if( uncompress( zBuf, &bufSize, &recvData[10], *(WORD*)&recvData[6]) == Z_OK ){
											if( bufSize == (DWORD)*(WORD*)&recvData[8] ){
												//debug
//												cout << "debug : res_inputdata_z size " << *(WORD*)&recvData[6] << " -> " << bufSize << endl;
												dat->inputData.SetInputDataArea( recvData[0], gameTimeTemp, (WORD *)zBuf, *(WORD*)&recvData[8] );
											}
										}
									}
								}
							}
							break;
						case cmd_addr_branch :
							//実験
							//要検討
//							cout << "debug : cmd_addr_branch ( Root )" << endl;
							WaitForSingleObject( dat->hMutex, INFINITE );
							dat->Root = *(SOCKADDR_IN*)recvData;
							ReleaseMutex( dat->hMutex );
							break;
						case res_rand :
							dat->enRandNo = recvData[0];
							dat->enRand = recvData[1];
							break;
						}
					}else if( addr.sin_addr.s_addr == dat->Access.sin_addr.s_addr ){
						//アクセス相手からの通信
						if( *command != cmd_exit ){
							lastTime->Access = *nowTime;
						}

						switch( *command ){
						case cmd_exit :
							WaitForSingleObject( dat->hMutex, INFINITE );
							memset( &(dat->Access), 0, sizeof(SOCKADDR_IN));
							ReleaseMutex( dat->hMutex );

							dat->roopFlg = 0;
							break;
						case cmd_testport :
							//TestPort
							{
								SOCKADDR_IN addrTemp;
								memset( &addrTemp, 0, sizeof(SOCKADDR_IN) );

								WaitForSingleObject( dat->hMutex, INFINITE );
								addrTemp.sin_family = AF_INET;
								addrTemp.sin_addr.s_addr = addr.sin_addr.s_addr;
								addrTemp.sin_port = htons( *(WORD*)recvData );
								ReleaseMutex( dat->hMutex );

								dat->SendCmdR( &addrTemp, res_testport );
							}
							break;
						case cmd_continue :
							dat->SendCmdR( dest_access, res_continue );
							break;
						case cmd_delayobs :
							dat->SendCmdR( dest_access, res_delayobs, recvData, 4 );
							break;
						case res_delayobs :
							if( dat->delayTimeObsNo < 5 ){
								DWORD timeTemp = timeGetTime();
								dat->delayTimeObs[ dat->delayTimeObsNo ] = (float)(timeTemp - *(DWORD*)recvData);
								dat->delayTimeObsNo = dat->delayTimeObsNo + 1;
							}
							break;
						case cmd_echo :
							dat->SendCmdR( dest_access, res_echo );
							break;
						case res_echo :
							dat->echoFlg.Access = 1;
							break;
						case cmd_access :
							if (size >= 11 && memcmp(recvData, mbcaster_id, 5)) {
								dat->remoteProtocolVersion = recvData[5];
							}
							if (dat->remoteProtocolVersion >= 1 && recvData[6] > 0 && size >= 13 && !dat->namesLocked) {
								int l = recvData[7];
								if (l > 0 && l <= 20 && (size-l)>=13) {
									dat->p2PlayerName[l] = '\0';
									memcpy(dat->p2PlayerName, &recvData[8], l);
									dat->cleanString(dat->p2PlayerName, 1);
								}
							}

							{
								dat->accessFlg = status_ok;
								data[0] = status_ok;
								memcpy(&data[1], mbcaster_id, 5);
								data[6] = mbcaster_protocol_id;

								int n = 7;
								int name_len = strlen(dat->myPlayerName);
								data[n++] = 1;
								data[n++] = name_len;
								memcpy(data+n, dat->myPlayerName, name_len);
								n += name_len;

								dat->SendCmdR( dest_access, res_access, data, n );
							}
							break;
						case res_access :
							if (!(size >= 11 && !memcmp(recvData+1, mbcaster_id, 5))) {
								WaitForSingleObject( dat->hPrintMutex, INFINITE );
								cout << "-- WARNING -- Remote version of ABKCaster is incompatible!" << endl
									<< "It is highly likely that you will be unable to play or spectate normally." << endl
									<< "Both players upgrading to the latest version is recommended." << endl;
								ReleaseMutex( dat->hPrintMutex );
							} else if (size >= 12) {
								dat->remoteProtocolVersion = recvData[6];
							}

							if (dat->remoteProtocolVersion >= 1 && size >= 14 && recvData[7] > 0 && !dat->namesLocked) {
								int l = recvData[8];
								if (l > 0 && l <= 20 && (size-l)>=14) {
									memcpy(dat->p2PlayerName, &recvData[9], l);
									dat->p2PlayerName[l] = 0;
									dat->cleanString(dat->p2PlayerName, 1);
								}
							}

							dat->accessFlg = recvData[0];
							break;
						case cmd_addr_branch :
							//実験
							//要検討
//							cout << "debug : cmd_addr_branch ( Access )" << endl;
							WaitForSingleObject( dat->hMutex, INFINITE );
							dat->Access = *(SOCKADDR_IN*)recvData;
							ReleaseMutex( dat->hMutex );
							break;
						case res_dataInfo :
							//メニューの情報を格納
							memcpy( &(dat->dataInfo), recvData, sizeof(dat->dataInfo) );
							if( size > 37 ){
								dat->targetMode = recvData[ sizeof(dat->dataInfo) ];
							}

							dat->hasRemote = 0;
							if (size >= 43) {
								int n = sizeof(dat->dataInfo) + 1;
								if (!memcmp(recvData+n, mbcaster_id, 5)) {
									dat->remoteProtocolVersion = recvData[n+5];
									n += 6;

									if (recvData[n++] != 0) {
										dat->hasRemote = 1;
										memcpy(&dat->Remote, recvData+n, sizeof(dat->Remote));
										n += sizeof(dat->Remote);
									}

									if (!dat->namesLocked) {
										int c = recvData[n];
										n++;
										if (c < 3) {
											dat->nPlayers = c;
											for (int i = 1; i <= c; ++i) {
												int l = recvData[n++];
												if (l > 20) {
													dat->nPlayers = 0;
													break;
												}
												if (i == 1) {
													dat->p1PlayerName[l] = '\0';
													memcpy(dat->p1PlayerName, recvData+n, l);
													dat->cleanString(dat->p1PlayerName, 1);
												} else {
													dat->p2PlayerName[l] = '\0';
													memcpy(dat->p2PlayerName, recvData+n, l);
													dat->cleanString(dat->p2PlayerName, 1);
												}

												n += l;
											}
										}
									}
								}
							}
							//要検討
							//Leafへの伝達で使う（観戦中かどうか）
							if( dat->roopFlg ) dat->dataInfo.terminalMode = dat->myInfo.terminalMode;
							break;
						case cmd_ready :
							//要検討
							if( readyTime + 200 < *nowTime ){
								//nowTimeは500刻み
								SOCKADDR_IN addrTemp = *(SOCKADDR_IN*)recvData;
								dat->SendCmdR( &addrTemp, res_continue );
								readyTime = *nowTime;
							}
							WaitForSingleObject( dat->hMutex, INFINITE );
							dat->Ready = *(SOCKADDR_IN*)recvData;
							dat->readyPort = *(WORD*)&recvData[ sizeof(SOCKADDR_IN) ];
							ReleaseMutex( dat->hMutex );
							break;
						case cmd_echoes :
							dat->SendCmdR( &addr, res_echoes, &(dat->myPort), 2 );
							break;
						case res_echoes :
							WaitForSingleObject( dat->hMutex, INFINITE );
							addr.sin_port = htons( *(WORD*)&recvData[ sizeof(SOCKADDR_IN) ] );
							dat->Access = addr;
							ReleaseMutex( dat->hMutex );
							dat->echoFlg.Access = 1;

//							cout << "debug : res_echoes" << endl;
							break;
						}
					}else{
						//Other
						switch( *command ){
						case cmd_testport :
							//TestPort
							{
								SOCKADDR_IN addrTemp;
								memset( &addrTemp, 0, sizeof(SOCKADDR_IN) );

								WaitForSingleObject( dat->hMutex, INFINITE );
								addrTemp.sin_family = AF_INET;
								addrTemp.sin_addr.s_addr = addr.sin_addr.s_addr;
								addrTemp.sin_port = htons( *(WORD*)recvData );
								ReleaseMutex( dat->hMutex );

								dat->SendCmdR( &addrTemp, res_testport );
							}
							break;
						case res_testport :
							//TestPort res
							dat->testPortFlg = 1;
							break;
						case cmd_standby :
							if( addr.sin_addr.s_addr == dat->Standby[0].sin_addr.s_addr ){
								lastTime->Standby[0] = *nowTime;
								if( dat->Standby[1].sin_addr.s_addr ){
									WaitForSingleObject( dat->hMutex, INFINITE );
									SOCKADDR_IN addrTemp = dat->Standby[1];
									ReleaseMutex( dat->hMutex );
									if( addrTemp.sin_addr.s_addr ){
										*(SOCKADDR_IN*)&data[0] = addrTemp;
										*(WORD*)&data[ sizeof(addrTemp) ] = dat->stdbyPort[1];
										dat->SendCmdR( &addr, cmd_ready, data, sizeof(addrTemp) + 2 );

										*(SOCKADDR_IN*)&data[0] = addr;
										*(WORD*)&data[ sizeof(addr) ] = dat->stdbyPort[0];
										dat->SendCmdR( &addrTemp, cmd_ready, data, sizeof(addr) + 2 );
									}
								}else{
									dat->SendCmdR( &addr, res_continue );
								}
							}else if( addr.sin_addr.s_addr == dat->Standby[1].sin_addr.s_addr ){
								lastTime->Standby[1] = *nowTime;
								if( dat->Standby[0].sin_addr.s_addr ){
									WaitForSingleObject( dat->hMutex, INFINITE );
									SOCKADDR_IN addrTemp = dat->Standby[0];
									ReleaseMutex( dat->hMutex );
									if( addrTemp.sin_addr.s_addr ){
										*(SOCKADDR_IN*)data = addrTemp;
										*(WORD*)&data[ sizeof(addrTemp) ] = dat->stdbyPort[0];
										dat->SendCmdR( &addr, cmd_ready, data, sizeof(addrTemp) + 2 );

										*(SOCKADDR_IN*)data = addr;
										*(WORD*)&data[ sizeof(addr) ] = dat->stdbyPort[1];
										dat->SendCmdR( &addrTemp, cmd_ready, data, sizeof(addr) + 2 );
									}
								}else{
									dat->SendCmdR( &addr, res_continue );
								}
							}else if( !(dat->Standby[0].sin_addr.s_addr) ){
								lastTime->Standby[0] = *nowTime;
								WaitForSingleObject( dat->hMutex, INFINITE );
								dat->Standby[0] = addr;
								dat->stdbyPort[0] = *(WORD*)recvData;
								ReleaseMutex( dat->hMutex );

							}else if( !(dat->Standby[1].sin_addr.s_addr) ){
								lastTime->Standby[1] = *nowTime;
								WaitForSingleObject( dat->hMutex, INFINITE );
								dat->Standby[1] = addr;
								dat->stdbyPort[1] = *(WORD*)recvData;
								ReleaseMutex( dat->hMutex );
							}
							break;
						case cmd_cast :
						case cmd_gameInfo :
						case cmd_continue :
							{
								int Counter;
								WORD Flg = 0;
								for(Counter = 0; Counter<20; Counter++){
									if( cmdExitTime[Counter][0] ){
										if( cmdExitTime[Counter][1] + 30000 < dat->nowTime ){
											cmdExitTime[Counter][0] = 0;
											cmdExitTime[Counter][1] = 0;
										}
									}
								}
								for(Counter = 0; Counter<20; Counter++){
									if( cmdExitTime[Counter][0] ){
										if( cmdExitTime[Counter][0] == addr.sin_addr.s_addr ){
											Flg = 0;
											break;
										}
									}else{
										Flg = 1;
									}
								}
								if( Flg ){
									dat->SendCmdR( &addr, cmd_exit );
									for(Counter = 0; Counter<20; Counter++){
										if( !cmdExitTime[Counter][0] ){
											cmdExitTime[Counter][0] = addr.sin_addr.s_addr;
											cmdExitTime[Counter][1] = dat->nowTime;
											break;
										}
									}
								}
							}
							break;
						case cmd_delayobs :
							dat->SendCmdR( &addr, res_delayobs, recvData, 4 );
							break;
						case cmd_echo :
							dat->SendCmdR( &addr, res_echo );
							break;
						case res_echo :
							dat->echoFlg.Other = 1;
							break;
						case cmd_access :
							if( dat->myInfo.terminalMode == mode_wait || (dat->myInfo.terminalMode == mode_wait_target && addr.sin_addr.s_addr == dat->waitTargetIP ) ){
								//接続待機中
								if( !(dat->Access.sin_addr.s_addr) && !(dat->Away.sin_addr.s_addr) ){
									//要検討
									lastTime->Away = *nowTime;
									WaitForSingleObject( dat->hMutex, INFINITE );
									dat->Away = addr;
									ReleaseMutex( dat->hMutex );
									dat->accessFlg = status_ok;

									if (size >= 10 && !memcmp(recvData, mbcaster_id, 5)) {
										if (size >= 11) {
											dat->remoteProtocolVersion = recvData[5];
										}
									} else {
										WaitForSingleObject( dat->hPrintMutex, INFINITE );
										cout << "-- WARNING -- Remote version of ABKCaster is incompatible!" << endl
											<< "It is highly likely that you will be unable to play normally." << endl
											<< "Both players upgrading to the latest version is recommended." << endl;
										ReleaseMutex( dat->hPrintMutex );
									}

									if (dat->remoteProtocolVersion >= 1 && recvData[6] > 0 && size >= 13 && !dat->namesLocked) {
										int l = recvData[7];
										if (l > 0 && l <= 20 && (size-l)>=13) {
											dat->p2PlayerName[l] = '\0';
											memcpy(dat->p2PlayerName, &recvData[8], l);
											dat->cleanString(dat->p2PlayerName, 1);
										}
									}

									dat->accessFlg = status_ok;
									data[0] = status_ok;
									memcpy(&data[1], mbcaster_id, 5);
									data[6] = mbcaster_protocol_id;

									int n = 7;
									int name_len = strlen(dat->myPlayerName);
									data[n++] = 1;
									data[n++] = name_len;
									memcpy(data+n, dat->myPlayerName, name_len);
									n += name_len;

									dat->SendCmdR( dest_away, res_access, data, n );
									break;
								}
							}else if( dat->myInfo.terminalMode == mode_access ){
								//none
							}else if( dat->myInfo.terminalMode == mode_default ){
								//none
							}else{
								//返信など
								data[0] = status_bad;
								memcpy(&data[1], mbcaster_id, 5);
								data[6] = mbcaster_protocol_id;
								dat->SendCmdR( &addr, res_access, data, 7 );
							}
							break;
						case cmd_dataInfo :
							if (size >= 11 && !memcmp(recvData, mbcaster_id, 5) && recvData[5] >= 1) {
								int len = 8;

								data[0] = dat->myInfo.terminalMode;
								memcpy(data+1, mbcaster_id, 5);
								data[6] = mbcaster_protocol_id;
								data[7] = 0;
								if (dat->myInfo.terminalMode == mode_root) {
									data[7] = 1;
									memcpy(data+8, &dat->Away, sizeof(dat->Away));
									len += sizeof(dat->Away);
								} else if (dat->myInfo.terminalMode == mode_branch || dat->myInfo.terminalMode == mode_subbranch) {
									data[7] = 2;
									memcpy(data+8, &dat->Root, sizeof(dat->Root));
									len += sizeof(dat->Root);
								}

								if (dat->myInfo.terminalMode == mode_branch || dat->myInfo.terminalMode == mode_subbranch
								    || dat->myInfo.terminalMode == mode_root || dat->myInfo.terminalMode == mode_broadcast
								    || dat->myInfo.terminalMode == mode_debug) {
								    	data[len++] = 2;

								    	const char *str;
								    	int name_len;

								    	str = dat->p1PlayerName;
								    	if (dat->p1TempName) {
								    		str = "";
								    	}
								    	name_len = strlen(str);
								    	data[len++] = name_len;
								    	memcpy(data+len, str, name_len);
								    	len += name_len;

								    	str = dat->p2PlayerName;
								    	if (dat->p2TempName) {
								    		str = "";
								    	}
								    	name_len = strlen(str);
								    	data[len++] = name_len;
								    	memcpy(data+len, str, name_len);
								    	len += name_len;
								} else if (dat->myPlayerName[0]) {
									data[len++] = 1;

								    	const char *str;
								    	int name_len;

								    	str = dat->myPlayerName;
								    	name_len = strlen(str);
								    	data[len++] = name_len;
								    	memcpy(data+len, str, name_len);
								    	len += name_len;
								} else {
									data[len++] = 0;
								}
								dat->SendCmdR( &addr, res_dataInfo, &(dat->dataInfo), sizeof(dat->dataInfo), data, len );
							} else {
								//メニューの情報を送る
								dat->SendCmdR( &addr, res_dataInfo, &(dat->dataInfo), sizeof(dat->dataInfo), &(dat->myInfo.terminalMode), 1 );
							}
							break;
						case cmd_join :
							{
								WORD usePort = 0;
								if( size > 6 ){
									usePort = *(WORD*)&recvData[0];
								}

								if( dat->obsCountFlg ){
									int Counter = 0;
									for(;;){
										if( dat->obsIP[Counter] ){
											if( dat->obsIP[Counter] == addr.sin_addr.s_addr ){
												break;
											}
										}else{
											dat->obsIP[Counter] = addr.sin_addr.s_addr;
											cout << "observer #" << Counter << " has connected";
											if (!memcmp(&recvData[2], mbcaster_id, 5) && recvData[7] >= 1 && recvData[8] > 0) {
												int l = recvData[9];
												if (l > 0 && l <= 20) {
													char str[21];
													memcpy(str, recvData+10, l);
													str[l] = 0;

													cout << ": " << str;
												}
											}
											cout << endl;

											if( Counter == 63 ){
												cout << "debug : Counter Limit" << endl;
											}
											break;
										}

										if( Counter >= 63 ) break;
										Counter++;
									}
								}

								if( !(dat->Leaf[0].sin_addr.s_addr) ){
									if( dat->myInfo.terminalMode == mode_branch || dat->allowObsFlg ){
										lastTime->Leaf[0] = *nowTime;
										WaitForSingleObject( dat->hMutex, INFINITE );
										dat->Leaf[0] = addr;
										ReleaseMutex( dat->hMutex );

										data[0] = status_ok;
										dat->SendCmdR( &addr, res_join, data, 1 );
									}
								}else if( dat->myInfo.terminalMode == mode_branch && !(dat->Leaf[1].sin_addr.s_addr) ){
									lastTime->Leaf[1] = *nowTime;
									WaitForSingleObject( dat->hMutex, INFINITE );
									dat->Leaf[1] = addr;
									ReleaseMutex( dat->hMutex );

									data[0] = status_ok;
									dat->SendCmdR( &addr, res_join, data, 1 );
								}else if( dat->myInfo.terminalMode == mode_branch && !(dat->Leaf[2].sin_addr.s_addr) ){
									lastTime->Leaf[2] = *nowTime;
									WaitForSingleObject( dat->hMutex, INFINITE );
									dat->Leaf[2] = addr;
									ReleaseMutex( dat->hMutex );

									data[0] = status_ok;
									dat->SendCmdR( &addr, res_join, data, 1 );
								}else if( dat->myInfo.terminalMode == mode_branch && !(dat->Leaf[3].sin_addr.s_addr) ){
									lastTime->Leaf[3] = *nowTime;
									WaitForSingleObject( dat->hMutex, INFINITE );
									dat->Leaf[3] = addr;
									ReleaseMutex( dat->hMutex );

									data[0] = status_ok;
									dat->SendCmdR( &addr, res_join, data, 1 );
								}else{
									if( addr.sin_addr.s_addr && TestIP( addr.sin_addr.s_addr ) && dat->Leaf[0].sin_addr.s_addr && TestIP( dat->Leaf[0].sin_addr.s_addr ) ){
										//要検討
										WaitForSingleObject( dat->hMutex, INFINITE );
										SOCKADDR_IN addrTemp = dat->Leaf[0];
										ReleaseMutex( dat->hMutex );
										dat->SendCmdR( &addr, cmd_addr_branch, &addrTemp, sizeof(addrTemp) );
									}else if( !TestIP( dat->Leaf[0].sin_addr.s_addr ) ){
										WaitForSingleObject( dat->hMutex, INFINITE );
										SOCKADDR_IN addrTemp = dat->Leaf[0];
										ReleaseMutex( dat->hMutex );
										dat->SendCmdR( &addr, cmd_addr_branch, &addrTemp, sizeof(addrTemp) );
										if( usePort ){
											dat->SendCmdR( &addrTemp, cmd_addr_leaf, &addr, sizeof(addr), &usePort, 2 );
										}else{
											dat->SendCmdR( &addrTemp, cmd_addr_leaf, &addr, sizeof(addr) );
										}
									}else if( !TestIP( dat->Leaf[1].sin_addr.s_addr ) ){
										WaitForSingleObject( dat->hMutex, INFINITE );
										SOCKADDR_IN addrTemp = dat->Leaf[1];
										ReleaseMutex( dat->hMutex );
										dat->SendCmdR( &addr, cmd_addr_branch, &addrTemp, sizeof(addrTemp) );
										if( usePort ){
											dat->SendCmdR( &addrTemp, cmd_addr_leaf, &addr, sizeof(addr), &usePort, 2 );
										}else{
											dat->SendCmdR( &addrTemp, cmd_addr_leaf, &addr, sizeof(addr) );
										}
									}else if( !TestIP( dat->Leaf[2].sin_addr.s_addr ) ){
										WaitForSingleObject( dat->hMutex, INFINITE );
										SOCKADDR_IN addrTemp = dat->Leaf[2];
										ReleaseMutex( dat->hMutex );
										dat->SendCmdR( &addr, cmd_addr_branch, &addrTemp, sizeof(addrTemp) );
										if( usePort ){
											dat->SendCmdR( &addrTemp, cmd_addr_leaf, &addr, sizeof(addr), &usePort, 2 );
										}else{
											dat->SendCmdR( &addrTemp, cmd_addr_leaf, &addr, sizeof(addr) );
										}
									}else if( !TestIP( dat->Leaf[3].sin_addr.s_addr ) ){
										WaitForSingleObject( dat->hMutex, INFINITE );
										SOCKADDR_IN addrTemp = dat->Leaf[3];
										ReleaseMutex( dat->hMutex );
										dat->SendCmdR( &addr, cmd_addr_branch, &addrTemp, sizeof(addrTemp) );
										if( usePort ){
											dat->SendCmdR( &addrTemp, cmd_addr_leaf, &addr, sizeof(addr), &usePort, 2 );
										}else{
											dat->SendCmdR( &addrTemp, cmd_addr_leaf, &addr, sizeof(addr) );
										}
									}else{
										data[0] = status_bad;
										dat->SendCmdR( &addr, res_join, data, 1 );
									}
								}
							}
							break;
						}
					}
				}else{
					//バージョンが違うとき
					if( addr.sin_addr.s_addr == dat->Access.sin_addr.s_addr ){
						dat->accessFlg = status_error;
					}
					if( recvBuf[0] == cmd_version_error ){
						if( addr.sin_addr.s_addr == dat->Root.sin_addr.s_addr ){
							WaitForSingleObject( dat->hMutex, INFINITE );
							memset( &(dat->Root), 0, sizeof(SOCKADDR_IN));
							ReleaseMutex( dat->hMutex );

							cout << "ERROR : Command Version check ( Root )" << endl;
							dat->roopFlg = 0;

						}else if( addr.sin_addr.s_addr == dat->Away.sin_addr.s_addr ){
							WaitForSingleObject( dat->hMutex, INFINITE );
							memset( &(dat->Away), 0, sizeof(SOCKADDR_IN));
							ReleaseMutex( dat->hMutex );

							cout << "ERROR : Command Version check ( Away )" << endl;
							dat->roopFlg = 0;

						}else if( addr.sin_addr.s_addr == dat->Leaf[0].sin_addr.s_addr ){
							WaitForSingleObject( dat->hMutex, INFINITE );
							memset( &(dat->Leaf[0]), 0, sizeof(SOCKADDR_IN));
							ReleaseMutex( dat->hMutex );
						}else if( addr.sin_addr.s_addr == dat->Leaf[1].sin_addr.s_addr ){
							WaitForSingleObject( dat->hMutex, INFINITE );
							memset( &(dat->Leaf[1]), 0, sizeof(SOCKADDR_IN));
							ReleaseMutex( dat->hMutex );
						}else if( addr.sin_addr.s_addr == dat->Leaf[2].sin_addr.s_addr ){
							WaitForSingleObject( dat->hMutex, INFINITE );
							memset( &(dat->Leaf[2]), 0, sizeof(SOCKADDR_IN));
							ReleaseMutex( dat->hMutex );
						}else if( addr.sin_addr.s_addr == dat->Leaf[3].sin_addr.s_addr ){
							WaitForSingleObject( dat->hMutex, INFINITE );
							memset( &(dat->Leaf[3]), 0, sizeof(SOCKADDR_IN));
							ReleaseMutex( dat->hMutex );
						}else if( addr.sin_addr.s_addr == dat->Access.sin_addr.s_addr ){
							WaitForSingleObject( dat->hMutex, INFINITE );
							memset( &(dat->Access), 0, sizeof(SOCKADDR_IN));
							ReleaseMutex( dat->hMutex );

							cout << "ERROR : Command Version check ( Access )" << endl;
							dat->roopFlg = 0;

						}
					}else{
						data[0] = cmd_version_error;
						dat->SendDataR( &addr, data, 1 );
					}
				}
			}

		}
	}

	if( zBuf ){
		free( zBuf );
		zBuf = NULL;

		//警告消しのため
		if( zBuf ){}
	}

	#if debug_mode_thread
		WaitForSingleObject( dat->hPrintMutex, INFINITE );
		cout << "debug : rcvThread end" << endl;
		ReleaseMutex( dat->hPrintMutex );
	#endif
	return 0;
}
