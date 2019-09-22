#ifndef CASTER_CONST
#define CASTER_CONST

//The total number of characters and stages
#define MAX_CHARACTERS 12
#define MAX_STAGES 28
#define COLOR_PAGE 6
#define MAX_COLORS 36

#define proc_id "AKATSUKI BLITZKAMPF / VERSION 1.1.3.1 SP1"
//Note: above is actually the beginning of the window title

#define memLoc_inputReadCall		0x401D3C
#define memLoc_inputLoop		0x403180

#define memLoc_p1Input			0x18F89EC
#define memLoc_p2Input			(memLoc_p1Input + 0x154)

#define memLoc_gameTime			0x4EF240
#define memLoc_rockTime			0x401CC7

#define memLoc_gamePhase		0xF53208

#define gamePhase_splash		0xFD5ED8
#define gamePhase_intro			0x1099458
#define gamePhase_menu			0x1099458
#define gamePhase_charsel		0x1107DB0
#define gamePhase_loading		0x1118B58
#define gamePhase_loading2		0x10969D8

#define gamePhase_postmatch		0xFE6D6C

#define memLoc_menuPointer		0xFD31FC
#define memLoc_menuSelection		0xFD2FC0

#define mainMenu_intro			0x2227C
#define mainMenu_title			0x2CAB0
#define mainMenu_menu			0x0DFA8
#define mainMenu_versus			0x11920

#define memLoc_p1Data			0x1C115EC
#define memLoc_p2Data			0x1C12760

//path
//(例)
#define th075_path "akatsukibk.exe\0"

//uMsg string
#define umsg_string "mbac_result"

//main
#define main_default	0
#define main_file		1
#define main_arg		2
#define main_wait		3
#define main_end		0xF


//buf_size
#define inputBuf_size	1024000
#define stask_buf_size	800
#define recv_buf_size	5120
#define z_buf_size	2048

//address
#define body_int3_address 0x401CC5
#define char_int3_address 0x41331F
#define input_int3_address 0x403180
#define time_int3_address 0x4028C8
#define replay_int3_address 0x402BDF

//シングルステップモードフラグ
#define EFLAGS_TF 0x00000100

//de
#define de_body	1
#define de_char	2
#define de_input 3

//timeout[ms]
#define timeout_time	10000

//auto_menu
#define auto_menu	1

//task
#define task_main		0
#define task_recv		1
#define task_manage	2

#define stask_data	1
#define stask_area	2

//debug_mode
#define debug_mode_main		0
#define debug_mode_mainRoop	0
#define debug_mode_thread	0
#define debug_mode_close 	0
#define debug_mode_func 	0

//status
#define status_default	0
#define status_ok		1
#define status_error	2
#define status_bad	3

//mode
#define mode_root		1
#define mode_branch	2
#define mode_subbranch	3
#define mode_broadcast	4
#define mode_access	5
#define mode_wait		6
#define mode_wait_target 7
#define mode_debug	9
#define mode_default	0

//phase
#define phase_none	0
#define phase_default	1
#define phase_menu	2
#define phase_read	3
#define phase_battle	4

//dest
#define dest_here 1
#define dest_away 2
#define dest_root 3
#define dest_branch 4
#define dest_subbranch 5
#define dest_leaf 6
#define dest_addr 7
#define dest_access 8

//header
#define cmd_version 0x81
#define cmd_space_1 1
#define cmd_space_2 0
#define cmd_space_3 0
#define cmd_version_error 0xFF

//command
#define cmd_echo		0
#define cmd_state		1
#define cmd_sendinput	2
#define cmd_time		3
#define cmd_gameInfo	4
#define cmd_dataInfo	5
#define cmd_input_req	6
#define cmd_reject	8
#define cmd_exit		9

#define res_echo		10
#define res_state		11
#define res_time		13
#define res_gameInfo	14
#define res_dataInfo	15
#define res_input_req	16

#define res_inputdata_area 17
#define cmd_inputdata_req 18
#define res_inputdata_req 19

#define cmd_access	20
#define res_access	23
#define cmd_delay		24
#define res_delay		25

#define cmd_continue	26
#define res_continue	27

#define cmd_rand		30
#define res_rand		31
#define cmd_playerside	32
#define res_playerside	33
#define cmd_delayobs	35
#define res_delayobs	36
#define cmd_session	37
#define res_session	38
#define cmd_initflg	40
#define res_initflg	41

#define cmd_join		45
#define res_join		46
#define cmd_cast		50

#define cmd_addr_branch 60
#define cmd_addr_leaf	61

#define cmd_standby	70
#define cmd_ready		71

#define res_inputdata_z	75

#define cmd_testport	80
#define res_testport	81

#define cmd_echoes	85
#define res_echoes	86

#define cmd_seek_leaf	87

// version identifier - fifth byte is protocol version, update when changed
#define mbcaster_protocol_version 0x02
static const char mbcaster_id[5] = { 'M', 'C', 0xa0, 0xa1, mbcaster_protocol_version };
static const char mbcaster_version_string[] = "090504";
static const char mbcaster_protocol_id = 1;

// keybind identifiers
enum {
    KEY_AUTOSAVE_ON,
    KEY_AUTOSAVE_OFF,
    KEY_AUTOSAVE_TOGGLE,
    KEY_ROUNDCOUNT_CYCLE,
    KEY_NOFAST_TOGGLE,
    KEY_ALWAYSONTOP_TOGGLE,
    KEY_DELAY1,
    KEY_DELAY2,
    KEY_DELAY3,
    KEY_DELAY4,
    KEY_DELAY5,
    KEY_DELAY6,
    KEY_DELAY7,
    KEY_DELAY8,
    KEY_DELAY9,
    KEY_DELAY10,
    KEY_COUNT
};

#endif
