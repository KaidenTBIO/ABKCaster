#ifndef CASTER_MAINDAT
#define CASTER_MAINDAT

#if _MSC_VER >= 1300
#	if !defined(_WINDOWS_)
#		if !defined(NO_WIN32_LEAN_AND_MEAN)
#		define WIN32_LEAN_AND_MEAN
#		define  _VCL_LEAN_AND_MEAN
#		endif
#	endif
#	include <windows.h>
#	include <mmsystem.h>
#	include <process.h>
#	include <winsock2.h>
#	include <time.h>
#	include <io.h>

#	include <iostream>
#	include <iomanip>
#	include <fstream>
#else
#	include <windows.h>
#	include <process.h>
#	include <winsock2.h>
#	include <time.h>
#	include <io.h>

#	include <iostream.h>
#	include <iomanip.h>
#	include <fstream.h>
#endif

#include "charDatClass.h"
#include "inputDataClass.h"
#include "syncDataClass.h"
#include "etc.h"
#include "const.h"

class mainDatClass{
	public:

	PROCESS_INFORMATION pi;
	HANDLE hProcess;
	HANDLE hProcessTh;
	DWORD  processThID;

	WSADATA wsaData;
	SOCKET s;
	SOCKADDR_IN Here;
	SOCKADDR_IN Away;
	SOCKADDR_IN Root;
	SOCKADDR_IN Branch;
	SOCKADDR_IN subBranch;
	SOCKADDR_IN Leaf[4];
	SOCKADDR_IN Access;
	SOCKADDR_IN Ready;
	SOCKADDR_IN Standby[2];
	SOCKADDR_IN Remote;
	bool hasRemote;

	HANDLE hMutex;
	HANDLE hPrintMutex;
	HANDLE hRecvTh;
	HANDLE hSendTh;
	HANDLE hSendEvent;
	HANDLE hCheckEvent;
	HANDLE hTh075Th;
	HANDLE hManageTh;

	DWORD nowTime;
	lastTimeStruct lastTime;

	sTaskClass sTask[50];

	charDatClass datA;
	charDatClass datB;

	int mainDatInit(int, char**);
	int mainDatEnd();
	int charInit();
	int charEnd();

	int backGroundRoop();
	int mainRoop();
	int ManipMenu();
	int Entrance();
	int WaitForStart();
	int WaitForMenu();
	int mainStep();
	int th075Roop(DWORD*);

	int UnRockTime();
	int RockTime();
	int SetBodyBreakPoint();
	int RemoveBodyBreakPoint();

	int SetCode();
	int RemoveCode();
	int TerminateTh075();
	int th075Quit();

	int WriteCode(void*, BYTE);
	int ReadMemory(void*, void*, DWORD);
	int WriteMemory(void*, void*, DWORD);

	HWND FindProcess();
	void changeSpeed(int);
	void fixOptions();

	int SendData(int, void*, DWORD);
	int SendData(SOCKADDR_IN*, void*, DWORD);
	int SendDataR(int, void*, DWORD);
	int SendDataR(SOCKADDR_IN*, void*, DWORD);
	int SendDataSub(int, void*, DWORD, WORD);
	int SendDataSub(SOCKADDR_IN*, void*, DWORD, WORD);

	int SendData(int, void*, DWORD, void*, DWORD);
	int SendData(SOCKADDR_IN*, void*, DWORD, void*, DWORD);
	int SendDataR(int, void*, DWORD, void*, DWORD);
	int SendDataR(SOCKADDR_IN*, void*, DWORD, void*, DWORD);
	int SendDataSub(int, void*, DWORD, void*, DWORD, WORD);
	int SendDataSub(SOCKADDR_IN*, void*, DWORD, void*, DWORD, WORD);

	int SendArea(int, void*, DWORD);
	int SendArea(SOCKADDR_IN*, void*, DWORD);
	int SendAreaR(int, void*, DWORD);
	int SendAreaR(SOCKADDR_IN*, void*, DWORD);
	int SendAreaSub(int, void*, DWORD, WORD);
	int SendAreaSub(SOCKADDR_IN*, void*, DWORD, WORD);

	int SendCmd( int, BYTE );
	int SendCmd( SOCKADDR_IN*, BYTE );
	int SendCmdR( int, BYTE );
	int SendCmdR( SOCKADDR_IN*, BYTE );

	int SendCmd( int, BYTE, void*, DWORD );
	int SendCmd( SOCKADDR_IN*, BYTE, void*, DWORD );
	int SendCmdR( int, BYTE, void*, DWORD );
	int SendCmdR( SOCKADDR_IN*, BYTE, void*, DWORD );
	int SendCmdR( int, BYTE, void*, DWORD, void*, DWORD );
	int SendCmdR( SOCKADDR_IN*, BYTE, void*, DWORD, void*, DWORD );
	int SendDataR(int, void*, DWORD, void*, DWORD, void*, DWORD);
	int SendDataR(SOCKADDR_IN*, void*, DWORD, void*, DWORD, void*, DWORD);
	int SendDataSub(int, void*, DWORD, void*, DWORD, void*, DWORD, WORD);
	int SendDataSub(SOCKADDR_IN*, void*, DWORD, void*, DWORD, void*, DWORD, WORD);

	int SendCmdM( int, BYTE );
	int SendCmdM( SOCKADDR_IN*, BYTE );

	WORD roopFlg;
	WORD th075Flg;
	WORD continueFlg;
	WORD rockFlg;

	gameInfoStruct myInfo;
	gameInfoStruct enInfo;
	gameInfoStruct dataInfo;

	int GetEsc();
	int GetMyPort();
	int GetPlayerSide();
	int GetRand();
	int GetDelay( BYTE, float* );
	int Communicate();

	WORD delayTime;
	WORD myPort;
	WORD accessFlg;

	BYTE enRandNo;
	BYTE enRand;
	BYTE myRandNo;
	BYTE myRand;
	WORD delayTimeObsNo;
	float delayTimeObs[5];
	WORD playerSideRes;
	BYTE myInitFlg;
	BYTE enInitFlg;
	BYTE joinRes;
	echoFlgStruct echoFlg;

	int WaitForIni();
	int WaitForRoopFlg();

	inputDataClass inputData;
	float delay;
	time_t startTime;

	int SetRandBreakPoint();
	int RemoveRandBreakPoint();

	int SetCharBreakPoint();
	int RemoveCharBreakPoint();
	WORD toughModeFlg;

	int SetRoopBreakPoint();
	int RemoveRoopBreakPoint();

	WORD autoNextFlg;

	WORD deInitFlg;
	DEBUG_EVENT de;
	DWORD ContinueStatus;

	WORD delayTimeA;
	WORD delayTimeB;

	int TestPort( SOCKADDR_IN* );
	WORD testPortFlg;

	WORD stdbyPort[2];
	WORD readyPort;

	WORD mainFlg;
	char iniPath[200];
	WORD enPort;
	WORD beepFlg;
	WORD waveFlg;
	unsigned long waitTargetIP;
	WORD endTimeFlg;
	WORD escSelectFlg;
	WORD lessCastFlg;
	WORD th075QuitFlg;
	WORD zlibFlg;
	WORD priorityFlg;
	WORD windowModeFlg;
	WORD stageLimitCancelFlg;
	WORD perfectFreezeFlg;

	WORD obsCountFlg;
	DWORD obsIP[64];
	WORD autoNextOnFlg;
	UINT uMsg;
	BYTE targetMode;
	char nowDir[200];
	WORD autoWaitFlg;
	WORD autoWaitOnFlg;
	WORD replaySaveFlg;
	WORD playerSideFlg;
	WORD playerSideHostFlg;
	WORD allowObsFlg;
	WORD windowTopFlg;
	WORD deSyncSoundFlg;

	struct {
		HANDLE  th;
	} Threads[20];
	int nThreads;
	HANDLE handle;

	argDataStruct argData;
	syncDataClass syncData;
	WORD accessPort;
	char accessIP[80];
	char standbyIP[80];
	WORD lastMode;

	BYTE input_data[ 4 * 12 ];
	char * route;

	DWORD prevPhase;
	DWORD fixCfgFlg;

	DWORD lastGameTime;
	DWORD totalGameTime;

	BYTE lastCharacterA;
	BYTE lastCharacterB;

	WORD winsA;
	WORD roundsWonA;
	WORD winsB;
	WORD roundsWonB;

	WORD systemTimeFlg;
	WORD noFastFlg;
	WORD disableScreensaverFlg;
	WORD autoSaveFlg;
	WORD roundShowFlg;
	WORD sessionLogFlg;

	DWORD windowWidth;
	DWORD windowHeight;

	int povSensitivity;

	ofstream sessionLogFile;

	char replayFilenameFormat[80];

	WORD toggleWindowTopFlg;

	WORD keybinds[KEY_COUNT];
	BYTE keystate[KEY_COUNT];

	HWND hWnd;

	WORD practiceModeFlg;

	void runKeybind(int key);
	void updateKeybinds();

	void runAutoSave();

	const char *getCharacterName(int ch);
	const char *getCharacterShortName(int ch);

	unsigned long GetAddress(char *name, unsigned short *port);

	void printDate(ostream *stream);
	void printTime(ostream *stream);
	
	char myPlayerName[21];
	char p1PlayerName[21];
	char p2PlayerName[21];
	bool myTempName;
	bool p1TempName;
	bool p2TempName;
	bool unknownNameFlg;
	int nPlayers;
	bool namesLocked;
	bool anonymousObsFlg;
	
	bool forceJoystick;
	
	int remoteProtocolVersion;
	
	void cleanString(char *string, bool spaces_okay);
};

#endif
