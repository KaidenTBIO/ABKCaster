#include "mainDatClass.h"
using namespace std;

//要検討
//念のためinputData.init()を余計に行っている

typedef struct{
	WORD mode;
	WORD targetPort;
	WORD enPort;
	char targetIP[80];
	HANDLE hPrintMutex;
	//WORD boosterOnFlg;

	WORD accessPort;
	char *accessIP;
	char *standbyIP;
}menuDataStruct;

void Sound( mainDatClass* dat ){
	//sound
	if( dat->waveFlg ){
		if( strcmp( dat->nowDir, "fail" ) && strlen( dat->nowDir ) < 180 ){
			char path[200];
			strcpy( path ,dat->nowDir );
			strcat( path, "\\sound.wav\0" );
			if( !_access( path, 0 ) ){
				PlaySound( path, NULL, SND_ASYNC );
			}
		}
	}

	if( dat->beepFlg ){
		Beep(3000, 500);
	}
}

unsigned long mainDatClass::GetAddress(char *name, unsigned short *port) {
	char namebuf[80];
	if( strcmp( iniPath, "fail" ) ){
		GetPrivateProfileString( "ALIAS", name, name, namebuf, 79, iniPath );
		namebuf[79] = '\0';

		name = namebuf;
	}

	char *colon = strchr(name, ':');
	if (colon) {
		*colon = 0;
		if (port) {
		*port = htons(atoi(colon+1));
		}
	} else {
		colon = 0;
	}

	unsigned long addr = inet_addr(name);
	if (addr != INADDR_NONE) {
		if (colon) {
		*colon = ':';
		}
		return addr;
	}

	struct hostent *hostdata;
	hostdata = gethostbyname(name);

	if (colon) {
		*colon = ':';
	}

	if (!hostdata) {
		return INADDR_NONE;
	}

	struct in_addr *in_addr = (struct in_addr *)hostdata->h_addr;

	return in_addr->s_addr;
}

bool read_int(int &dest) {
	char str[50];
	//cin.clear();
	cin.getline(str,50);

	if (cin.fail()) {
		return 1;
	}

	if (str[0] == '\0') {
		return 0;
	}

	int n = 0;
	do {
		if (str[n] < '0' || str[n] > '9') {
			return 1;
		}
		n++;
	} while (str[n] != '\0');
	dest = atoi(str);

	return 0;
}

int menu( menuDataStruct* menuData ){
	if( !menuData ) return 0xF;

	memset( menuData->targetIP, 0, sizeof( menuData->targetIP ) );

	WaitForSingleObject( menuData->hPrintMutex, INFINITE );
	cout	<< endl
		<< "<Menu>" << endl
		<< "Escape key will take you back to here." << endl
		<< "0 : Exit" << endl
		<< "1 : Wait for access" << endl
		<< "2 : Try access" << endl
		<< "3 : Spectate" << endl
		<< "4 : Broadcast" << endl
		<< "5 : Get Information" << endl
		<< "6 : Try access ( Tough )" << endl
		<< "7 : Standby" << endl
		<< "8 : Test myPort" << endl
		<< "9 : debug( 127.0.0.1 )" << endl;
	cout	<< "Input [" << menuData->mode << "] >";
	ReleaseMutex( menuData->hPrintMutex );

	int mode = menuData->mode;
	if( read_int(mode) || cin.fail() ){
		cin.clear();
		menuData->mode = 0;
		return 1;
	}
	menuData->mode = mode;

	switch( menuData->mode ){
	case 0 :
		return 0xF;
	case 1 :
		break;
	case 2 :
	case 3 :
	case 5 :
	case 55 :
	case 6 :
	case 7 :
	case 8 :
		menuData->targetPort = menuData->enPort;

		cin.clear();

		WaitForSingleObject( menuData->hPrintMutex, INFINITE );
		cout << "Input Target IP [";

		if (menuData->mode == 7 || menuData->mode == 8) {
		    cout << menuData->standbyIP;
		} else {
		    cout << menuData->accessIP;
		}

		cout << "] >";
		ReleaseMutex( menuData->hPrintMutex );
		cin.getline(menuData->targetIP, 80);
		if( cin.fail() ){
			cin.clear();
			return 1;
		}
		if (menuData->targetIP[0] == '\0') {
		    if (menuData->mode == 7 || menuData->mode == 8) {
		        strcpy(menuData->targetIP, menuData->standbyIP);
		    } else {
			strcpy(menuData->targetIP, menuData->accessIP);
		    }
		}
		if (menuData->targetIP[0] == '\0') {
		    return 1;
		}
		break;
	case 4 :
		break;
	case 9 :
		break;
	//case 10 : // practice mode, doesn't actually work, don't use it.
	//	break;
	default :
		return 1;
	}

	menuData->targetIP[79] = 0;
	return 0;
}

int mainDatClass::Entrance(){
	#if debug_mode_main
		WaitForSingleObject( hPrintMutex, INFINITE );
		cout << "debug : Entrance()" << endl;
		ReleaseMutex( hPrintMutex );
	#endif

	menuDataStruct menuData;
	menuData.mode = lastMode;
	menuData.hPrintMutex = hPrintMutex;
	menuData.enPort = enPort;

	menuData.accessPort = accessPort;
	menuData.accessIP = accessIP;
	menuData.standbyIP = standbyIP;

	roopFlg = 0;
	delayTime = 0;
	continueFlg = 1;

	//SOCADDR_INの初期化
	WaitForSingleObject( hMutex, INFINITE );
	memset( &Away , 0, sizeof(SOCKADDR_IN) );
	memset( &Root , 0, sizeof(SOCKADDR_IN) );
	memset( &Branch , 0, sizeof(SOCKADDR_IN) );
	memset( &subBranch , 0, sizeof(SOCKADDR_IN) );
	memset( &Access , 0, sizeof(SOCKADDR_IN) );

	memset( &Leaf[0] , 0, sizeof(SOCKADDR_IN) );
	memset( &Leaf[1] , 0, sizeof(SOCKADDR_IN) );
	memset( &Leaf[2] , 0, sizeof(SOCKADDR_IN) );
	memset( &Leaf[3] , 0, sizeof(SOCKADDR_IN) );

	memset( &Ready , 0, sizeof(SOCKADDR_IN) );
	memset( &Standby[0] , 0, sizeof(SOCKADDR_IN) );
	memset( &Standby[1] , 0, sizeof(SOCKADDR_IN) );
	ReleaseMutex( hMutex );

//	Sleep(100);
	if( inputData.init() ) return 0xF;


	memset( &myInfo, 0, sizeof(myInfo) );
	memset( &enInfo, 0, sizeof(enInfo) );
	memset( &dataInfo, 0, sizeof(dataInfo) );
	myInfo.terminalMode = mode_default;
	myInitFlg = 0;
	enInitFlg = 0;
	memset( &echoFlg, 0, sizeof(echoFlg) );
	memset( &lastTime, 0, sizeof(lastTime) );
	myInfo.phase = phase_default;
	dataInfo.phase = phase_default;
	myRandNo = 0;
	enRandNo = 0;
	myRand = 0;
	enRand = 0;
	delay = 0;
	toughModeFlg = 0;
	autoNextFlg = 0;
	targetMode = 0;

	nThreads = 0;
	handle = 0;
	rockFlg = 0;

	stdbyPort[0] = 0;
	stdbyPort[1] = 0;
	readyPort = 0;
	playerSideHostFlg = 0;

	winsA = 0;
	roundsWonA = 0;
	winsB = 0;
	roundsWonB = 0;
	lastGameTime = 0;
	totalGameTime = 0;
	nPlayers = 0;
	namesLocked = 0;

	practiceModeFlg = 0;

	memset( obsIP, 0, sizeof( obsIP ) );

	strcpy(p1PlayerName, myPlayerName);
	strcpy(p2PlayerName, "");
	p1TempName = 0;
	p2TempName = 0;
	if (!p1PlayerName[0]) {
		p1TempName = 1;
	}

	remoteProtocolVersion = 0;

	//srand
//	srand( (unsigned)startTime );
//	srand( (unsigned)time( NULL ) );

	//wait
	if( mainFlg == main_default && autoWaitFlg ){
		mainFlg = main_wait;
	}

	//main
	switch( mainFlg ){
	case main_default :
		{
			int Res;
			for(;;){
				Res = menu( &menuData );
				if( Res == 0xF ){
					if( endTimeFlg ){
						//稼動時間を表示
						cout	<< endl
							<< "<End>" << endl
							<< "Time : ";
						double allTime = difftime( time( NULL ), startTime );
						if( allTime > 60 * 60 ) cout << (DWORD)(( allTime / 60 ) / 60) << "h ";
						allTime = allTime - (DWORD)((allTime / 60) / 60) * 60 * 60;
						cout << (DWORD)(allTime / 60)  << "min" << endl;
						Sleep(500);
					}
					return 0xF;
				}
				if( Res == 0 ) break;
			}
			lastMode = menuData.mode;
		}
		break;
	case main_file :
		//動作モードと相手のIPとポートの情報を読む
		{
			char path[200];
			if( GetCurrentDirectory( 180, path ) < 180 ){
				//ok
				path[180] = 0;
				strcat( path, "\\info.ini\0" );

				if( GetPrivateProfileInt( "MAIN", "onoff", 0, path ) ){
					//on
					int mode = GetPrivateProfileInt( "MAIN", "mode", 0, path );
					switch( mode ){
					case 0 :
						//通常
						//それ用のモードを追加
						menuData.mode = 20;

						//情報読み込み
						GetPrivateProfileString( "TARGET", "ip", '\0', menuData.targetIP, 49, path );
						menuData.targetIP[49] = '\0';
						menuData.targetPort = GetPrivateProfileInt( "TARGET", "port", 0, path );

						if( menuData.targetIP[0] != '\0' && menuData.targetPort != 0 ){
							//ok
						}else{
							return 0xF;
						}
						break;
					case 1 :
						//特定相手待ち
						menuData.mode = 21;
						GetPrivateProfileString( "TARGET", "ip", '\0', menuData.targetIP, 49, path );
						menuData.targetIP[49] = '\0';

						if( menuData.targetIP[0] != '\0' ){
							//ok
						}else{
							return 0xF;
						}
						cout	<<endl
							<< "<Menu>" << endl;
						break;
					case 2 :
						//待ち
						menuData.mode = 1;
						cout	<<endl
							<< "<Menu>" << endl;
						break;
					default :
						return 0xF;
					}
				}else{
					return 0xF;
				}
			}else{
				return 0xF;
			}
		}

		mainFlg = main_end;
		break;
	case main_arg :
		//引数指定
		//fileモードのを流用
		switch( argData.argMode ){
		case 0 :
			menuData.mode = 20;

			//IP
			strcpy( menuData.targetIP, argData.targetIP );
			menuData.targetIP[79] = '\0';

			//port
			if( argData.targetPort ){
				menuData.targetPort = argData.targetPort;
			}else{
				menuData.targetPort = 7500;
			}

			if( menuData.targetIP[0] != '\0' ){
				//ok
			}else{
				return 0xF;
			}

			break;
		case 1 :
			//特定相手待ち
			menuData.mode = 21;
			strcpy( menuData.targetIP, argData.targetIP );
			menuData.targetIP[79] = '\0';

			if( menuData.targetIP[0] != '\0' ){
				//ok
			}else{
				return 0xF;
			}
			cout	<<endl
				<< "<Menu>" << endl;
			break;
		case 2 :
			//待ち
			menuData.mode = 1;
			cout	<<endl
				<< "<Menu>" << endl;
			break;
		case 3 :
			//特定相手待ち
			menuData.mode = 22;
			strcpy( menuData.targetIP, argData.targetIP );
			menuData.targetIP[79] = '\0';

			if( menuData.targetIP[0] != '\0' ){
				//ok
			}else{
				return 0xF;
			}
			cout	<<endl
				<< "<Menu>" << endl;
			break;
		default :
			return 0xF;
		}

		mainFlg = main_end;
		break;
	case main_end :
		//終了を待つ
		for(;;){
			if( GetEsc() ) break;
			if( !FindProcess() ) break;
			Sleep(200);
		}
		return 0xF;
	case main_wait :
		cout	<<endl
			<< "<Menu>" << endl;
		menuData.mode = 1;
		break;
	default :
		return 0xF;
	}

	nowTime = timeGetTime();

	bool skipSpecQuestion = 0;

	myInfo.terminalMode = mode_access;
	switch( menuData.mode ){
	case 1 :
		//Wait for access
		cout << "Now Waiting for access." << endl;

		if( autoWaitOnFlg ){
			autoWaitFlg = 1;
		}

		myInfo.terminalMode = mode_wait;
		accessFlg = status_default;
		for(;;){
			if( accessFlg == status_ok ) break;
			if( accessFlg == status_error ) return 1;
			if( accessFlg == status_bad ) return 1;
			if( GetEsc() ){
				autoWaitFlg = 0;
				if( mainFlg == main_wait ){
					mainFlg = main_default;
				}

				return 1;
			}
			Sleep(100);
		}
		cout << "Access from " << inet_ntoa( Away.sin_addr ) << endl;
		myInfo.terminalMode = mode_root;

		Sound( this );

		//playerSideHost
		playerSideHostFlg = 1;

		break;
	case 6 :
		//Tough
		toughModeFlg = 1;
	case 3 :
		if (menuData.mode == 3) {
			// Spectate
			lastTime.Access = nowTime;

			Access.sin_family = AF_INET;
			Access.sin_port = htons( menuData.targetPort );
			Access.sin_addr.s_addr = GetAddress(menuData.targetIP, &Access.sin_port);
			if( !Access.sin_addr.s_addr || Access.sin_addr.s_addr == 0xFFFFFFFF ){
				return 1;
			}
			strcpy(accessIP, menuData.targetIP);

			accessPort = ntohs(Access.sin_port);

			cout << "Waiting on game at " << inet_ntoa(Access.sin_addr) << ":" << dec << accessPort << endl;

			lastTime.Access = nowTime;
			dataInfo.phase = 0xFF;

			char param[6];
			memcpy(param, mbcaster_id, 5);
			param[5] = mbcaster_protocol_id;

			int counter = 9;

			for(;;){
				counter++;
				if (counter == 10) {
					SendCmd( dest_access, cmd_dataInfo, param, 6);
					counter = 0;
				}

				Sleep(100);

				if( dataInfo.phase == phase_battle || dataInfo.phase == phase_read || dataInfo.phase == phase_menu ) {
					break;
				}

				if( !(Access.sin_addr.s_addr) ){
					return 1;
				}
				if( GetEsc() ) return 1;
			}

			Sound( this );

			toughModeFlg = 0;
			skipSpecQuestion = 1;
		}
	case 2 :
		//Try access

		//念のため（要検討）　
		lastTime.Access = nowTime;

		Access.sin_family = AF_INET;
		Access.sin_port = htons( menuData.targetPort );
		Access.sin_addr.s_addr = GetAddress(menuData.targetIP, &Access.sin_port);
		if( !Access.sin_addr.s_addr || Access.sin_addr.s_addr == 0xFFFFFFFF ){
			return 1;
		}

		strcpy(accessIP, menuData.targetIP);

		lastTime.Access = nowTime;
		echoFlg.Access = 0;
		for(;;){
			SendCmd( dest_access, cmd_echo );
			Sleep(100);
			if( echoFlg.Access ) break;
			if( !(Access.sin_addr.s_addr) ) return 1;
			if( GetEsc() ) return 1;
		}
		break;
	case 4 :
		//Broadcast
		myInfo.terminalMode = mode_broadcast;

		GetPrivateProfileString( "PLAYER", "p2Name", "", p2PlayerName, 20, iniPath);
		p2PlayerName[20] = '\0';
		cleanString(p2PlayerName, 1);

		if (!p2PlayerName[0]) {
			strcpy(p2PlayerName, "Player 2");
		}
		break;
	case 5 :
		//Get Information
		//念のため（要検討）　
		lastTime.Access = nowTime;

		Access.sin_family = AF_INET;
		Access.sin_port = htons( menuData.targetPort );
		Access.sin_addr.s_addr = GetAddress(menuData.targetIP, &Access.sin_port);
		if( !Access.sin_addr.s_addr || Access.sin_addr.s_addr == 0xFFFFFFFF ){
			return 1;
		}
		strcpy(accessIP, menuData.targetIP);


		lastTime.Access = nowTime;

		{
			char param[6];
			memcpy(param, mbcaster_id, 5);
			param[5] = mbcaster_protocol_id;

			dataInfo.phase = 0xFF;
			for(;;){
				SendCmd( dest_access, cmd_dataInfo, param, 6 );
				Sleep(100);
				if( dataInfo.phase != 0xFF ) break;
				if( !(Access.sin_addr.s_addr) ){
					return 1;
				}
				if( GetEsc() ) return 1;
			}

			float delayTemp = 0;
			if( GetDelay( dest_access, &delayTemp) ) return 1;


			cout.setf(ios::fixed, ios::floatfield);
			cout	<< endl
				<< "<Information>" << endl
				<< "Delay : About " << setprecision( 1 ) << delayTemp << "[ms]" << endl;

			if (dataInfo.terminalMode == mode_root) {
				if (dataInfo.playerSide == 0xA) {
					cout << "Side : Player 1" << endl;
				} else if (dataInfo.playerSide == 0xB) {
					cout << "Side : Player 2" << endl;
				}
			}

			if (remoteProtocolVersion >= 1) {
				if (hasRemote) {
					cout << "Remote : " << inet_ntoa(Remote.sin_addr) << ":" << dec << ntohs(Remote.sin_port) << endl;
				}

				if (nPlayers == 1) {
					if (p1PlayerName[0]) {
						cout << "Player : " << p1PlayerName << endl;
					}
				} else if (nPlayers == 2) {
					cout << "Players : ";

					if (p1PlayerName[0]) {
						cout << p1PlayerName;
					} else {
						cout << "Unknown";
					}

					cout << " vs ";

					if (p2PlayerName[0]) {
						cout << p2PlayerName;
					} else {
						cout << "Unknown";
					}
					cout << endl;
				}
			}

			switch( dataInfo.phase ){
			case phase_read :
			case phase_battle :
				cout << "Phase : Battle" << endl;
				{
					BYTE ID;
					WORD printFlg = 0;
					for(;;){
						if( printFlg == 0){
							ID = dataInfo.A.ID;
							printFlg = 1;
						}else{
							ID = dataInfo.B.ID;
							printFlg = 2;
						}

						cout << getCharacterName(ID);

						if( printFlg == 1 ) cout << " vs ";
						if( printFlg > 1 ) break;
					}
				}
				cout << " ( " << dataInfo.gameTime / 2 / 60 << "sec )" << endl;
				break;
			case phase_menu :
				cout << "Phase : Menu" << endl;
				break;
			case phase_default :
				if( targetMode == mode_wait ){
					cout << "Phase : Default ( Wait )" << endl;
				}else{
					cout << "Phase : Default" << endl;
				}
				break;
			default :
				cout << "Phase : Default?" << endl;
				break;
			}
		}
		return 1;
	case 7 :
		//Standby
		lastTime.Access = nowTime;

		Access.sin_family = AF_INET;
		Access.sin_port = htons( menuData.targetPort );
		Access.sin_addr.s_addr = GetAddress(menuData.targetIP, &Access.sin_port);
		if( !Access.sin_addr.s_addr || Access.sin_addr.s_addr == 0xFFFFFFFF ){
			return 1;
		}

		strcpy(standbyIP, menuData.targetIP);

		lastTime.Access = nowTime;

		cout << "Now Waiting for introduction." << endl;

		{
			DWORD roopCounter = 100;
			for(;;){
				if( roopCounter > 30 ){
					SendCmd( dest_access, cmd_standby, &myPort, 2 );
					roopCounter = 0;
				}
				roopCounter++;
				Sleep(10);
				if( Ready.sin_addr.s_addr ) break;
				if( !(Access.sin_addr.s_addr) ) return 1;
				if( GetEsc() ) return 1;
			}
		}

		lastTime.Access = nowTime;
		WaitForSingleObject( hMutex, INFINITE );
		Access = Ready;
		ReleaseMutex( hMutex );

		//転送後
		cout << "Now Waiting for response." << endl;
		echoFlg.Access = 0;
		for(;;){
			{
				SendCmd( dest_access, cmd_echo );

				SOCKADDR_IN AccessTemp = Access;
				AccessTemp.sin_port = htons( readyPort );
				SendCmd( &AccessTemp, cmd_echoes, &myPort, 2 );
			}

			Sleep(100);
			if( echoFlg.Access ) break;
			if( !(Access.sin_addr.s_addr) ) return 1;
			if( GetEsc() ) return 1;
		}

		Sound( this );

		//echoesを待つ
		Sleep(200);

		break;
	case 8 :
		//Receive Test signal
		{
			SOCKADDR_IN addrTemp;
			memset( &addrTemp, 0, sizeof( addrTemp ) );

			addrTemp.sin_family = AF_INET;
			addrTemp.sin_addr.s_addr = GetAddress(menuData.targetIP, &addrTemp.sin_port);
			if( !addrTemp.sin_addr.s_addr || addrTemp.sin_addr.s_addr == 0xFFFFFFFF ){
				return 1;
			}
			addrTemp.sin_port = htons( menuData.targetPort );


			int Res = TestPort( &addrTemp );

			cout	<< endl
				<< "<Report>" << endl;
			switch( Res ){
			case status_ok :
				cout << "Port : OK ( UDP." << myPort << " received signal )" << endl;
				break;
			case status_bad :
				cout << "Port : BAD ( UDP." << myPort << " received No signal )" << endl;
				break;
			case status_error :
				cout << "Port : ERROR or ESC" << endl;
				break;
			default :
				return 1;
			}
		}
		return 1;
	case 10:
		practiceModeFlg = 1;
	case 9 :
		strcpy(p2PlayerName, "Debug mode");
		//Debug ( 127.0.0.1 )
		WaitForSingleObject( hMutex, INFINITE );
		memset( &Away, 0, sizeof(Away));
		Away.sin_family = AF_INET;
		Away.sin_addr.s_addr = inet_addr( "127.0.0.1" );
		Away.sin_port = htons( myPort );
		ReleaseMutex( hMutex );

		myInfo.terminalMode = mode_debug;

		break;
    case 22 :
		// Spectate
		toughModeFlg = 1;
		lastTime.Access = nowTime;

		Access.sin_family = AF_INET;
		Access.sin_port = htons( menuData.targetPort );
		Access.sin_addr.s_addr = GetAddress(menuData.targetIP, &Access.sin_port);
		if( !Access.sin_addr.s_addr || Access.sin_addr.s_addr == 0xFFFFFFFF ){
			return 1;
		}
		strcpy(accessIP, menuData.targetIP);

		accessPort = ntohs(Access.sin_port);

		cout << "Waiting on game at " << inet_ntoa(Access.sin_addr) << ":" << dec << accessPort << endl;

		lastTime.Access = nowTime;
		dataInfo.phase = 0xFF;
		{
			char param[6];
			memcpy(param, mbcaster_id, 5);
			param[5] = mbcaster_protocol_id;

			int counter = 9;
			for(;;){
				counter++;
				if (counter == 10) {
					SendCmd( dest_access, cmd_dataInfo, param, 6 );
					counter = 0;
				}

				Sleep(100);

				if( dataInfo.phase == phase_battle || dataInfo.phase == phase_read || dataInfo.phase == phase_menu ) {
					break;
				}

				if( !(Access.sin_addr.s_addr) ){
					return 1;
				}
				if( GetEsc() ) return 1;
			}
		}

		Sound( this );

		toughModeFlg = 0;
		skipSpecQuestion = 1;
	case 20 :
		//Try access

		//念のため（要検討）　
		lastTime.Access = nowTime;

		Access.sin_family = AF_INET;
		Access.sin_port = htons( menuData.targetPort );
		Access.sin_addr.s_addr = GetAddress(menuData.targetIP, &Access.sin_port);
		if( !Access.sin_addr.s_addr || Access.sin_addr.s_addr == 0xFFFFFFFF ){
			return 1;
		}


		lastTime.Access = nowTime;
		echoFlg.Access = 0;
		for(;;){
			SendCmd( dest_access, cmd_echo );
			Sleep(100);
			if( echoFlg.Access ) break;
			if( !(Access.sin_addr.s_addr) ) return 1;
			if( GetEsc() ) return 1;
		}

//		Sound( this );
		break;
	case 21 :
		//Wait for access
		cout << "Now Waiting for access. " << endl;
		waitTargetIP = inet_addr( menuData.targetIP );
		if( !waitTargetIP || waitTargetIP == 0xFFFFFFFF ){
			cout << "debug : IP conv fail" << endl;
			Sleep(1000);
			return 0xF;
		}

		myInfo.terminalMode = mode_wait_target;
		accessFlg = status_default;
		for(;;){
			if( accessFlg == status_ok ) break;
			if( accessFlg == status_error ) return 1;
			if( accessFlg == status_bad ) return 1;
			if( GetEsc() ) return 1;
			Sleep(100);
		}
//		cout << "Access from " << inet_ntoa( Away.sin_addr ) << endl;
		myInfo.terminalMode = mode_root;

		//playerSideHost
		playerSideHostFlg = 1;

		Sound( this );
		break;
	default :
		return 1;
	}

	if( myInfo.terminalMode == mode_access ){
		//mode_root or mode_branch or mode_subbranch
		if( Communicate() ) return 1;
	}

	//test
	dataInfo.terminalMode = myInfo.terminalMode;

	if( myInfo.terminalMode == mode_debug ){
		myInfo.playerSide = 0xA;
		practiceModeFlg = 1;
		if (practiceModeFlg) {
			int delayTemp = 0;
			cout << "Input delay 0-10 [" << delayTemp << "]>";

			if( read_int(delayTemp) || cin.fail() ){
				cin.clear();
				return 1;
			}
			if( delayTemp > 10 ) delayTemp = 10;
			delayTime = delayTemp * 2;
		} else {
			delayTime = 0;
		}
		myInfo.sessionNo = 1;
		myInfo.sessionID = 1 + rand()%255;
		myInfo.sessionIDNext = myInfo.sessionID;

		dataInfo.sessionNo = myInfo.sessionNo;
		dataInfo.sessionID = myInfo.sessionID;
		dataInfo.sessionIDNext = myInfo.sessionIDNext;
	}else if( myInfo.terminalMode == mode_branch || myInfo.terminalMode == mode_subbranch ){
		//対戦キャラクター、gameTIme表示など
		//観戦するかどうか
		char param[6];
		memcpy(param, mbcaster_id, 5);
		param[5] = mbcaster_protocol_id;

		for(;;){
			SendCmd( dest_root, cmd_dataInfo, param, 6 );
			Sleep(100);
			if( dataInfo.sessionID ) break;
			if( !(Root.sin_addr.s_addr) ){
				cout << "ERROR : TIMEOUT ( Root )" << endl;
				return 1;
			}
			if( GetEsc() ) return 1;
		}

		WaitForSingleObject( hPrintMutex, INFINITE );
		cout	<< endl
			<< "<Target Condition>" << endl;

		//test
		switch( dataInfo.terminalMode ){
		case mode_root :
			cout << "Mode : Root" << endl;
			break;
		case mode_broadcast :
			cout << "Mode : Broadcast" << endl;
			break;
		case mode_branch :
			cout << "Mode : Branch" << endl;
			break;
		case mode_subbranch :
			cout << "Mode : subBranch" << endl;
			break;
		}

		if (dataInfo.terminalMode == mode_root) {
			if (dataInfo.playerSide == 0xA) {
				cout << "Target's side: Player 1" << endl;
			} else if (dataInfo.playerSide == 0xB) {
				cout << "Target's side: Player 2" << endl;
			}
		}

		namesLocked = 1;
		if (remoteProtocolVersion >= 1) {
			if (hasRemote) {
				cout << "Remote : " << inet_ntoa(Remote.sin_addr) << ":" << dec << ntohs(Remote.sin_port) << endl;
			}
		}

		if (remoteProtocolVersion >= 1 && nPlayers == 2) {
			if (!p1PlayerName[0]) {
				if (!unknownNameFlg && dataInfo.playerSide == 0xA) {
					strcpy(p1PlayerName, inet_ntoa(Root.sin_addr));
				} else if (!unknownNameFlg && hasRemote && dataInfo.playerSide == 0xB) {
					strcpy(p1PlayerName, inet_ntoa(Remote.sin_addr));
				} else {
					strcpy(p1PlayerName, "Unknown");
				}
			}

			if (!p2PlayerName[0]) {
				if (!unknownNameFlg && dataInfo.playerSide == 0xB) {
					strcpy(p2PlayerName, inet_ntoa(Root.sin_addr));
				} else if (!unknownNameFlg && hasRemote && dataInfo.playerSide == 0xA) {
					strcpy(p2PlayerName, inet_ntoa(Remote.sin_addr));
				} else {
					strcpy(p2PlayerName, "Unknown");
				}
			}
		} else {
			strcpy(p1PlayerName, "Unknown");
			strcpy(p2PlayerName, "Unknown");

			if (!unknownNameFlg) {
				if (dataInfo.playerSide == 0xA) {
					strcpy(p1PlayerName, inet_ntoa(Root.sin_addr));
					if (hasRemote) {
						strcpy(p2PlayerName, inet_ntoa(Remote.sin_addr));
					}
				} else if (dataInfo.playerSide == 0xB) {
					strcpy(p2PlayerName, inet_ntoa(Root.sin_addr));
					if (hasRemote) {
						strcpy(p1PlayerName, inet_ntoa(Remote.sin_addr));
					}
				}
			}
		}

		cout << "Players : " << p1PlayerName << " vs " << p2PlayerName << endl;

		p1TempName = 0;
		p2TempName = 0;

		switch( dataInfo.phase ){
		case phase_read :
		case phase_battle :
			{
				BYTE ID;
				WORD printFlg = 0;
				for(;;){
					if( printFlg == 0){
						ID = dataInfo.A.ID;
						printFlg = 1;
					}else{
						ID = dataInfo.B.ID;
						printFlg = 2;
					}

					cout << getCharacterName(ID);

					if( printFlg == 1 ) cout << " vs ";
					if( printFlg > 1 ) break;
				}
			}
			cout << " ( " << dataInfo.gameTime / 2 / 60 << "sec )" << endl;
			break;
		case phase_default :
		case phase_menu :
		default :
			cout << "Target is in Preparation." << endl;
			break;
		}

		if (!skipSpecQuestion) {
			WaitForSingleObject( hPrintMutex, INFINITE );
			cout	<< "Decide within " << timeout_time / 1000 << "sec." << endl
				<< "0 : Stop" << endl
				<< "1 : Continue" << endl
				<< "Input [1] >";
			ReleaseMutex( hPrintMutex );

			int Temp = 1;

			if( read_int(Temp) || cin.fail() ){
				cin.clear();
				return 1;
			}
			if( !(Root.sin_addr.s_addr) ){
				cout << "ERROR : TIMEOUT ( Root )" << endl;
				return 1;
			}

			switch( Temp ){
			case 0 :
				return 1;
			case 1 :
				//続行
				break;
			default :
				return 1;
			}
		}

		char join_packet[2 + 5 + 1 + 1 + 1 + 20];
		int join_len = 9;
		memcpy(join_packet, &myPort, 2);
		memcpy(join_packet+2, mbcaster_id, 5);
		join_packet[7] = mbcaster_protocol_id;
		join_packet[8] = 0;

		if (!anonymousObsFlg) {
			join_packet[8] = 1;

			int name_len = strlen(myPlayerName);
			join_packet[join_len++] = name_len;
			memcpy(join_packet + join_len, myPlayerName, name_len);
			join_len += name_len;
		}

		joinRes = status_default;
		while( joinRes == status_default ){
			SendCmd( dest_root, cmd_join, join_packet, join_len );
			Sleep(100);
			if( !(Root.sin_addr.s_addr) ){
				cout << "ERROR : TIMEOUT ( Root )" << endl;
				return 1;
			}
			if( GetEsc() ) return 1;
		}
		if( joinRes == status_ok ){
			cout << "Status : OK" << endl;
		}else if( joinRes == status_bad ){
			cout << "Status : BAD" << endl;
			return 1;
		}

		if( GetRand() ) return 1;

	}else if( myInfo.terminalMode == mode_broadcast ){
		//セッションID導入
		myInfo.sessionNo = 1;
		myInfo.sessionID = 1 + rand()%255;
		myInfo.sessionIDNext = myInfo.sessionID;

		dataInfo.sessionNo = myInfo.sessionNo;
		dataInfo.sessionID = myInfo.sessionID;
		dataInfo.sessionIDNext = myInfo.sessionIDNext;

		myInfo.playerSide = 0;
		enInfo.playerSide = 0;

		{
			int delayTimeTemp;
			cout	<< endl
				<< "<delay>" << endl;

			//sideA
			cout << "Input delay A [0] >";
			delayTimeTemp = 0;
			//cin >> setw(1) >> delayTimeTemp;
			if( read_int(delayTimeTemp) || cin.fail() ){
				cin.clear();
				//cin.ignore(1024,'\n');
				return 1;
			}
			if( delayTimeTemp > 10 ) delayTimeTemp = 10;
			delayTimeA = delayTimeTemp * 2;

			//sideB
			cout << "Input delay B [0] >";
			delayTimeTemp = 0;
			//cin >> setw(1) >> delayTimeTemp;
			if( read_int(delayTimeTemp) || cin.fail() ){
				cin.clear();
				//cin.ignore(1024,'\n');
				return 1;
			}
			if( delayTimeTemp > 10 ) delayTimeTemp = 10;
			delayTimeB = delayTimeTemp * 2;
		}
	}else if( myInfo.terminalMode == mode_root ){

		//データをまとめておいたほうが楽
		//後ほど
		myInitFlg = 1;
		for(;;){
			if( enInitFlg ) break;
			SendCmd( dest_away, cmd_initflg );
			if( !(Away.sin_addr.s_addr) ) return 1;
			if( GetEsc() ) return 1;
			Sleep(100);
		}

		//通信してABを決める
		if( GetPlayerSide() ) return 1;

		cout << "Connecting player : ";
		namesLocked = 1;
		if (!p2PlayerName[0]) {
			p2TempName = 1;
			if (unknownNameFlg) {
				strcpy(p2PlayerName, "Unknown");
			} else {
				strcpy(p2PlayerName, inet_ntoa(Away.sin_addr));
			}
		}
		cout << p2PlayerName << endl;

		if( myInfo.playerSide == 0xA ){
			int delayTimeTemp;

			for(;;){
				if( GetDelay( dest_away, &delay ) ) return 1;
				cout.setf(ios::fixed, ios::floatfield);
				cout	<< endl
					<< "<Delay>" << endl
					<< "About " << setprecision( 1 ) << delay << "[ms] delay exist in a round." << endl;
				if (delay < 4) {
					delayTimeTemp = 1;
				} else {
					delayTimeTemp = (int)ceil((delay+16.66667)/33.333333);
				}
				if (delayTimeTemp >= 10) {
					delayTimeTemp = 10;
					if (delay >= 330) {
						cout << "Warning: 330ms is the maximum playable ping." << endl
						     << "         You probably shouldn't even bother trying to play." << endl;
					}
				}
				//	<< "( DelayTime[ms] = 16[ms] x Input )" << endl;
				//cout << "Input >";
				cout << "Input Delay [Recommended: "  << delayTimeTemp <<"] >";

				if (read_int(delayTimeTemp)) {
				//cin >> delayTimeTemp;
				//if( cin.fail() ){
					cin.clear();
					//cin.ignore(1024,'\n');
					return 1;
				}

				if( delayTimeTemp > 10 ) delayTimeTemp = 10;
//				if( delayTimeTemp == 0 ) delayTimeTemp = 1;
				/*
				if( delayTimeTemp == 1 ){
					cout << "Warning: 'Buffer Margin 1' is in testing stages." << endl;
					cout << "If you experience problems, please switch to 2." << endl;
				}
				 */
				if( delayTimeTemp ) break;
			}
			delayTime = delayTimeTemp * 2;
		}else{
			cout	<< endl
				<< "<Delay>" << endl
				<< "Now waiting for buffer margin value." << endl;
			for(;;){
				SendCmd( dest_away, cmd_delay );
				if( delayTime ) break;
				if( GetEsc() ) return 1;
				Sleep(100);
			}
			cout.setf(ios::fixed, ios::floatfield);
			cout << "Buffer margin : " << delayTime / 2 << " ( Observed about " << setprecision( 1 ) << delay << "[ms] delay )" << endl;

			// swap player names
			char tempPlayerName[21];
			strcpy(tempPlayerName, p1PlayerName);
			strcpy(p1PlayerName, p2PlayerName);
			strcpy(p2PlayerName, tempPlayerName);

			bool tempTempName;
			tempTempName = p1TempName;
			p1TempName = p2TempName;
			p2TempName = tempTempName;
		}

		if( myInfo.playerSide == 0xA ){
			//セッションID（ランダム）を導入
			myInfo.sessionNo = 1;
			myInfo.sessionID = 1 + rand()%255;
			myInfo.sessionIDNext = myInfo.sessionID;
		}else{
			for(;;){
				SendCmd( dest_away, cmd_session );
				if( enInfo.sessionID ) break;
				if( !(Away.sin_addr.s_addr) ) return 1;
				if( GetEsc() ) return 1;
				Sleep(100);
			}
			myInfo.sessionNo = enInfo.sessionNo;
			myInfo.sessionID = enInfo.sessionID;
			myInfo.sessionIDNext = enInfo.sessionIDNext;
		}

		myInitFlg = 2;
		for(;;){
			if( enInitFlg == 2) break;
			SendCmd( dest_away, cmd_initflg );
			if( !(Away.sin_addr.s_addr) ) return 1;
			if( GetEsc() ) return 1;
			Sleep(100);
		}
	}else{
		return 1;
	}


	if( myInfo.terminalMode == mode_branch || myInfo.terminalMode == mode_subbranch ){
		if( autoNextOnFlg ){
			autoNextFlg = 1;
			cout << "debug : AutoNext ON" << endl;
		}
	}

	if (sessionLogFlg) {
		sessionLogFile.open("sessionlog.txt", ios::app);

		if (sessionLogFile) {
			sessionLogFile << endl;

			printDate(&sessionLogFile);
			sessionLogFile << ' ';
			printTime(&sessionLogFile);

			sessionLogFile << " Session start. Players: " << p1PlayerName << " vs " << p2PlayerName << endl;
		}
	}

	//念のため
	if( inputData.init() ) return 0xF;



	if( hTh075Th ){
		if( !WaitForSingleObject( hTh075Th, 0 ) ){
			CloseHandle( hTh075Th );
			hTh075Th = NULL;

			HWND hWnd = FindProcess();
			if( hWnd ){
				PostMessage(hWnd, WM_CLOSE, 0, 0);
				Sleep(3000);
			}
			//萃夢想のスレッドを開始する
			hCheckEvent = CreateEvent(NULL, FALSE, FALSE, NULL);
			hProcess = NULL;
			hTh075Th = (HANDLE)_beginthreadex(NULL, 0, th075Thread, this, 0, NULL);
			if( !hTh075Th ) return 1;

			WaitForSingleObject(hCheckEvent, 5000);
			CloseHandle( hCheckEvent );
			hCheckEvent = NULL;

			if( !hProcess ){
				CloseHandle( hTh075Th );
				hTh075Th = NULL;
				return 1;
			}
		}else{
			if( myInfo.terminalMode == mode_debug ){
				cout << "debug : th075thread already running" << endl;
			}

			HWND hWnd = FindProcess();
			if( !hWnd ){
				//終わりかけ
				if( WaitForSingleObject(hTh075Th, 3000) ){
					CloseHandle( pi.hThread );
					CloseHandle( pi.hProcess );
				}
				CloseHandle( hTh075Th );
				hTh075Th = NULL;
				cout << "debug : th075thread restart" << endl;

				hCheckEvent = CreateEvent(NULL, FALSE, FALSE, NULL);
				hProcess = NULL;
				hTh075Th = (HANDLE)_beginthreadex(NULL, 0, th075Thread, this, 0, NULL);
				if( !hTh075Th ) return 1;

				WaitForSingleObject(hCheckEvent, 5000);
				CloseHandle( hCheckEvent );
				hCheckEvent = NULL;

				if( !hProcess ){
					CloseHandle( hTh075Th );
					hTh075Th = NULL;
					return 1;
				}
			}
		}
	}else{
		HWND hWnd = FindProcess();
		if( hWnd ){
			DWORD PID;
			HANDLE hProcessTemp;
			GetWindowThreadProcessId( hWnd , &PID );
			hProcessTemp  = OpenProcess( PROCESS_ALL_ACCESS, FALSE, PID );

			PostMessage(hWnd, WM_CLOSE, 0, 0);
			WaitForSingleObject( hProcessTemp, 3000 );
			CloseHandle( hProcessTemp );
		}
		//萃夢想のスレッドを開始する
		hCheckEvent = CreateEvent(NULL, FALSE, FALSE, NULL);
		hProcess = NULL;
		hTh075Th = (HANDLE)_beginthreadex(NULL, 0, th075Thread, this, 0, NULL);
		if( !hTh075Th ) return 1;

		WaitForSingleObject(hCheckEvent, 5000);
		CloseHandle( hCheckEvent );
		hCheckEvent = NULL;

		if( !hProcess ){
			CloseHandle( hTh075Th );
			hTh075Th = NULL;
			return 1;
		}
	}


	return 0;
}

//バックグラウンド
int mainDatClass::backGroundRoop(){
	#if debug_mode
		WaitForSingleObject( hPrintMutex, INFINITE );
		cout << "debug : backGroundRoop()" << endl;
		ReleaseMutex( hPrintMutex );
	#endif


	//萃夢想のウィンドウを待つ
	for(;;){
		if( GetEsc() ){
			return 1;
		}
		if( FindProcess() ) break;
		Sleep(200);
	}

	//top
	hWnd = FindProcess();

	if( windowTopFlg ){
		if( hWnd ){
			SetWindowPos(hWnd, HWND_TOPMOST, 0, 0, 0, 0, SWP_NOSIZE | SWP_NOMOVE | SWP_SHOWWINDOW);
		}
	}

	toggleWindowTopFlg = 0;

	if ( disableScreensaverFlg ) {
		SystemParametersInfo(SPI_SETSCREENSAVEACTIVE, 0, 0, 0);
		SystemParametersInfo(SPI_SETLOWPOWERACTIVE, 0, 0, 0);
		SystemParametersInfo(SPI_SETPOWEROFFACTIVE, 0, 0, 0);
	}

	roopFlg = 1;
	for(;;){
		if( GetEsc() ) break;
		hWnd = FindProcess();
		if( !hWnd ) break;
		if( !roopFlg ) break;

		if (toggleWindowTopFlg) {
			if (windowTopFlg) {
				SetWindowPos(hWnd, HWND_NOTOPMOST, 0, 0, 0, 0, SWP_NOSIZE | SWP_NOMOVE | SWP_SHOWWINDOW);
				windowTopFlg = 0;
				cout << "setting: alwaysOnTop off" << endl;
			} else {
				SetWindowPos(hWnd, HWND_TOPMOST, 0, 0, 0, 0, SWP_NOSIZE | SWP_NOMOVE | SWP_SHOWWINDOW);
				windowTopFlg = 1;
				cout << "setting: alwaysOnTop on" << endl;
			}

			toggleWindowTopFlg = 0;
		}

		Sleep(200);

	}
	roopFlg = 0;

	if (winsA >= 1 || winsB >= 1) {
		for (int i = 0; i < 2; ++i) {
			ostream *stream;
			if (i == 0) {
				if (roundShowFlg < 1) {
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

			*stream << "Session results: " << dec << (int)(winsA + winsB) << " match";
			if ((winsA+winsB)>1) {
				*stream << "es";
			}

			*stream << ", " << (int)winsA << "(" << (int)roundsWonA << ") - " << (int)winsB << "(" << (int)roundsWonB << ") wins(rounds) (" << totalGameTime/60 << "s)" << endl;
		}
	}
	sessionLogFile.close();

	if ( disableScreensaverFlg ) {
		SystemParametersInfo(SPI_SETSCREENSAVEACTIVE, 1, 0, 0);
		SystemParametersInfo(SPI_SETLOWPOWERACTIVE, 1, 0, 0);
		SystemParametersInfo(SPI_SETPOWEROFFACTIVE, 1, 0, 0);
	}

	return 0;
}


