#if !defined(__POWER_PROTECTER_ATTACH_H__)
#define __POWER_PROTECTER_ATTACH_H__

#if defined(__cplusplus)
extern "C"
{
#endif

#include "PowerProtecter.h"

#define __POWER_PROTECT_MAX_PROCEDURE__			1024
#define __POWER_PROTECT_MAX_INSTRUCTION__		1024

typedef __memory (__API__ *FPGenPowerProtecterInvokeThunkCode)(__integer iSize);

/*
 * ָ���¼,��¼��Ҫ������ָ��,Ҫ���ӵĵ�ַ
 * bWatched�ֶα����Ƿ�Ҫ���������ڴ�
 */
typedef struct _POWER_PROTECTER_INSTRUCTION {
	POWER_PROTECTER_INSTRUCTION_RECORD Instruction;//Ҫ������ָ��
	POWER_PROTECTER_WATCH_RECORD WatchRecord;//Ҫ�����ĵ�ַ
	__bool bWatched;//�Ƿ���ӵ�ַ
} POWER_PROTECTER_INSTRUCTION, *PPOWER_PROTECTER_INSTRUCTION;

/*
 * �˽ṹ�ɸ�����ʹ��,�ⲿ����������й�������ʱ,��Ҫ��������ϸ��Ϣ���뵽��
 */
typedef struct _POWER_PROTECTER_PROCEDURE {
	POWER_PROTECTER_PROCEDURE_RECORD Procedure[__POWER_PROTECT_MAX_PROCEDURE__];//Ҫ������������Ϣ
	POWER_PROTECTER_PROCEDURE_RECORD KeyProcedure[__POWER_PROTECT_MAX_PROCEDURE__];//��Ӧ�ļ��Ӻ�������Ϣ
	POWER_PROTECTER_INSTRUCTION Instructions[__POWER_PROTECT_MAX_PROCEDURE__][__POWER_PROTECT_MAX_INSTRUCTION__];//Ҫ������ָ��
	__integer iInstructionCount[__POWER_PROTECT_MAX_PROCEDURE__];//ÿ����������ָ��ĸ���
	__integer iCount;//�ܹ������ļ���
} POWER_PROTECTER_PROCEDURE, *PPOWER_PROTECTER_PROCEDURE;

// �������������ýṹ
typedef struct _POWER_PROTECTER_ATTACH_CONFIGURE {
	FPGenPowerProtecterInvokeThunkCode pGenPowerProtecterInvokeThunkCode;
	POWER_PROTECTER_PROCEDURE PowerProtecterProcedure;
} POWER_PROTECTER_ATTACH_CONFIGURE, *PPOWER_PROTECTER_ATTACH_CONFIGURE;

/*
 * ���º������а��յ���˳��
 */
__integer __API__ PowerProtectAthSelectProcedure(PPOWER_PROTECTER_PROCEDURE pRecord, __address addrProcedure, __integer iSize, __address addrKeyProcedure, __integer iKeySize, PPOWER_PROTECTER_INSTRUCTION pInstruction, __integer iInstCount);
__integer __API__ PowerProtecterAthCountAllEncryptProceduresSize(__memory pMem, PPOWER_PROTECTER_PROCEDURE pRecord);
__void __API__ PowerProtecterAthSetArguments(PPOWER_PROTECTER_ATTACH_CONFIGURE pConfigure);
__void __API__ PowerProtectAthGo(__memory pMem, PPOWER_PROTECTER_PROCEDURE pRecord, PPOWER_PROTECTER_ENCRYPT_PROCEDURE pEncryptProcedures);

#if defined(__cplusplus)
}
#endif

#endif