#include "mainDatClass.h"
using namespace std;

int mainDatClass::Communicate(){
	//アクセス相手がどんな状況か、など
	//myInfo.terminalMode = mode_root など
	
	accessFlg = status_default;
	
	char access_packet[5 + 1 + 1 + 1 + 20];
	int access_packet_len = 7;
	
	memcpy(access_packet, mbcaster_id, 5);
	access_packet[5] = mbcaster_protocol_id;
	access_packet[6] = 0;
	if (myPlayerName[0]) {
		access_packet[6] = 1;
		
		int name_len = strlen(myPlayerName);
		access_packet[7] = name_len;
		memcpy(access_packet+8, myPlayerName, name_len);
		
		access_packet_len += 1+name_len;
	}
	
	for(;;){
		SendCmd( dest_access, cmd_access, (void *)access_packet, access_packet_len );
		Sleep(100);
		if( accessFlg == status_ok ){
			//アクセスOK
			myInfo.terminalMode = mode_root;
			lastTime.Away = nowTime;
			WaitForSingleObject( hMutex, INFINITE );
			Away = Access;
			memset( &Access, 0, sizeof(Access) );
			ReleaseMutex( hMutex );
			break;
			
		}else  if( accessFlg == status_bad ){
			//アクセスBAD
//			cout << "STATUS : BAD ( Communicate() )" << endl;
			myInfo.terminalMode = mode_branch;
			lastTime.Root = nowTime;
			WaitForSingleObject( hMutex, INFINITE );
			Root = Access;
			memset( &Access, 0, sizeof(Access) );
			ReleaseMutex( hMutex );
			break;
			
		}else if( accessFlg == status_error ){
			cout << "Error ( version error? )" << endl;
			myInfo.terminalMode = mode_default;
			WaitForSingleObject( hMutex, INFINITE );
			memset( &Access, 0, sizeof(SOCKADDR_IN) );
			ReleaseMutex( hMutex );
			return 1;
			
		}
		if( !(Access.sin_addr.s_addr) ){
			cout << "ERROR : TIMEOUT? ( Access )" << endl;
			return 1;
		}
		if( GetEsc() ) return 1;
	}
	return 0;
}


