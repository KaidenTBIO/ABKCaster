#include "mainDatClass.h"
using namespace std;

//http://nienie.com/~masapico/doc_ApiSpy.html
//↑のページ「MASAPICO'S Page」を参考

int mainDatClass::th075Roop( DWORD* deInfo ){


	BYTE newCode = 0xCC;
	CONTEXT ct;

	*deInfo = 0;
	/* デバッグの継続 */
	if( deInitFlg ){
		if( !ContinueDebugEvent(de.dwProcessId, de.dwThreadId, ContinueStatus) ){
			WaitForSingleObject( hPrintMutex, INFINITE );
			cout << "ERROR : ContinueDebugEvent" << endl;
			ReleaseMutex( hPrintMutex );
			return 1;
		}
	}else{
		deInitFlg = 1;
	}
	if( !WaitForDebugEvent(&de, INFINITE) ){
		WaitForSingleObject( hPrintMutex, INFINITE );
		cout << "ERROR : WaitForDebugEvent" << endl;
		ReleaseMutex( hPrintMutex );
		return 1;
	}

	ContinueStatus = DBG_EXCEPTION_NOT_HANDLED;

	switch(de.dwDebugEventCode) {
	case CREATE_PROCESS_DEBUG_EVENT:
		th075Flg = 1;
		hProcessTh  = de.u.CreateProcessInfo.hThread;
		processThID = de.dwThreadId;

		//priority
		if( priorityFlg == 1 ){
			SetPriorityClass( hProcess, ABOVE_NORMAL_PRIORITY_CLASS );
		}
		if( priorityFlg == 2 ){
			SetPriorityClass( hProcess, HIGH_PRIORITY_CLASS );
		}

		SetBodyBreakPoint();
		SetCharBreakPoint();
		SetCode();

		FlushInstructionCache(pi.hProcess, NULL, 0);
		break;

	case EXIT_PROCESS_DEBUG_EVENT:
		th075Flg = 0;
		return 1;

	case EXCEPTION_DEBUG_EVENT: /* 例外発生 */
		switch(de.u.Exception.ExceptionRecord.ExceptionCode) {
        case EXCEPTION_ACCESS_VIOLATION:

            if (de.u.Exception.ExceptionRecord.ExceptionAddress > 0x0) {
                cout << "FATAL ERROR: Access violation at 0x" << hex << de.u.Exception.ExceptionRecord.ExceptionAddress << endl;
            }

            break;
		case EXCEPTION_BREAKPOINT:
			/* ブレークポイントに遭遇した場合 */
			ct.ContextFlags = CONTEXT_CONTROL;

			if( de.dwThreadId != processThID ){
				WaitForSingleObject( hPrintMutex, INFINITE );
				cout << "ERROR : processThID" << endl;
				ReleaseMutex( hPrintMutex );
				return 1;
			}
			if( !GetThreadContext(hProcessTh, &ct) ){
				WaitForSingleObject( hPrintMutex, INFINITE );
				cout << "ERROR : GetThreadContext" << endl;
				ReleaseMutex( hPrintMutex );
				return 1;
			}
			
			if(ct.Eip - 1 == body_int3_address) {
				*deInfo = de_body;

				updateKeybinds();
				
			} else if (ct.Eip -1 == body_int3_address + 1) {
				if (!rockFlg) {
					ct.ContextFlags = CONTEXT_INTEGER | CONTEXT_CONTROL;
					GetThreadContext(hProcessTh, &ct);
				
					ct.Esp -= 4;
				
					const DWORD zero = 0;
				
					WriteProcessMemory(pi.hProcess, (void *)ct.Esp, (void *)&zero, 4, 0);
					
					SetThreadContext(hProcessTh, &ct);
				
					FlushInstructionCache(pi.hProcess, NULL, 0);
				}
			}else if(ct.Eip - 1 == char_int3_address){
				*deInfo = de_char;

				ct.ContextFlags = CONTEXT_INTEGER;
				GetThreadContext(hProcessTh, &ct);
				
				ct.Eax += ct.Ecx;
				
				SetThreadContext(hProcessTh, &ct);

				FlushInstructionCache(pi.hProcess, NULL, 0);
			} else if (ct.Eip - 1 == input_int3_address) {
				*deInfo = de_input;

				datA.writeBodyInput();
				datB.writeBodyInput();
				datA.storeBodyInput();
				datB.storeBodyInput();
			} else if (ct.Eip - 1 == time_int3_address) {
				ct.ContextFlags = CONTEXT_INTEGER;
				GetThreadContext(hProcessTh, &ct);
				
				ct.Eax = 0x7fff;
				WriteProcessMemory(pi.hProcess, (void *)0x4ef288, (void *)&ct.Eax, 4, 0);
				
				SetThreadContext(hProcessTh, &ct);

				FlushInstructionCache(pi.hProcess, NULL, 0);
			} else if (ct.Eip - 1 == replay_int3_address) {
				runAutoSave();
			}

			//起動時に常に必要
			ContinueStatus = DBG_CONTINUE;

			break;

		case EXCEPTION_SINGLE_STEP: /* シングルステップ実行例外 */
			/* 再びブレークポイントを設置する */
			if( !WriteProcessMemory(pi.hProcess, (void*)body_int3_address, &newCode, 1, NULL) ){
				WaitForSingleObject( hPrintMutex, INFINITE );
				cout << "ERROR : Set new code ( body )" << endl;
				ReleaseMutex( hPrintMutex );
				return 1;
			}

			FlushInstructionCache(pi.hProcess, NULL, 0);
			/* シングルステップモードを中止 */
			ct.ContextFlags = CONTEXT_CONTROL;
			if(!GetThreadContext(hProcessTh, &ct)){
				WaitForSingleObject( hPrintMutex, INFINITE );
				cout << "ERROR : GetThreadContext ( single step )" << endl;
				ReleaseMutex( hPrintMutex );
				return 1;
			}
			ct.EFlags &= ~EFLAGS_TF;
			if(!SetThreadContext(hProcessTh, &ct)){
				WaitForSingleObject( hPrintMutex, INFINITE );
				cout << "ERROR : SetThreadContext ( single step )" << endl;
				ReleaseMutex( hPrintMutex );
				return 1;
			}

			ContinueStatus = DBG_CONTINUE;
			break;
		}
		break;
	}


	return 0;
}

