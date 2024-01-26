/*--------------------------------------------------------------------*/
/*! @file	Safety_Function.c
 *	@brief	安全機能
 *
 *	@note	Copyright(C) 2016 Hitachi Appliances, Inc. All Rights Reserved.
 */
/*---------------------------------------------------------------------
 *	履歴
 *	2016.11.22	SSXCH01	a.kinoshita		新規
 *	2020.06.24	CO191	y.yamamoto		Gen3SFVRFから流用
 *--------------------------------------------------------------------*/
/*------------------------------------------------------------------*/
/* 定義ファイル														*/
/*------------------------------------------------------------------*/
#include "safety_function.h"
#include "iodefine.h"
#include "cpu_test.h"


/*--------------------------------------------------------------------*/
/*		ROMサム調整値格納領域										  */
/*--------------------------------------------------------------------*/
#pragma section SUM
#ifdef RELEASE						/* 量産用									*/
// const uint8_t RomSum = 0x54U;			/* FunctionalTest2()でsum = 0となる調整値	*/
#else								/* 開発用									*/
// const uint8_t RomSum = 0xFFU;
#endif
#pragma section

/*--------------------------------------------------------------------*/
/* 外部変数の定義													  */
/*--------------------------------------------------------------------*/
static unsigned char ucRunTaskNo = 0xffU;

/*--------------------------------------------------------------------*/
/* 内部定数の定義													  */
/*--------------------------------------------------------------------*/
//#define MEMORY_MAX			(0xffffffffUL)

#define ROM_SIZE				(0x00180000)		/* R5F5651CDDFP 1.5MB			*/

#define ROM_START_ADDR			(0xFE7F5D00)		/* 内蔵ROM						*/
#define ROM_END_ADDR			(0xFE7F5D7F)		/*  (オプション設定メモリ)		*/

#define ROM_START_ADDR1			(0xFFE80000)		/* 内蔵ROM						*/
#define ROM_END_ADDR1			(0xFFFFFFFF)		/* (コードフラッシュメモリ)		*/

#define RAM_START_ADDR			(0x00000000)		/* RAM start addr: 0x00000000	*/
#define RAM_END_ADDR			(0x0003FFFF)		/* RAM start addr: 0x0003FFFF	*/

#define RAM_START_ADDR1			(0x00800000)		/* RAM start addr: 0x00800000	*/
#define RAM_END_ADDR1			(0x0085FFFF)		/* RAM start addr: 0x0085FFFF	*/

#define	RAM_CHECK_DATA1			((uint8_t)0x55)
#define	RAM_CHECK_DATA2			((uint8_t)0xAA)

#define	RAMCHK2DATA1_NUM		(0x55555555)		/* RAMチェック用データ1         */
#define	RAMCHK2DATA2_NUM		(0xAAAAAAAA)		/* RAMチェック用データ2         */

#define	INTRUPUCHECKTIME		(1562)				/* 割り込み時間1ms (CMT3ｶｳﾝﾄ)	*/
#define	INTRUPUCHECKCOUNT		(5)					/* 割り込み計測時間5ms 			*/

#define	STACK_SU				(uint32_t)(__secsize("SU"))	/* ユーザスタックサイズ		*/
#define	STACK_SI				(uint32_t)(__secsize("SI"))	/* 割込みスタックサイズ		*/

#define	get_isp					__get_isp
#define	get_psw					__get_psw

/*--------------------------------------------------------------------*/
/* 構造体の型定義													  */
/*--------------------------------------------------------------------*/
//[CO191]/*** 定期ROM検査 ﾊｰﾄﾞｳｪｱCRC計算 状態定義 ***/
//[CO191]typedef enum _CRC_STATUS{
//[CO191]	CRC_NOT_CREATE = 0,								/* 準備失敗						*/
//[CO191]	CRC_INIT_STATUS,								/* 初期状態 					*/
//[CO191]	CRC_CALUCULATE_STATUS,							/* 計算中						*/
//[CO191]	CRC_FINISH_STATUS								/* 計算終了						*/
//[CO191]} CRC_STATUS;
//[CO191]
//[CO191]typedef struct {
//[CO191]   unsigned long length;
//[CO191]   unsigned char *start_address;
//[CO191]} CHECKSUM_CRC_TEST_AREA;
typedef enum _ROM_STATUS{
	ROM_INIT_STATUS = 0,							/* 初期状態 					*/
	ROM_CALUCULATE_STATUS,							/* 計算中						*/
	ROM_FINISH_STATUS								/* 計算終了						*/
} ROM_STATUS;

/*--------------------------------------------------------------------*/
/* 内部変数の定義													  */
/*--------------------------------------------------------------------*/
static SF_RAMCHECK_INIT_AREA	ramInitInfo;
static SF_RAMCHECK_INIT_AREA	ramInitInfo1;
static volatile unsigned long	*RamChk_AddrP_32u;		/* チェックアドレス格納(ポインタ)   */
static unsigned long	RamChk_DataBuff_32u;			/* チェック領域データ退避           */
static volatile unsigned long	*RamChk_AddrP_32u1;		/* チェックアドレス格納(ポインタ)   */
static unsigned long	RamChk_DataBuff_32u1;			/* チェック領域データ退避           */

//[CO191]static SF_CRC_INIT_AREA crcInitInfo;					/* オプション設定領域				*/
//[CO191]static SF_CRC_INIT_AREA crcInitInfo1;					/* コードフラッシュ領域				*/
//[CO191]static CHECKSUM_CRC_TEST_AREA checksumCRC;
//[CO191]static CRC_STATUS crcStatus = CRC_NOT_CREATE;
//[CO191]static CRC_STATUS crcStatus1 = CRC_NOT_CREATE;
static ROM_STATUS romStatus = ROM_INIT_STATUS;			/* ROMテスト状態を初期状態へ	*/
static unsigned char ramStatus = 0;
static unsigned char ucclockerrCount;						/* 周波数測定ｴﾗｰｶｳﾝﾄ変数	*/
static unsigned char uc1mserrCount;						/* 1msﾀｲﾏ割込みｴﾗｰｶｳﾝﾄ変数	*/

/*------------------------------------------------------------------*/
/* 関数プロトタイプ宣言												*/
/*------------------------------------------------------------------*/
//[CO191]static unsigned long GetEnd( unsigned long checkStart, unsigned long size, unsigned long endAdr );
//[CO191]static unsigned long GetEnd1( unsigned long checkStart, unsigned long size, unsigned long endAdr );
//[CO191]static unsigned short UpdateCRC( unsigned long checkStart, unsigned long checkEnd, unsigned short startCrc );
//[CO191]static SF_RESULT CompareCRC( unsigned short crc, unsigned long correctAdr );
//[CO191]static void CRC_Start(void);
//[CO191]static void CRC_AddRange(unsigned char *Data, long Length);
//[CO191]static unsigned short CRC_Result(void);

/**********************************************************************************/
/*** モジュール個別外部参照関数													***/
/**********************************************************************************/
/*** ルネサスセルフテストライブラリ ***/
//[CO191]extern	unsigned char	Test_GeneralA (void);			/* 汎用ﾚｼﾞｽﾀ(呼出前後で値を保証しない)ﾁｪｯｸ処理		*/
//[CO191]extern	unsigned char	Test_GeneralB (void);		/* 汎用ﾚｼﾞｽﾀ(呼出前後で値を保証する)ﾁｪｯｸ処理		*/
//[CO191]extern	unsigned char	Test_Control (void);		/* 制御ﾚｼﾞｽﾀﾁｪｯｸ処理		*/
//[CO191]extern	unsigned char	Test_Accumulator (void);		/* ｱｷｭﾑﾚｰﾀﾁｪｯｸ処理		*/
extern	uint32_t CPU_Test_All( void );

/*--------------------------------------------------------------------------------------*/
/*!	レジスタテスト
 *	@param	なし
 *	@return	判定結果 (SF_NORMAL：正常、SF_DISORDER：異常)
 *	@note	
 */
/*--------------------------------------------------------------------------------------*/
/*  履歴
 *	2015.06.15  a.kato  新規
 *	2020.06.24	CO191	y.yamamoto		ルネサス IEC60730 Self Test Code 利用
 */
/*--------------------------------------------------------------------------------------*/
SF_RESULT SFTestRegister( void )
{
	SF_RESULT err = SF_NORMAL;

//[CO191]	if( (Test_GeneralA() != 0U) 		/* 汎用ﾚｼﾞｽﾀ(呼出前後で値を保証しない)ﾁｪｯｸ処理	*/
//[CO191]	 || (Test_GeneralB() != 0U)			/* 汎用ﾚｼﾞｽﾀ(呼出前後で値を保証する)ﾁｪｯｸ処理	*/
//[CO191]	 || (Test_Control() != 0U) 			/* 制御ﾚｼﾞｽﾀﾁｪｯｸ処理							*/
//[CO191]	 || (Test_Accumulator() != 0U) ){		/* ｱｷｭﾑﾚｰﾀﾁｪｯｸ処理							*/
	if( CPU_Test_All() > 0U ){
		err = SF_DISORDER;
	} else {
		err = SF_NORMAL;
	}
	
	return err;
}

/*--------------------------------------------------------------------*/
/*! 暴走監視処理(SPﾁｪｯｸ他)
 *	@param
 *	@return	判定結果 (SF_NORMAL：正常、SF_DISORDER：異常)
 *	@note	マイコン暴走監視用の処理をまとめて記述
 *	@note	メインループの先頭で呼出すこと。以下の場合には
 *	@note	マイコン暴走として強制リセットをかける		
 *	@note	①スタック使用量のチェック					
 *	@note	   スタック使用量が異常な場合				
 *	@note	②割込み許可レベルのチェック				
 *	@note	   割込み許可レベルが異常値な場合			
 */
/*--------------------------------------------------------------------
 *	履歴
 *	2016.12.19	SSXCH04	a.kinoshita		PV151_CC_11_00(P-4452)流用
 *	2019.01.14			M.Morooka		SSXCH_21(P-4608)流用
 *--------------------------------------------------------------------*/
SF_RESULT SFPCErrcheck(void)
{
	unsigned long wk_isp;
	unsigned long wk_psw;
	SF_RESULT err = SF_NORMAL;

	DI_ORG;

	wk_isp = (unsigned long)get_isp();
	wk_psw =(get_psw()&0x0F000000) >> 24;
	if( ( (STACK_SI + STACK_SU - 0x180) > wk_isp )
	|| (wk_psw > 0)){						/* 割り込み優先レベルが0より小さい 時	*/
		err = SF_DISORDER;					/* 異常									*/
	}
	EI_ORG;
	return err;
}

/*--------------------------------------------------------------------*/
/*! 実行タスク番号初期化
 *	@param
 *	@return
 *	@note
 */
/*--------------------------------------------------------------------
 *	履歴
 *	2016.12.19	SSXCH04	a.kinoshita		マルチ流用
 *	2019.01.14			M.Morooka		SSXCH_21(P-4608)流用
 *--------------------------------------------------------------------*/
void SFClearRunTaskNo(void)
{
	ucRunTaskNo = 0xffU;						/* ﾀｽｸNo初期化					*/
}

/*--------------------------------------------------------------------*/
/*! 実行タスク番号セット
 *	@param	no	タスク番号
 *	@return
 *	@note
 */
/*--------------------------------------------------------------------
 *	履歴
 *	2016.12.19	SSXCH04	a.kinoshita		マルチ流用
 *	2019.01.14			M.Morooka		SSXCH_21(P-4608)流用
 *--------------------------------------------------------------------*/
void SFSetRunTaskNo(uint8_t no)
{
	ucRunTaskNo = no;							/* ﾀｽｸNoｾｯﾄ						*/
}

/*--------------------------------------------------------------------*/
/*! 実行タスク番号テスト
 *	@param	no	タスク番号
 *	@return	判定結果(SF_NORMAL：正常、SF_DISORDER：異常)
 *	@note
 */
/*--------------------------------------------------------------------
 *	履歴
 *	2016.12.19	SSXCH04	a.kinoshita		マルチ流用
 *	2019.01.14			M.Morooka		SSXCH_21(P-4608)流用
 *--------------------------------------------------------------------*/
SF_RESULT SFTestRunTaskNo(uint8_t no)
{
	SF_RESULT err = SF_NORMAL;

	if(no != ucRunTaskNo){						/* ﾀｽｸNo不一致 ならば			*/
		err = SF_DISORDER;						/* 異常ｾｯﾄ						*/
	}
	else{										/* ﾀｽｸNo正常 ならば				*/
		err = SF_NORMAL;						/* 正常ｾｯﾄ						*/
	}
	
	return err;
}

/*--------------------------------------------------------------------*/
/*!	CAC周波数エラーチェック
 *	@param	なし
 *	@return	判定結果 (SF_NORMAL：正常、SF_DISORDER：異常)
 *	@note	
 */
/*--------------------------------------------------------------------
 *	履歴
 *	2016.12.19	SSXCH04	a.kinoshita		新規
 *	2019.01.14			M.Morooka		SSXCH_21(P-4608)流用
 *--------------------------------------------------------------------*/
void SFTestCACInit( void )
{
	CAC.CAICR.BIT.MENDFCL = 1;				/*割り込みフラグクリア				*/
	
	if (CAC.CASTR.BIT.FERRF == 1) {			/* 周波数エラー						*/
		ucclockerrCount++;					/* エラーカウント変数インクリメント	*/
		CAC.CAICR.BIT.FERRFCL = 1;			/* エラーフラグクリア				*/
	} 
	else {
		ucclockerrCount =0;					/* エラーカウントクリア				*/
	}

	if(ucclockerrCount >=5){
		Err_reset( SF_ERRNO_MAIN_CLOCK_ABNORMAL );	/* エラーリセット			*/
	}

}

/*--------------------------------------------------------------------*/
/*!	CACオーバーフロー割り込み
 *	@param	なし
 *	@return	
 *	@note	オーバーフロー割り込み時はIWDTカウントが異常のため
 *	@note	ソフトウェアリセット
 */
/*--------------------------------------------------------------------
 *	履歴
 *	2016.12.19	SSXCH04	a.kinoshita		新規
 *	2019.01.14			M.Morooka		SSXCH_21(P-4608)流用
 *--------------------------------------------------------------------*/
void SFTestCACOverFlow( void )
{
	CAC.CAICR.BIT.OVFFCL = 1;			/*割り込みフラグクリア				*/
	Err_reset( SF_ERRNO_MAIN_CLOCK_ABNORMAL );	/* エラーリセット			*/
}

/*--------------------------------------------------------------------*/
/*!	1msタイマ割込みテスト
 *	@param	なし
 *	@return	判定結果 (SF_NORMAL：正常、SF_DISORDER：異常)
 *	@note	ソフトウェアリセット
 */
/*--------------------------------------------------------------------
 *	履歴
 *	2016.12.19	SSXCH04	a.kinoshita		新規
 *	2019.01.14			M.Morooka		SSXCH_21(P-4608)流用
 *--------------------------------------------------------------------*/
void SFTest1msInit( void )
{
	static uint8_t	startflg = 0;
	static uint8_t	count5ms;
	static uint16_t	backtim;					/* 経過時間算出用ワーク             */
	uint16_t		timenow;
	int32_t		wk_time;

	if( startflg == 0 ){					/* 初期処理未実施					*/
		startflg = 1;						/* 初期処理実施済み					*/
		count5ms = 0;						/* 5msカウンタクリア				*/
		backtim = CMT3.CMCNT;				/* 前回値の初期化		 			*/
		return;								/* 初回の割り込みでは初期化のみ		*/
	}

	timenow = CMT3.CMCNT;					/* タイマをワークに格納 			*/
	count5ms++;
	if(count5ms >=5){
		count5ms = 0;
		if(backtim > timenow){	/* カウンタが逆転したらオーバーフローしたと判断 */
			wk_time = (int32_t)(0xFFFF - backtim + timenow)/INTRUPUCHECKTIME;
		}
		else{
			wk_time = (int32_t)(timenow - backtim)/INTRUPUCHECKTIME;
		}
		
/* Chg-start 2017.05.09 a.kinoshita SSXCH_19 */
//		if( (wk_time >=  INTRUPUCHECKCOUNT+2 )
//		 || (wk_time <=  INTRUPUCHECKCOUNT-2) ){	/* 計測値/基準が正常値±2の場合		*/
		if( (wk_time >  INTRUPUCHECKCOUNT+2 )
		 || (wk_time <  INTRUPUCHECKCOUNT-2) ){		/* 計測値/基準が正常値±2外の場合		*/
/* Chg-end   2017.05.09 a.kinoshita SSXCH_19 */
			uc1mserrCount++;						/* エラーカウント変数インクリメント	*/
		}
		else{
			uc1mserrCount = 0;						/* エラーカウントクリア				*/
		}
		
		if(uc1mserrCount>=5){						/* 5回連続で異常					*/
			Err_reset( SF_ERRNO_TIMER_INTERVAL );	/* エラーリセット			*/
		}
		
		backtim = timenow;
	}
	

}

/*--------------------------------------------------------------------*/
/*!	内蔵RAM判定準備
 *	@param	初期値構造体
 *	@return	設定異常有無 (SF_NORMAL：無、SF_DISORDER：有)
 *	@note	
 */
/*--------------------------------------------------------------------
 *	履歴
 *	2016.12.19	SSXCH04	a.kinoshita		マルチ流用
 *--------------------------------------------------------------------*/
SF_RESULT SFRamCheckInit( SF_RAMCHECK_INIT_AREA *pram )
{
	SF_RESULT err = SF_NORMAL;
	
	/* IARではFFFF FFFFhとの比較(<、>両方とも)は「Pa084」のワーニングとなるため、コメントアウトする。 */
	if ((pram == NULL)
//	|| (pram ->startAdr > MEMORY_MAX)					/* 先頭ｱﾄﾞﾚｽがﾒﾓﾘ範囲外	*/
//	|| (pram ->endAdr > MEMORY_MAX)						/* 末尾ｱﾄﾞﾚｽがﾒﾓﾘ範囲外	*/
	|| (pram ->startAdr > pram ->endAdr) ){		 		/* 先頭 > 末尾			*/
		err = SF_DISORDER;								/* 準備失敗				*/
		ramStatus = 0;									/* →準備失敗					*/
	} else {
		ramInitInfo.startAdr = pram->startAdr;			/* 対象先頭ｱﾄﾞﾚｽ				*/
		ramInitInfo.endAdr = pram->endAdr;				/* 対象末尾ｱﾄﾞﾚｽ				*/
		if( ramInitInfo.wdtRestart != NULL ){
			ramInitInfo.wdtRestart = pram->wdtRestart;	/* WDTﾘｽﾀｰﾄ				*/
		}else{
			/* nothing */
		}
		err = SF_NORMAL;
		ramStatus = 1;									/* →準備ＯＫ					*/
	}
	
	return err;
}

/*--------------------------------------------------------------------*/
/*!	内蔵拡張RAM判定準備
 *	@param	初期値構造体
 *	@return	設定異常有無 (SF_NORMAL：無、SF_DISORDER：有)
 *	@note	CO191では内蔵拡張RAMを使わないが、起動時の検査とクリアは実施
 */
/*--------------------------------------------------------------------
 *	履歴
 *	2020.1.22	M.Morooka		CHソフト流用
 *--------------------------------------------------------------------*/
SF_RESULT SFRamCheckInit1( SF_RAMCHECK_INIT_AREA *pram )
{
	SF_RESULT err = SF_NORMAL;
	
	/* IARではFFFF FFFFhとの比較(<、>両方とも)は「Pa084」のワーニングとなるため、コメントアウトする。 */
	if ((pram == NULL)
//	|| (pram ->startAdr > MEMORY_MAX)					/* 先頭ｱﾄﾞﾚｽがﾒﾓﾘ範囲外	*/
//	|| (pram ->endAdr > MEMORY_MAX)						/* 末尾ｱﾄﾞﾚｽがﾒﾓﾘ範囲外	*/
	|| (pram ->startAdr > pram ->endAdr) ){		 		/* 先頭 > 末尾			*/
		err = SF_DISORDER;								/* 準備失敗				*/
		ramStatus = 0;									/* →準備失敗					*/
	} else {
		ramInitInfo1.startAdr = pram->startAdr;			/* 対象先頭ｱﾄﾞﾚｽ				*/
		ramInitInfo1.endAdr = pram->endAdr;				/* 対象末尾ｱﾄﾞﾚｽ				*/
		if( ramInitInfo1.wdtRestart != NULL ){
			ramInitInfo1.wdtRestart = pram->wdtRestart;	/* WDTﾘｽﾀｰﾄ				*/
		}else{
			/* nothing */
		}
		err = SF_NORMAL;
		ramStatus = 1;									/* →準備ＯＫ					*/
	}
	
	return err;
}

/*--------------------------------------------------------------------*/
/*! 内蔵RAMのチェック 機能テスト
 *	@param	*pTop：先頭アドレス
 *	@param	*pEnd：サイズ(バイト数)
 *	@return	判定結果 (SF_NORMAL：正常、SF_DISORDER：異常)
 *	@note	1.チェックアドレスをワークへ取り込み
 *	@note	2.チェックアドレス(ワーク)に0x55を書き込み
 *	@note	3.チェックアドレス(ワーク)と0x55を比較
 *	@note	4.チェックアドレス(ワーク)を1ビット左シフト
 *	@note	5.チェックアドレス(ワーク)と0xAAを比較
 *	@note	6.チェックアドレス(ワーク)を0クリア
 *	@note	7.チェックアドレスを更新
 */
/*--------------------------------------------------------------------
 *	履歴
 *	2016.12.19	SSXCH04	a.kinoshita		PV151_CC_11_00(P-4452)流用
 *--------------------------------------------------------------------*/
SF_RESULT SFRamCheckAndClear(void)
{
	SF_RESULT err = SF_NORMAL;
	uint8_t		*p;
	uint32_t		i;
	uint32_t		size;
	

	if (ramStatus == 0){								/* →準備失敗					*/
		err = SF_DISORDER;									/* 準備失敗					*/
	} else {
		//[CO191] R_Config_IWDT_Restart();				/* 照合エラーが連続した場合に	*/
														/* 備え最初にWDTクリア			*/
		p = (unsigned char *) ramInitInfo.startAdr;		/* get RAM addr 				*/
		size = ramInitInfo.endAdr - ramInitInfo.startAdr + 1;	/* RAMチェックｻｲｽﾞ取得	*/
		for( i = 0; i< size; i++ ){
			*p = RAM_CHECK_DATA1;						/* 固定データ１を書込み			*/
			if( *p == RAM_CHECK_DATA1 ){				/* 書込み／読出しデータ一致？	*/
				*p <<= 1;					/* Left shift 1bit: 01010101B -> 10101010B */
				if( *p != RAM_CHECK_DATA2 ){			/* 書込み／読出しデータ一致？	*/
					err = SF_DISORDER;					/* 異常							*/
					break;
				}
				else{
					*p = 0;								/* ０クリア						*/
		//[CO191]	if( (i & 0x000fL) == 0 ){			/* 16バイト毎WDTをクリア		*/
		//[CO191]		R_Config_IWDT_Restart();
		//[CO191]	}
				}
				p++;
			}
			else{										/* 書込み／読出しデータ不一致	*/
				err = SF_DISORDER;						/* 異常							*/
				break;
			}
		}
	}
	return err;

}

/*--------------------------------------------------------------------*/
/*! 内蔵拡張RAMのチェック 機能テスト
 *	@param	*pTop：先頭アドレス
 *	@param	*pEnd：サイズ(バイト数)
 *	@return	判定結果 (SF_NORMAL：正常、SF_DISORDER：異常)
 *	@note	1.チェックアドレスをワークへ取り込み
 *	@note	2.チェックアドレス(ワーク)に0x55を書き込み
 *	@note	3.チェックアドレス(ワーク)と0x55を比較
 *	@note	4.チェックアドレス(ワーク)を1ビット左シフト
 *	@note	5.チェックアドレス(ワーク)と0xAAを比較
 *	@note	6.チェックアドレス(ワーク)を0クリア
 *	@note	7.チェックアドレスを更新
 */
/*--------------------------------------------------------------------
 *	履歴
 *	2020.1.22	M.Morooka		CHソフト流用
 *--------------------------------------------------------------------*/
SF_RESULT SFRamCheckAndClear1(void)
{
	SF_RESULT err = SF_NORMAL;
	uint8_t		*p;
	uint32_t		i;
	uint32_t		size;
	

	if (ramStatus == 0){								/* →準備失敗					*/
		err = SF_DISORDER;									/* 準備失敗					*/
	} else {
		//[CO191] R_Config_IWDT_Restart();				/* 照合エラーが連続した場合に	*/
														/* 備え最初にWDTクリア			*/
		p = (unsigned char *) ramInitInfo1.startAdr;	/* get RAM addr 				*/
		size = ramInitInfo1.endAdr - ramInitInfo1.startAdr + 1;	/* RAMチェックｻｲｽﾞ取得	*/
		for( i = 0; i< size; i++ ){
			*p = RAM_CHECK_DATA1;						/* 固定データ１を書込み			*/
			if( *p == RAM_CHECK_DATA1 ){				/* 書込み／読出しデータ一致？	*/
				*p <<= 1;					/* Left shift 1bit: 01010101B -> 10101010B */
				if( *p != RAM_CHECK_DATA2 ){			/* 書込み／読出しデータ一致？	*/
					err = SF_DISORDER;					/* 異常							*/
					break;
				}
				else{
					*p = 0;								/* ０クリア						*/
		//[CO191]	if( (i & 0x000fL) == 0 ){			/* 16バイト毎WDTをクリア		*/
		//[CO191]		R_Config_IWDT_Restart();
		//[CO191]	}
				}
				p++;
			}
			else{										/* 書込み／読出しデータ不一致	*/
				err = SF_DISORDER;						/* 異常							*/
				break;
			}
		}
	}
	return err;

}

/*--------------------------------------------------------------------*/
/*! RAMチェック処理インバータ初期化時処理(内蔵RAM)
 *	@param	
 *	@return	
 *	@note
 */
/*--------------------------------------------------------------------
 *	履歴
 *	2016.12.03	SSXCH04	a.kinoshita		PV151_CC_11_00(P-4452)流用
 *--------------------------------------------------------------------*/
void SFTestPeriodicRAM_init(void)
{
													/* チェックアドレス格納(ポインタ)   */
	RamChk_AddrP_32u = (unsigned long *)ramInitInfo.startAdr;
	RamChk_DataBuff_32u = 0;						/* チェック領域データ退避           */
}

/*--------------------------------------------------------------------*/
/*! RAMチェック処理インバータ初期化時処理(内蔵拡張RAM)
 *	@param	
 *	@return	
 *	@note
 */
/*--------------------------------------------------------------------
 *	履歴
 *	2020.1.22	M.Morooka		CHソフト流用
 *--------------------------------------------------------------------*/
void SFTestPeriodicRAM_init1(void)
{
													/* チェックアドレス格納(ポインタ)   */
	RamChk_AddrP_32u1 = (unsigned long *)ramInitInfo1.startAdr;
	RamChk_DataBuff_32u1 = 0;						/* チェック領域データ退避           */
}

/*--------------------------------------------------------------------*/
/*! 内蔵RAMのチェック(定期テスト)
 *	@param	
 *	@return	判定結果(SF_NORMAL：正常、SF_DISORDER：異常)
 *	@note	固定データ(55h,AAh,00h)書込み後に読み出して照合。
 *	@note	 指定範囲が全て正常になるまで無限ループ。
 *	@note	 1回の処理の時間を短縮するため、１アドレス(４バイト)ずつ
 */
/*--------------------------------------------------------------------
 *	履歴
 *	2016.12.19	SSXCH04	a.kinoshita		PV151_CC_11_00(P-4452)流用
 *	2020.06.24	CO191	y.yamamoto		検査ができない対象外変数を追加
 *--------------------------------------------------------------------*/
SF_RESULT SFTestPeriodicRAM(void)
{
	volatile unsigned long	*wk_chk_addr_32u;				/* チェックRAMアドレス  */
	SF_RESULT err = SF_NORMAL;

	/* --------------------------------- */
	/* 対象外の領域                      */
	/* --------------------------------- */
	if( &wk_chk_addr_32u == (volatile unsigned long **)RamChk_AddrP_32u ){
		RamChk_AddrP_32u++;
		return err;
	}

	/* --------------------------------- */
	/* 割込み禁止設定                    */
	/* --------------------------------- */
	DI_ORG;	/* 割込みマスク */
	
	/* チェックアドレスの取り込み */
	wk_chk_addr_32u = RamChk_AddrP_32u;
	
	/* --------------------------------- */
	/* ＲＡＭチェック処理                */ 
	/* --------------------------------- */
	/* チェックエリアのデータ退避 */
	RamChk_DataBuff_32u = (*wk_chk_addr_32u);
	
	/* チェックエリアにH'55555555書込み       */
	(*wk_chk_addr_32u) = RAMCHK2DATA1_NUM;
	
	/* 書込みデータチェック       */
	if((*wk_chk_addr_32u) != RAMCHK2DATA1_NUM)
	{	/* 一致しなかったらマイコンリセット */
		err = SF_DISORDER;						/* 異常ｾｯﾄ						*/
	}
	else{
		/* チェックエリアにH'AAAAAAAA書込み      */
		(*wk_chk_addr_32u) = RAMCHK2DATA2_NUM;
	
		/* 書込みデータチェック       */
		if((*wk_chk_addr_32u) != RAMCHK2DATA2_NUM)
		{	/* 一致しなかったらマイコンリセット */
			err = SF_DISORDER;						/* 異常ｾｯﾄ						*/
		}
		else{
			/* 一致したら退避データを復帰 & チェックアドレス更新 */
			(*wk_chk_addr_32u++) = RamChk_DataBuff_32u;
	
			/* 念のためバッファエリアをクリア */
			RamChk_DataBuff_32u = 0;
	
			/* 最終アドレスかチェック */
			/* 4ﾊﾞｲﾄ(ﾛﾝｸﾞ単位)ごとのRAMﾁｪｯｸなので、最後のﾁｪｯｸｱﾄﾞﾚｽは(最終アドレス－3)となる */
			if(wk_chk_addr_32u > (unsigned long *)(ramInitInfo.endAdr-3))
			{	/* チェックアドレスを先頭に移動 */
				wk_chk_addr_32u = (unsigned long *)ramInitInfo.startAdr;
			}
	
			/* チェックアドレスの更新 */
			RamChk_AddrP_32u = wk_chk_addr_32u;
		}
	}

	/* --------------------------------- */
	/* 割込み許可設定                    */
	/* --------------------------------- */
	EI_ORG;		/* 割込みマスク解除 */

	return err;
}

/*--------------------------------------------------------------------*/
/*! 内蔵拡張RAMのチェック(定期テスト)
 *	@param	
 *	@return	判定結果(SF_NORMAL：正常、SF_DISORDER：異常)
 *	@note	固定データ(55h,AAh,00h)書込み後に読み出して照合。
 *	@note	 指定範囲が全て正常になるまで無限ループ。
 *	@note	 1回の処理の時間を短縮するため、１アドレス(４バイト)ずつ
 */
/*--------------------------------------------------------------------
 *	履歴
 *	2020.1.22	M.Morooka		CHソフト流用
 *	2020.06.24	y.yamamoto		検査ができない対象外変数を追加
 *--------------------------------------------------------------------*/
SF_RESULT SFTestPeriodicRAM1(void)
{
	volatile unsigned long	*wk_chk_addr_32u;				/* チェックRAMアドレス  */
	SF_RESULT err = SF_NORMAL;

	/* --------------------------------- */
	/* 対象外の領域                      */
	/* --------------------------------- */
	if( &wk_chk_addr_32u == (volatile unsigned long **)RamChk_AddrP_32u1 ){
		RamChk_AddrP_32u++;
		return err;
	}

	/* --------------------------------- */
	/* 割込み禁止設定                    */
	/* --------------------------------- */
	DI_ORG;	/* 割込みマスク */
	
	/* チェックアドレスの取り込み */
	wk_chk_addr_32u = RamChk_AddrP_32u1;
	
	/* --------------------------------- */
	/* ＲＡＭチェック処理                */ 
	/* --------------------------------- */
	/* チェックエリアのデータ退避 */
	RamChk_DataBuff_32u1 = (*wk_chk_addr_32u);
	
	/* チェックエリアにH'55555555書込み       */
	(*wk_chk_addr_32u) = RAMCHK2DATA1_NUM;
	
	/* 書込みデータチェック       */
	if((*wk_chk_addr_32u) != RAMCHK2DATA1_NUM)
	{	/* 一致しなかったらマイコンリセット */
		err = SF_DISORDER;						/* 異常ｾｯﾄ						*/
	}
	else{
		/* チェックエリアにH'AAAAAAAA書込み      */
		(*wk_chk_addr_32u) = RAMCHK2DATA2_NUM;
	
		/* 書込みデータチェック       */
		if((*wk_chk_addr_32u) != RAMCHK2DATA2_NUM)
		{	/* 一致しなかったらマイコンリセット */
			err = SF_DISORDER;						/* 異常ｾｯﾄ						*/
		}
		else{
			/* 一致したら退避データを復帰 & チェックアドレス更新 */
			(*wk_chk_addr_32u++) = RamChk_DataBuff_32u1;
	
			/* 念のためバッファエリアをクリア */
			RamChk_DataBuff_32u1 = 0;
	
			/* 最終アドレスかチェック */
			/* 4ﾊﾞｲﾄ(ﾛﾝｸﾞ単位)ごとのRAMﾁｪｯｸなので、最後のﾁｪｯｸｱﾄﾞﾚｽは(最終アドレス－3)となる */
			if(wk_chk_addr_32u > (unsigned long *)(ramInitInfo1.endAdr-3))
			{	/* チェックアドレスを先頭に移動 */
				wk_chk_addr_32u = (unsigned long *)ramInitInfo1.startAdr;
			}
	
			/* チェックアドレスの更新 */
			RamChk_AddrP_32u1 = wk_chk_addr_32u;
		}
	}

	/* --------------------------------- */
	/* 割込み許可設定                    */
	/* --------------------------------- */
	EI_ORG;		/* 割込みマスク解除 */

	return err;
}

//[CO191]/*--------------------------------------------------------------------*/
//[CO191]/*!	CRC判定準備
//[CO191] *	@param	初期値構造体
//[CO191] *	@return	設定異常有無 (SF_NORMAL：無、SF_DISORDER：有)
//[CO191] *	@note	
//[CO191] */
//[CO191]/*--------------------------------------------------------------------
//[CO191] *	履歴
//[CO191] *	2016.12.03	SSXCH04	a.kinoshita		マルチ流用
//[CO191] *--------------------------------------------------------------------*/
//[CO191]SF_RESULT SFCreateCRC( SF_CRC_INIT_AREA *crcSet )
//[CO191]{
//[CO191]	SF_RESULT err = SF_NORMAL;
//[CO191]	
//[CO191]	/* IARではFFFF FFFFhとの比較は「Pa084」のワーニングとなるため、コメントアウトする。 */
//[CO191]	if ((crcSet == NULL)
//[CO191]//	|| (crcSet->startAdr > MEMORY_MAX)						/* 先頭ｱﾄﾞﾚｽがﾒﾓﾘ範囲外			*/
//[CO191]//	|| (crcSet->endAdr > MEMORY_MAX)						/* 末尾ｱﾄﾞﾚｽがﾒﾓﾘ範囲外			*/
//[CO191]	|| ( crcSet->cutSize > ROM_SIZE)						/* 分割ｻｲｽﾞがﾒﾓﾘ範囲外			*/
//[CO191]	|| (crcSet->startAdr > crcSet->endAdr) 					/* 先頭 > 末尾 					*/
//[CO191]	|| (crcSet->cutSize == 0)) {							/* ｻｲｽﾞが0						*/
//[CO191]		err = SF_DISORDER;
//[CO191]		crcStatus = CRC_NOT_CREATE;							/* →準備失敗					*/
//[CO191]	} else {
//[CO191]		crcInitInfo.startAdr = crcSet->startAdr;			/* 対象先頭ｱﾄﾞﾚｽ				*/
//[CO191]		crcInitInfo.endAdr = crcSet->endAdr;				/* 対象末尾ｱﾄﾞﾚｽ				*/
//[CO191]		crcInitInfo.cutSize = crcSet->cutSize;				/* CRC計算分割ｻｲｽﾞ				*/
//[CO191]		crcInitInfo.initCrc = crcSet->initCrc;				/* CRC計算初期化値				*/
//[CO191]		crcInitInfo.correctAdr = crcSet->correctAdr;		/* CRC格納ｱﾄﾞﾚｽ					*/
//[CO191]		crcInitInfo.wdtRestart = crcSet->wdtRestart;		/* WDTﾘｽﾀｰﾄ関数ﾎﾟｲﾝﾀ			*/
//[CO191]		err = SF_NORMAL;
//[CO191]		CRC_Start();
//[CO191]		crcStatus = CRC_INIT_STATUS;						/* →初期状態					*/
//[CO191]	}
//[CO191]	
//[CO191]	return err;
//[CO191]}
//[CO191]
//[CO191]/*--------------------------------------------------------------------*/
//[CO191]/*!	CRC判定準備
//[CO191] *	@param	初期値構造体
//[CO191] *	@return	設定異常有無 (SF_NORMAL：無、SF_DISORDER：有)
//[CO191] *	@note	
//[CO191] */
//[CO191]/*--------------------------------------------------------------------
//[CO191] *	履歴
//[CO191] *	2020.1.22	M.Morooka		CHソフト流用
//[CO191] *--------------------------------------------------------------------*/
//[CO191]SF_RESULT SFCreateCRC1( SF_CRC_INIT_AREA *crcSet1 )
//[CO191]{
//[CO191]	SF_RESULT err = SF_NORMAL;
//[CO191]	
//[CO191]	/* IARではFFFF FFFFhとの比較は「Pa084」のワーニングとなるため、コメントアウトする。 */
//[CO191]	if ((crcSet1 == NULL)
//[CO191]//	|| (crcSet1->startAdr > MEMORY_MAX)						/* 先頭ｱﾄﾞﾚｽがﾒﾓﾘ範囲外			*/
//[CO191]//	|| (crcSet1->endAdr > MEMORY_MAX)						/* 末尾ｱﾄﾞﾚｽがﾒﾓﾘ範囲外			*/
//[CO191]	|| ( crcSet1->cutSize > ROM_SIZE)					/* 分割ｻｲｽﾞがﾒﾓﾘ範囲外			*/
//[CO191]	|| (crcSet1->startAdr > crcSet1->endAdr) 					/* 先頭 > 末尾 					*/
//[CO191]	|| (crcSet1->cutSize == 0)) {							/* ｻｲｽﾞが0						*/
//[CO191]		err = SF_DISORDER;
//[CO191]		crcStatus1 = CRC_NOT_CREATE;							/* →準備失敗					*/
//[CO191]	} else {
//[CO191]		crcInitInfo1.startAdr = crcSet1->startAdr;			/* 対象先頭ｱﾄﾞﾚｽ				*/
//[CO191]		crcInitInfo1.endAdr = crcSet1->endAdr;				/* 対象末尾ｱﾄﾞﾚｽ				*/
//[CO191]		crcInitInfo1.cutSize = crcSet1->cutSize;				/* CRC計算分割ｻｲｽﾞ				*/
//[CO191]		crcInitInfo1.initCrc = crcSet1->initCrc;				/* CRC計算初期化値				*/
//[CO191]		crcInitInfo1.correctAdr = crcSet1->correctAdr;		/* CRC格納ｱﾄﾞﾚｽ					*/
//[CO191]		crcInitInfo1.wdtRestart = crcSet1->wdtRestart;		/* WDTﾘｽﾀｰﾄ関数ﾎﾟｲﾝﾀ			*/
//[CO191]		err = SF_NORMAL;
//[CO191]		CRC_Start();
//[CO191]		crcStatus1 = CRC_INIT_STATUS;						/* →初期状態					*/
//[CO191]	}
//[CO191]	
//[CO191]	return err;
//[CO191]}
//[CO191]
//[CO191]/*--------------------------------------------------------------------*/
//[CO191]/*!	CRC機能テスト(オプション設定領域)
//[CO191] *	@param	
//[CO191] *	@return	判定結果 (SF_NORMAL：正常、SF_DISORDER：異常)
//[CO191] *	@note	機能テストは初期状態で対象全領域のテストを終える
//[CO191] */
//[CO191]/*--------------------------------------------------------------------
//[CO191] *	履歴
//[CO191] *	2016.12.03	SSXCH04	a.kinoshita		マルチ流用
//[CO191] *--------------------------------------------------------------------*/
//[CO191]SF_RESULT SFTestFunctionalCRC( void )
//[CO191]{
//[CO191]	unsigned short	crcF = crcInitInfo.initCrc;
//[CO191]	unsigned long	checkStartF = crcInitInfo.startAdr;
//[CO191]	unsigned long	checkEndF = 0UL;
//[CO191]	unsigned char	endFlag = 0U;
//[CO191]	SF_RESULT		err = SF_NORMAL;
//[CO191]	
//[CO191]	switch(crcStatus){
//[CO191]	case CRC_NOT_CREATE:										/* 状態：準備失敗			*/
//[CO191]		err = SF_DISORDER;
//[CO191]		break;
//[CO191]		
//[CO191]	case CRC_INIT_STATUS:										/* 状態：初期状態			*/
//[CO191]/* Add-start 2017.05.09 a.kinoshita SSXCH_19 */
//[CO191]		CRC_Start();											/* CRC初期化				*/
//[CO191]/* Add-end   2017.05.09 a.kinoshita SSXCH_19 */
//[CO191]		while(endFlag == 0U){
//[CO191]			checkEndF = GetEnd(checkStartF, crcInitInfo.cutSize, crcInitInfo.endAdr);
//[CO191]																/* 計算末尾ｱﾄﾞﾚｽ算出		*/
//[CO191]			crcF = UpdateCRC(checkStartF, checkEndF, crcF);		/* CRC値計算				*/
//[CO191]			if (checkEndF >= crcInitInfo.endAdr) {				/* 対象範囲最後まで終了		*/
//[CO191]				endFlag = 1U;									/* ﾙｰﾌﾟを抜ける準備			*/
//[CO191]			} else {
//[CO191]				checkStartF = checkEndF + 1UL;					/* 次の計算先頭ｱﾄﾞﾚｽに更新	*/
//[CO191]			}
//[CO191]			if (crcInitInfo.wdtRestart != NULL) {
//[CO191]				crcInitInfo.wdtRestart();						/* WDTﾘｽﾀｰﾄ					*/
//[CO191]			} else {
//[CO191]				/* nothing */
//[CO191]			}
//[CO191]		}
//[CO191]		err = CompareCRC(crcF, crcInitInfo.correctAdr);			/* 格納値との比較結果取得	*/
//[CO191]		break;
//[CO191]
//[CO191]	default:
//[CO191]		err = SF_DISORDER;
//[CO191]		crcStatus = CRC_NOT_CREATE;
//[CO191]		break;
//[CO191]	}
//[CO191]	return err;
//[CO191]}
//[CO191]
//[CO191]/*--------------------------------------------------------------------*/
//[CO191]/*!	CRC機能テスト(コードフラッシュ設定領域)
//[CO191] *	@param	
//[CO191] *	@return	判定結果 (SF_NORMAL：正常、SF_DISORDER：異常)
//[CO191] *	@note	機能テストは初期状態で対象全領域のテストを終える
//[CO191] */
//[CO191]/*--------------------------------------------------------------------
//[CO191] *	履歴
//[CO191] *	2020.1.22	M.Morooka		CHソフト流用
//[CO191] *--------------------------------------------------------------------*/
//[CO191]SF_RESULT SFTestFunctionalCRC1( void )
//[CO191]{
//[CO191]	unsigned short	crcF = crcInitInfo1.initCrc;
//[CO191]	unsigned long	checkStartF = crcInitInfo1.startAdr;
//[CO191]	unsigned long	checkEndF = 0UL;
//[CO191]	unsigned char	endFlag = 0U;
//[CO191]	SF_RESULT		err = SF_NORMAL;
//[CO191]	
//[CO191]	switch(crcStatus1){
//[CO191]	case CRC_NOT_CREATE:										/* 状態：準備失敗			*/
//[CO191]		err = SF_DISORDER;
//[CO191]		break;
//[CO191]		
//[CO191]	case CRC_INIT_STATUS:										/* 状態：初期状態			*/
//[CO191]		CRC_Start();											/* CRC初期化				*/
//[CO191]
//[CO191]		while(endFlag == 0U){
//[CO191]			checkEndF = GetEnd1(checkStartF, crcInitInfo1.cutSize, crcInitInfo1.endAdr);
//[CO191]																/* 計算末尾ｱﾄﾞﾚｽ算出		*/
//[CO191]			crcF = UpdateCRC(checkStartF, checkEndF, crcF);		/* CRC値計算				*/
//[CO191]			if (checkEndF >= crcInitInfo1.endAdr) {				/* 対象範囲最後まで終了		*/
//[CO191]				endFlag = 1U;									/* ﾙｰﾌﾟを抜ける準備			*/
//[CO191]			} else {
//[CO191]				checkStartF = checkEndF + 1UL;					/* 次の計算先頭ｱﾄﾞﾚｽに更新	*/
//[CO191]			}
//[CO191]			if (crcInitInfo1.wdtRestart != NULL) {
//[CO191]				crcInitInfo1.wdtRestart();						/* WDTﾘｽﾀｰﾄ					*/
//[CO191]			} else {
//[CO191]				/* nothing */
//[CO191]			}
//[CO191]		}
//[CO191]		err = CompareCRC(crcF, crcInitInfo1.correctAdr);		/* 格納値との比較結果取得	*/
//[CO191]		break;
//[CO191]
//[CO191]	default:
//[CO191]		err = SF_DISORDER;
//[CO191]		crcStatus1 = CRC_NOT_CREATE;
//[CO191]		break;
//[CO191]	}
//[CO191]	return err;
//[CO191]}
//[CO191]
//[CO191]/*--------------------------------------------------------------------*/
//[CO191]/*! CRC定期テスト(コードフラッシュ設定領域)
//[CO191] *	@param	なし
//[CO191] *	@return	判定結果 (SF_NORMAL：正常、SF_DISORDER：異常)
//[CO191] *	@note
//[CO191] */
//[CO191]/*--------------------------------------------------------------------
//[CO191] *	履歴
//[CO191] *	2016.12.03	SSXCH04	a.kinoshita		マルチ流用
//[CO191] *--------------------------------------------------------------------*/
//[CO191]SF_RESULT SFTestPeriodicCRC1(void)
//[CO191]{
//[CO191]	static uint32_t checkStartP = 0UL;
//[CO191]	static uint16_t crcP = 0U;
//[CO191]	uint32_t checkEndP = 0UL;
//[CO191]	SF_RESULT err = SF_NORMAL;
//[CO191]	
//[CO191]	switch(crcStatus1){							/* CRCﾃｽﾄ状態にて処理分岐		*/
//[CO191]		case CRC_NOT_CREATE:					/* 状態：準備失敗				*/
//[CO191]			err = SF_DISORDER;					/* 異常ｾｯﾄ						*/
//[CO191]			break;
//[CO191]		
//[CO191]		case CRC_INIT_STATUS:					/* 状態：初期化					*/
//[CO191]			CRC_Start();						/* CRC初期化					*/
//[CO191]			crcP = crcInitInfo1.initCrc;			/* CRC値初期化					*/
//[CO191]			checkStartP = crcInitInfo1.startAdr;	/* 計算先頭ｱﾄﾞﾚｽ初期化			*/
//[CO191]			crcStatus1 = CRC_CALUCULATE_STATUS;	/* CRCﾃｽﾄ状態を計算中へ			*/
//[CO191]			err = SF_NORMAL;					/* 正常ｾｯﾄ						*/
//[CO191]			break;
//[CO191]		
//[CO191]		case CRC_CALUCULATE_STATUS:				/* 状態：計算中					*/
//[CO191]												/* 計算末尾ｱﾄﾞﾚｽ算出			*/
//[CO191]			checkEndP = GetEnd1(checkStartP, crcInitInfo1.cutSize, crcInitInfo1.endAdr);
//[CO191]												/* CRC値計算					*/
//[CO191]			crcP = UpdateCRC(checkStartP, checkEndP, crcP);
//[CO191]			
//[CO191]			if(checkEndP < crcInitInfo1.endAdr){	/* 対象ｱﾄﾞﾚｽ範囲内 ならば		*/
//[CO191]				checkStartP = checkEndP + 1UL;	/* 次の計算先頭ｱﾄﾞﾚｽに更新		*/
//[CO191]			}
//[CO191]			else{								/* 対象ｱﾄﾞﾚｽ到達 ならば			*/
//[CO191]				crcStatus1 = CRC_FINISH_STATUS;	/* CRCﾃｽﾄ状態を計算終了へ		*/
//[CO191]			}
//[CO191]			err = SF_NORMAL;					/* 正常ｾｯﾄ						*/
//[CO191]			break;
//[CO191]		
//[CO191]		case CRC_FINISH_STATUS:					/* 状態：計算終了				*/
//[CO191]												/* 格納値との比較結果取得		*/
//[CO191]			err = CompareCRC(crcP, crcInitInfo1.correctAdr);
//[CO191]			crcStatus1 = CRC_INIT_STATUS;		/* CRCﾃｽﾄ状態を初期状態へ		*/
//[CO191]			break;
//[CO191]		
//[CO191]		default:								/* 状態：その他					*/
//[CO191]			err = SF_DISORDER;					/* 異常ｾｯﾄ						*/
//[CO191]			crcStatus1 = CRC_NOT_CREATE;			/* CRCﾃｽﾄ状態を準備失敗へ		*/
//[CO191]			break;
//[CO191]	}
//[CO191]	return err;
//[CO191]}
//[CO191]
//[CO191]/**********************************************************************************/
//[CO191]/*** 隠れ関数																	***/
//[CO191]/**********************************************************************************/
//[CO191]/*--------------------------------------------------------------------------------------*/
//[CO191]/*!	計算末尾ｱﾄﾞﾚｽ算出(オプション設定領域)
//[CO191] *	@param	計算先頭ｱﾄﾞﾚｽ, 分割ｻｲｽﾞ, 対象範囲末尾ｱﾄﾞﾚｽ
//[CO191] *	@return	計算末尾ｱﾄﾞﾚｽ
//[CO191] *	@note	
//[CO191] *			
//[CO191] */
//[CO191]/*--------------------------------------------------------------------------------------*/
//[CO191]/*  履歴  2015.06.15  a.kato  新規
//[CO191] */
//[CO191]/*--------------------------------------------------------------------------------------*/
//[CO191]static unsigned long GetEnd( unsigned long checkStart, unsigned long size, unsigned long endAdr )
//[CO191]{
//[CO191]	unsigned long checkEnd = 0UL;
//[CO191]	unsigned long sizeOver = 0UL;
//[CO191]	
//[CO191]	sizeOver = ROM_END_ADDR - checkStart + 1;
//[CO191]	if (sizeOver < size ) {				/* ﾒﾓﾘ範囲外		*/
//[CO191]		checkEnd = ROM_END_ADDR;
//[CO191]	} else {
//[CO191]		checkEnd = checkStart + size - 1UL;
//[CO191]	}
//[CO191]	if (checkEnd > endAdr) {
//[CO191]		checkEnd = endAdr;
//[CO191]	} else {
//[CO191]		/* nothing */
//[CO191]	}
//[CO191]	return checkEnd;
//[CO191]}
//[CO191]
//[CO191]/*--------------------------------------------------------------------------------------*/
//[CO191]/*!	計算末尾ｱﾄﾞﾚｽ算出(コードフラッシュ設定領域)
//[CO191] *	@param	計算先頭ｱﾄﾞﾚｽ, 分割ｻｲｽﾞ, 対象範囲末尾ｱﾄﾞﾚｽ
//[CO191] *	@return	計算末尾ｱﾄﾞﾚｽ
//[CO191] *	@note	
//[CO191] *			
//[CO191] */
//[CO191]/*--------------------------------------------------------------------------------------*/
//[CO191]/*  履歴  2015.06.15  a.kato  新規
//[CO191] */
//[CO191]/*--------------------------------------------------------------------------------------*/
//[CO191]static unsigned long GetEnd1( unsigned long checkStart, unsigned long size, unsigned long endAdr )
//[CO191]{
//[CO191]	unsigned long checkEnd = 0UL;
//[CO191]	unsigned long sizeOver = 0UL;
//[CO191]	
//[CO191]	sizeOver = ROM_END_ADDR1 - checkStart + 1;
//[CO191]	if (sizeOver < size ) {				/* ﾒﾓﾘ範囲外		*/
//[CO191]		checkEnd = ROM_END_ADDR1;
//[CO191]	} else {
//[CO191]		checkEnd = checkStart + size - 1UL;
//[CO191]	}
//[CO191]	if (checkEnd > endAdr) {
//[CO191]		checkEnd = endAdr;
//[CO191]	} else {
//[CO191]		/* nothing */
//[CO191]	}
//[CO191]	return checkEnd;
//[CO191]}
//[CO191]
//[CO191]/*--------------------------------------------------------------------------------------*/
//[CO191]/*!	CRC値計算
//[CO191] *	@param	計算先頭ｱﾄﾞﾚｽ, 計算末尾ｱﾄﾞﾚｽ, 計算初期値
//[CO191] *	@return	CRC値
//[CO191] *	@note	
//[CO191] *			
//[CO191] */
//[CO191]/*--------------------------------------------------------------------------------------*/
//[CO191]/*  履歴  2015.06.15  a.kato  新規
//[CO191] */
//[CO191]/*--------------------------------------------------------------------------------------*/
//[CO191]static unsigned short UpdateCRC( unsigned long checkStart, unsigned long checkEnd, unsigned short startCrc )
//[CO191]{
//[CO191]	unsigned short crc = 0U;
//[CO191]	
//[CO191]	checksumCRC.length = checkEnd - checkStart + 1UL;
//[CO191]	checksumCRC.start_address = (unsigned char *)checkStart;
//[CO191]	CRC_AddRange(checksumCRC.start_address,checksumCRC.length);
//[CO191]	crc = CRC_Result();
//[CO191]	return crc;
//[CO191]}
//[CO191]
//[CO191]/*--------------------------------------------------------------------------------------*/
//[CO191]/*!	格納CRCと比較
//[CO191] *	@param	CRC計算値
//[CO191] *	@return	判定結果 (SF_NORMAL：正常、SF_DISORDER：異常)
//[CO191] *	@note	
//[CO191] *			
//[CO191] */
//[CO191]/*--------------------------------------------------------------------------------------*/
//[CO191]/*  履歴  2015.06.15  a.kato  新規
//[CO191] */
//[CO191]/*--------------------------------------------------------------------------------------*/
//[CO191]static SF_RESULT CompareCRC( unsigned short crc, unsigned long correctAdr )
//[CO191]{
//[CO191]	SF_RESULT err = SF_NORMAL;
//[CO191]	
//[CO191]	if (crc != *((unsigned short *)correctAdr)) {
//[CO191]		err = SF_DISORDER;
//[CO191]	} else {
//[CO191]		/* nothing */
//[CO191]	}
//[CO191]	return err;
//[CO191]}
//[CO191]
//[CO191]/*--------------------------------------------------------------------------------------*/
//[CO191]/*!	CRC初期化
//[CO191] *	@param	計算先頭ｱﾄﾞﾚｽ, 計算末尾ｱﾄﾞﾚｽ, 計算初期値
//[CO191] *	@return	CRC値
//[CO191] *	@note	
//[CO191] *			
//[CO191] */
//[CO191]/*--------------------------------------------------------------------------------------*/
//[CO191]/*  履歴  2015.06.15  a.kato  新規
//[CO191] */
//[CO191]/*--------------------------------------------------------------------------------------*/
//[CO191]static void CRC_Start(void)
//[CO191]{
//[CO191]	/*Clear any previous result.*/
//[CO191]	CRC.CRCCR.BIT.DORCLR = 1;
//[CO191]	
//[CO191]	/*Initial value */
//[CO191]	CRC.CRCDOR.WORD = 0xFFFF;
//[CO191]}
//[CO191]
//[CO191]/*--------------------------------------------------------------------------------------*/
//[CO191]/*!	CRC値計算
//[CO191] *	@param	計算先頭ｱﾄﾞﾚｽ, 計算末尾ｱﾄﾞﾚｽ, 計算初期値
//[CO191] *	@return	CRC値
//[CO191] *	@note	
//[CO191] *			
//[CO191] */
//[CO191]/*--------------------------------------------------------------------------------------*/
//[CO191]/*  履歴  2015.06.15  a.kato  新規
//[CO191] */
//[CO191]/*--------------------------------------------------------------------------------------*/
//[CO191]static void CRC_AddRange(unsigned char *Data, long Length)
//[CO191]{
//[CO191]	/*Write the data a byte at a time to the CRC Data Input register.*/
//[CO191]	while(0 != Length)
//[CO191]	{	
//[CO191]		R_Config_CRC_Input_Data(*Data);
//[CO191]		/*Onto the next byte*/
//[CO191]		Data++;
//[CO191]		Length--;
//[CO191]	}
//[CO191]}
//[CO191]
//[CO191]/*--------------------------------------------------------------------------------------*/
//[CO191]/*!	CRCデータ出力結果
//[CO191] *	@param	計算先頭ｱﾄﾞﾚｽ, 計算末尾ｱﾄﾞﾚｽ, 計算初期値
//[CO191] *	@return	CRC値
//[CO191] *	@note	
//[CO191] *			
//[CO191] */
//[CO191]/*--------------------------------------------------------------------------------------*/
//[CO191]/*  履歴  2015.06.15  a.kato  新規
//[CO191] */
//[CO191]/*--------------------------------------------------------------------------------------*/
//[CO191]static unsigned short CRC_Result(void)
//[CO191]{
//[CO191]    return (unsigned short)(CRC.CRCDOR.WORD);
//[CO191]}
/*--------------------------------------------------------------------*/
/*! ROM定期テスト(コードフラッシュ設定領域)
 *	@param	なし
 *	@return	判定結果 (SF_NORMAL：正常、SF_DISORDER：異常)
 *	@note	チラーは 485通信で CRC を使用するため、ROMテストでCRCを使うことができない。
 *	@note	やむを得ずチラーでの ROMテストは従来どおりの簡易的なものとする。
 */
/*--------------------------------------------------------------------
 *	履歴
 *	2020.06.24	CO191	y.yamamoto
 *--------------------------------------------------------------------*/
SF_RESULT SFTestPeriodicROM(void)
{
	static uint32_t checkAddrP = 0UL;
	static uint8_t checkSum = 0U;
	uint8_t checkCount;
	SF_RESULT err = SF_NORMAL;

	switch( romStatus )
	{
	case ROM_INIT_STATUS:						/* 状態：初期化					*/
		checkAddrP = ROM_START_ADDR1;			/* 計算先頭ｱﾄﾞﾚｽ初期化			*/
		checkSum = 0U;							/* チェックサムクリア			*/
		romStatus = ROM_CALUCULATE_STATUS;		/* ROMテスト状態を計算中へ		*/
		break;
	case ROM_CALUCULATE_STATUS:					/* 状態：計算中					*/
		checkCount = 32U;						/* 1回の計算量 32バイト			*/
		while( checkCount > 0U )
		{
			checkCount--;
			checkSum += *((uint8_t*)checkAddrP);	/* チェックサム加算				*/
			checkAddrP++;
			if( checkAddrP == ROM_END_ADDR1 )
			{	/* 最終アドレスに到達ならば		*/
				romStatus = ROM_FINISH_STATUS;	/* ROMテスト状態を計算終了へ	*/
				break;
			}
		}
		break;
	case ROM_FINISH_STATUS:						/* 状態：計算終了				*/
		if( checkSum != 0U )
		{					/* チェックサムNGならば			*/
#ifdef RELEASE
			err = SF_DISORDER;					/* 異常セット					*/
#endif
		}
		romStatus = ROM_INIT_STATUS;			/* ROMテスト状態を初期状態へ	*/
		break;
	default:									/* 状態：その他					*/
		err = SF_DISORDER;						/* 異常セット					*/
		romStatus = ROM_INIT_STATUS;			/* ROMテスト状態を初期状態へ	*/
		break;
	}

	return err;
}

/*--------------------------------------------------------------------------------------*/
/*! ソフトウェアリセット時の前処理
 *	@param		errno  エラーコード
 *	@return		
 *	@note		初期化処理の前に全ての割込み要因を無効化
 */
/*--------------------------------------------------------------------------------------*/
/*	履歴
 *	2016.11.08	SSXCH01	a.kinoshita		新規
 *	2020.06.24	CO191	y.yamamoto		引数にエラーコードを追加
/*--------------------------------------------------------------------------------------*/
void	Err_reset( SF_ERRNO errno )
{
#ifdef SF_RELEASE
	DI_ORG;

	for(;;){							/* リセットがかかるまで無限ループ				*/
		SYSTEM.PRCR.WORD = 0xA502;		/* プロテクト解除 PRKEY=0xA5, PRC1=1			*/
		SYSTEM.SWRR = 0xA501;			/* ソフトウェア・リセット						*/
	}
#endif
}

/*--------------------------------------------------------------------*/
/*! 安全機能 機能テスト RAM検査
 *	@param
 *	@return		
 *	@note		
 */
void	FunctionalTest1(void)
{
	static const SF_RAMCHECK_INIT_AREA RamRegionCheck = {
		RAM_START_ADDR,						/* 先頭ｱﾄﾞﾚｽ					*/
		RAM_END_ADDR,						/* 末尾ｱﾄﾞﾚｽ					*/
//[CO191]	&R_Config_IWDT_Restart			/* WDTﾘｽﾀｰﾄ関数ﾎﾟｲﾝﾀ			*/
		(void (*)(void))NULL				/* WDTﾘｽﾀｰﾄ関数ﾎﾟｲﾝﾀ			*/
	};
	
	static const SF_RAMCHECK_INIT_AREA RamRegionCheck1 = {
		RAM_START_ADDR1,					/* 先頭ｱﾄﾞﾚｽ					*/
		RAM_END_ADDR1,						/* 末尾ｱﾄﾞﾚｽ					*/
//[CO191]	&R_Config_IWDT_Restart			/* WDTﾘｽﾀｰﾄ関数ﾎﾟｲﾝﾀ			*/
		(void (*)(void))NULL				/* WDTﾘｽﾀｰﾄ関数ﾎﾟｲﾝﾀ			*/
	};

#ifdef SF_RELEASE
	if( SFTestRegister() != SF_NORMAL ){		/* ﾚｼﾞｽﾀ検査				*/
		Err_reset( SF_ERRNO_MCU_REGISTER );
	}
#endif

	SFRamCheckInit( (SF_RAMCHECK_INIT_AREA*)&RamRegionCheck );	/* 内蔵RAM検査準備			*/
#ifdef SF_RELEASE
	if( SFRamCheckAndClear() != SF_NORMAL ){	/* 内蔵RAM機能ﾃｽﾄ			*/
		Err_reset( SF_ERRNO_RAM1_CHECK_STARTUP );
	}
#endif

	SFRamCheckInit1( (SF_RAMCHECK_INIT_AREA*)&RamRegionCheck1 );/* 内蔵拡張RAM検査準備		*/
#ifdef SF_RELEASE
	if( SFRamCheckAndClear1() != SF_NORMAL){	/* 内蔵拡張RAM機能ﾃｽﾄ		*/
		Err_reset( SF_ERRNO_RAM2_CHECK_STARTUP );
	}
#endif
}

/*--------------------------------------------------------------------*/
/*! 安全機能 機能テスト ROM検査
 *	@param
 *	@return		
 *	@note	チラーは 485通信で CRC を使用するため、ROMテストでCRCを使うことができない。
 *	@note	やむを得ずチラーでの ROMテストは従来どおりの簡易的なものとする。
 */
void	FunctionalTest2(void)
{
//[CO191]	SF_CRC_INIT_AREA	allCrc = {			/* ROM機能ﾃｽﾄ(ｵﾌﾟｼｮﾝ設定領域)	*/
//[CO191]		ROM_START_ADDR,						/* 先頭ｱﾄﾞﾚｽ					*/
//[CO191]		ROM_END_ADDR,						/* 末尾ｱﾄﾞﾚｽ					*/
//[CO191]		CRC_CUTSIZE,						/* 分割ｻｲｽﾞ						*/
//[CO191]		CRC_INIT,							/* CRC初期化値					*/
//[CO191]		ROM_CORRECT_ADDR,					/* 正解値格納ｱﾄﾞﾚｽ				*/
//[CO191]		&R_Config_IWDT_Restart				/* WDTﾘｽﾀｰﾄ関数ﾎﾟｲﾝﾀ			*/
//[CO191]	};
//[CO191]	
//[CO191]	SF_CRC_INIT_AREA	allCrc1 = {			/* ROM機能ﾃｽﾄ(ｺｰﾄﾞﾌﾗｯｼｭﾒﾓﾘ領域)	*/
//[CO191]		ROM_START_ADDR1,					/* 先頭ｱﾄﾞﾚｽ					*/
//[CO191]		ROM_END_ADDR1,						/* 末尾ｱﾄﾞﾚｽ					*/
//[CO191]		CRC_CUTSIZE,						/* 分割ｻｲｽﾞ						*/
//[CO191]		CRC_INIT,							/* CRC初期化値					*/
//[CO191]		ROM_CORRECT_ADDR1,					/* 正解値格納ｱﾄﾞﾚｽ				*/
//[CO191]		&R_Config_IWDT_Restart				/* WDTﾘｽﾀｰﾄ関数ﾎﾟｲﾝﾀ			*/
//[CO191]	};
//[CO191]
//[CO191]	SFCreateCRC(&allCrc);					/* ROM検査準備					*/
//[CO191]#ifdef SF_RELEASE
//[CO191]	while (SFTestFunctionalCRC() != SF_NORMAL){};		/* ROM機能ﾃｽﾄ(ｵﾌﾟｼｮﾝ設定領域)	*/
//[CO191]#endif
//[CO191]
//[CO191]	SFCreateCRC1(&allCrc1);								/* ROM検査準備					*/
//[CO191]#ifdef SF_RELEASE
//[CO191]	while (SFTestFunctionalCRC1() != SF_NORMAL){};		/* ROM機能ﾃｽﾄ(ｺｰﾄﾞﾌﾗｯｼｭﾒﾓﾘ領域)	*/
//[CO191]#endif

	volatile uint8_t *p = (uint8_t*)ROM_START_ADDR1;
	volatile uint8_t sum = 0U;

	while( p != (uint8_t*)ROM_END_ADDR1 ){
		sum += *p++;
	}

	if( sum != 0U ){
#ifdef RELEASE
		Err_reset( SF_ERRNO_ROM_CHECK_STARTUP );	/* [SF] リセットスタート		*/
#endif
	}
}

/*--------------------------------------------------------------------*/
/*! 安全機能 ROM/RAM 定期検査準備
 *	@param
 *	@return
 *	@note		ﾒｲﾝ周期毎に検査。
 *	@note		CRC計算分割ｻｲｽﾞ：8ﾊﾞｲﾄ
 */
/*--------------------------------------------------------------------
 *	履歴
 *	2016.12.03	SSXCH04	a.kinoshita		マルチ流用
 *--------------------------------------------------------------------*/
void RomTestPeriodicSet(void)
{ 
//[CO191]	SF_CRC_INIT_AREA loopCrc = {
//[CO191]		ROM_START_ADDR1,					/* 先頭ｱﾄﾞﾚｽ					*/
//[CO191]		ROM_END_ADDR1,						/* 末尾ｱﾄﾞﾚｽ					*/
//[CO191]		CRC_CUTSIZE,						/* 分割ｻｲｽﾞ						*/
//[CO191]		CRC_INIT,							/* CRC初期化値					*/
//[CO191]		ROM_CORRECT_ADDR1,					/* 正解値格納ｱﾄﾞﾚｽ				*/
//[CO191]		&R_Config_IWDT_Restart				/* WDTﾘｽﾀｰﾄ関数ﾎﾟｲﾝﾀ			*/
//[CO191]	};
	static const SF_RAMCHECK_INIT_AREA RamRegionCheck = {
		RAM_START_ADDR,						/* 先頭ｱﾄﾞﾚｽ					*/
		RAM_END_ADDR,						/* 末尾ｱﾄﾞﾚｽ					*/
//[CO191]	&R_Config_IWDT_Restart			/* WDTﾘｽﾀｰﾄ関数ﾎﾟｲﾝﾀ			*/
		(void (*)(void))NULL				/* WDTﾘｽﾀｰﾄ関数ﾎﾟｲﾝﾀ			*/
	};
	
	static const SF_RAMCHECK_INIT_AREA RamRegionCheck1 = {
		RAM_START_ADDR1,					/* 先頭ｱﾄﾞﾚｽ					*/
		RAM_END_ADDR1,						/* 末尾ｱﾄﾞﾚｽ					*/
//[CO191]	&R_Config_IWDT_Restart			/* WDTﾘｽﾀｰﾄ関数ﾎﾟｲﾝﾀ			*/
		(void (*)(void))NULL				/* WDTﾘｽﾀｰﾄ関数ﾎﾟｲﾝﾀ			*/
	};

//[CO191]	SFCreateCRC1(&loopCrc);						/* ROM機能ﾃｽﾄ(CRC16)準備	*/
	SFRamCheckInit( (SF_RAMCHECK_INIT_AREA*)&RamRegionCheck );	/* 内蔵RAM検査準備			*/
	SFTestPeriodicRAM_init();					/* 内蔵RAM定期検査準備		*/
	SFRamCheckInit1( (SF_RAMCHECK_INIT_AREA*)&RamRegionCheck1 );/* 内蔵拡張RAM検査準備		*/
	SFTestPeriodicRAM_init1();					/* 内蔵拡張RAM定期検査準備	*/
}

/*--------------------------------------------------------------------------------------*/
/*! 安全機能  初期化
 *	@param
 *	@return		
 *	@note		
 */
/*--------------------------------------------------------------------------------------*/
/*	履歴
 *	2020.06.24	CO191	y.yamamoto		新規
/*--------------------------------------------------------------------------------------*/
void Init_SafetyFunction( void )
{
	RomTestPeriodicSet();				/*! 安全機能 ROM/RAM 定期検査準備				*/

	CAC.CACR1.BIT.CACREFE = 0U;			/* CACREF端子入力無効							*/
	CAC.CACR1.BIT.FMCS = 0U;			/* メインクロック								*/
	CAC.CACR1.BIT.TCSS = 0U;			/* 分周なし										*/
	CAC.CACR1.BIT.EDGES = 0U;			/* 立ち上がりエッジ								*/

	CAC.CACR2.BIT.RPS = 1U;				/* 基準信号＝内部クロック						*/
	CAC.CACR2.BIT.RSCS = 4U;			/* 測定基準クロック＝IWDTCLK 120KHz				*/
	CAC.CACR2.BIT.RCDS = 0U;			/* 測定基準クロック 32分周						*/
	CAC.CACR2.BIT.DFS = 0U;				/* デジタルフィルタ機能無効						*/

	CAC.CAICR.BIT.FERRIE = 0U;			/* 周波数エラー割り込み要求無効					*/
	CAC.CAICR.BIT.MENDIE = 1U;			/* 測定終了割り込み要求有効						*/
	CAC.CAICR.BIT.OVFIE = 1U;			/* オーバフロー割り込み要求有効					*/
	CAC.CAICR.BIT.FERRFCL = 1U;			/* FERRFフラグクリア							*/
	CAC.CAICR.BIT.MENDFCL = 1U;			/* MENDFCLフラグクリア							*/
	CAC.CAICR.BIT.OVFFCL = 1U;			/* OVFFCLフラグクリア							*/

	CAC.CAULVR = 3200U;					/* Upper-limit Typ * 1.2 (+20%)					*/
										/* Typ. 2667Hz = 10MHz / ( 120KHz / 32 )		*/
	CAC.CALLVR = 2133U;					/* Lower-limit Typ * 0.8 (-20%)					*/

	CAC.CACR0.BIT.CFME = 1U;			/* Enable clock frequency measurement			*/

	while( 0U == CAC.CACR0.BIT.CFME ){
		NOP_ORG;
	}

	CMT3.CMCR.WORD = 0x0081;			/* CMT3 クロック選択 PCLKB/32					*/
										/*      コンペアマッチ割り込み禁止				*/
										/*      予約ビットb7=1 (重要!)					*/
	CMT3.CMCOR = 0xFFFF;				/* CMT3 タイマ初期値							*/
	CMT.CMSTR1.BIT.STR3 = 1;			/* CMT3 カウント開始							*/
}
