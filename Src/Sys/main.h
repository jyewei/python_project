#ifndef _MAIN_H
#define _MAIN_H




typedef enum _RUN_MODE 
{
	NORMAL_MODE,		//0:Normal mode	
	UNIT_CHECK_MODE,	//1:Commercial test mode
	BOX_CHECK_MODE,		//2:Electric box test mode
	SELF_CHECK_MODE		//3:self check mode

} RUN_MODE;


extern RUN_MODE	run_mode;	//action mode


#endif//_MAIN_H
