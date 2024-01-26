#ifndef _SAFETY_H
#define _SAFETY_H
// #include "common.h"

/**********************************************************************************/
/*** 定義																		***/
/**********************************************************************************/
typedef enum _SF_RESULT{
	SF_NORMAL = 0,		/* 正常	*/
	SF_DISORDER			/* 異常	*/
} SF_RESULT;

/*** CRC計算パラメータ用構造体 ***/
typedef struct {
	unsigned long startAdr;  									/* 対象範囲先頭ｱﾄﾞﾚｽ 			*/
	unsigned long endAdr;   									/* 対象範囲末尾ｱﾄﾞﾚｽ			*/
	unsigned long cutSize;  									/* 計算分割ｻｲｽﾞ					*/
	unsigned short initCrc;  									/* CRC計算初期化値				*/
	unsigned long correctAdr;  									/* CRC格納ｱﾄﾞﾚｽ					*/
	void (*wdtRestart)(void);									/* WDTﾘｽﾀｰﾄ関数ﾎﾟｲﾝﾀ			*/
} SF_CRC_INIT_AREA;


/*** RAMチェックパラメータ用構造体 ***/
typedef struct {
	unsigned long startAdr;  									/* 対象範囲先頭ｱﾄﾞﾚｽ 			*/
	unsigned long endAdr;   									/* 対象範囲末尾ｱﾄﾞﾚｽ			*/
	void (*wdtRestart)(void);									/* WDTﾘｽﾀｰﾄ関数ﾎﾟｲﾝﾀ			*/
} SF_RAMCHECK_INIT_AREA;

typedef enum _SF_ERRNO{
	SF_ERRNO_UNDEFINED_INTERRUPTION = 0,	/* 未定義ベクタの割り込み発生			*/
	SF_ERRNO_TASKNO_ABNORMAL,				/* タスク番号が不正						*/
	SF_ERRNO_SP_ILVL_ABNORMAL,				/* スタックまたは割り込みレベルが不正	*/
	SF_ERRNO_MAIN_CLOCK_ABNORMAL,			/* メインクロック不正					*/
	SF_ERRNO_TIMER_INTERVAL,				/* タイマー割り込み周期不正				*/
	SF_ERRNO_MCU_REGISTER,					/* マイコン・レジスタ検査NG				*/
	SF_ERRNO_ROM_CHECK_STARTUP,				/* 内蔵ROM(コードフラッシュ)初期検査NG	*/
	SF_ERRNO_ROM_CHECK_PERIODIC,			/* 内蔵ROM(コードフラッシュ)定期検査NG	*/
	SF_ERRNO_RAM1_CHECK_STARTUP,			/* 内蔵RAM検査＆クリアNG				*/
	SF_ERRNO_RAM2_CHECK_STARTUP,			/* 内蔵拡張RAM検査＆クリアNG			*/
	SF_ERRNO_RAM1_CHECK_PERIODIC,			/* 内蔵RAM定期検査NG					*/
	SF_ERRNO_RAM2_CHECK_PERIODIC			/* 内蔵拡張RAM定期検査NG				*/
} SF_ERRNO;

/**********************************************************************************/
/*** グローバル変数																***/
/**********************************************************************************/


/**********************************************************************************/
/*** グローバルテーブル															***/
/**********************************************************************************/


/**********************************************************************************/
/*** グローバル関数																***/
/**********************************************************************************/
/*** ﾚｼﾞｽﾀ検査用	***/
SF_RESULT SFTestRegister( void );

/*** クロック検査用	***/
void SFTestCACInit( void );
void SFTestCACOverFlow( void );
void SFTest1msInit( void );

/*** PC検査用 ***/
SF_RESULT SFPCErrcheck(void);
void SFClearRunTaskNo(void);
void SFSetRunTaskNo(unsigned char no);
SF_RESULT SFTestRunTaskNo(unsigned char no);

/*** RAM検査用	***/
SF_RESULT SFRamCheckInit( SF_RAMCHECK_INIT_AREA *pram );
SF_RESULT SFRamCheckAndClear( void );
void SFTestPeriodicRAM_init(void);
SF_RESULT SFTestPeriodicRAM(void);

//[CO191]/*** ROM検査用	***/
//[CO191]SF_RESULT SFCreateCRC( SF_CRC_INIT_AREA *crcSet );
//[CO191]SF_RESULT SFTestFunctionalCRC( void );
//[CO191]SF_RESULT SFTestPeriodicCRC(void);
SF_RESULT SFTestPeriodicROM(void);


void Err_reset( SF_ERRNO );
void Init_SafetyFunction( void );
void FunctionalTest1(void);
void FunctionalTest2(void);

#endif