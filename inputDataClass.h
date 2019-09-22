#ifndef CASTER_INPUTDATA
#define CASTER_INPUTDATA

#if _MSC_VER >= 1300
#	if !defined(_WINDOWS_)
#		if !defined(NO_WIN32_LEAN_AND_MEAN)
#		define WIN32_LEAN_AND_MEAN
#		define  _VCL_LEAN_AND_MEAN
#		endif
#	endif
#	include <windows.h>
#	include <iostream>
#else
#	include <windows.h>
#	include <iostream.h>
#endif

#include "const.h"
#include "etc.h"

class inputDataSubClass{
	private:
	WORD* data;

	public:
	inputDataSubClass();
	~inputDataSubClass();
	int SetInputDataArea( DWORD, WORD*, WORD );
	void SetInputDataSub( DWORD, WORD );
	void SetInputDataA( DWORD, WORD );
	void SetInputDataB( DWORD, WORD );
	WORD* GetInputDataAddress( DWORD, WORD );
	int GetInputDataSub( DWORD, WORD* );
	int GetInputDataA( DWORD, WORD* );
	int GetInputDataB( DWORD, WORD* );
	int init();
	gameInfoStruct dataInfo;
};

class inputDataClass{
	private:
	inputDataSubClass inputDataSub[3];
	WORD now;
	WORD next;
	WORD prev;

	public:
	int	SetInputDataArea(WORD, DWORD, WORD*, WORD);
	void	SetInputData(WORD, DWORD, WORD, WORD);	//sessionNo, gameTime, playerSide, Input
	void	SetInputDataA(WORD, DWORD, WORD);
	void	SetInputDataB(WORD, DWORD, WORD);
	int	GetInputData(WORD, DWORD, WORD, WORD*);
	int	GetInputDataA(WORD, DWORD, WORD*);
	int	GetInputDataB(WORD, DWORD, WORD*);

	int Start( gameInfoStruct* );
	WORD* GetInputDataAddress( WORD, DWORD, WORD );
	int init();
	DWORD GetTime( WORD );
	void SetTime(WORD, DWORD);
};

#endif
