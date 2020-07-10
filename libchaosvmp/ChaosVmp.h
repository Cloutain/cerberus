#if !defined(__CHAOSVMP_H__)
#define __CHAOSVMP_H__

#include "Common.h"
#include "lcrypto.h"

#if defined(__cplusplus)
extern "C"
{
#endif

#define __MAX_VMP_PROCEDURE__		64

typedef enum _MODRM_LOCAL {
	MSL_MOD = 0,
	MSL_RO = 1,
	MSL_RM = 2
} MODRM_LOCAL;

typedef enum _SIB_LOCAL {
	MSL_SS = 0,
	MSL_INDEX = 1,
	MSL_BASE = 2
} SIB_LOCAL;

#define __MAX_PROTECT_INSTRUCTION__		4096
typedef struct _CHAOSVMP_INSTRUCTION {
	//__bool bRelocate;//����ָ����Ҫ�ض�λ
	__integer iInstEncryptLength;//ָ��ֳ���
} CHAOSVMP_INSTRUCTION, *PCHAOSVMP_INSTRUCTION;

typedef struct _CHAOSVMP_JMPTARGET_INST {
	union {
		__address addrAddress;
		__offset ofRVA;//����ڱ������������ַ���ڴ�ƫ��
	};
	__offset ofOffsetByProcedure;//����ڱ���������ͷ��ƫ��
	__memory pAddress;
	__integer iPrevInstRandLen;//�����ȡ����ָ��ȫָ�Χ�ڵĳ���
	__integer iInstIndex;//���ָ�������
} CHAOSVMP_JMPTARGET_INST, *PCHAOSVMP_JMPTARGET_INST;

typedef struct _CHAOSVMP_JMPTARGET {
	CHAOSVMP_JMPTARGET_INST pInstList[__MAX_PROTECT_INSTRUCTION__];
	__integer iAddressCount;
} CHAOSVMP_JMPTARGET, *PCHAOSVMP_JMPTARGET;

/*
 * ���������������⻯�ṹ
 * ��������������Ҫ���ֶ�,��ChaosVm�к�,
 * addrProcedureMemoryAddress:��ʾ����ԭ�����ڴ��ַ;
 * pVmpProcedure:��ʾ���������⻯��ĵ�ַ
 * �������ֶ��ڻ������������ʱ������ڴ��ַת������Ҫ����
 */
#define __MAX_JMPTARGET_KEY_LEN__	1024
typedef struct _CHAOSVMP_PROCEDURE {
	__bool bHeader;//�Ƿ���ͷ����
	//////////////////////////////////////////////////////////////////////////
	//������ĺ��������Ϣ
	union {
		__address addrRealAddress;
		__address addrProcedureMemoryAddress;//����ͷ�ڴ��ַ
		__offset ofProcedureMemoryAddress;//����ͷ�ڴ�RVA,������������ʵ���ڵ�λ��
	};
	union {
		__address addrVisualAddress;
		__memory pVmpProcedure;
		__offset ofVmpProcedureRVA;//���RVA�Ǽ��ܺ󱻱����������ڵ�RVA
	};
	__integer iSize;//�����ĳ���
	__dword dwKey;//��ǰ������������KEY

	//////////////////////////////////////////////////////////////////////////
	// ����Key����
	__offset ofKeyRVA;//��ǰ������������Key��RVA
	union {
		__bool bUseProcKey;//�Ƿ���ú���Key
		__integer iKeySize;//��ǰ������������Key�ĳ���,����˳���Ϊ0,��������ѡȡ
	};
	__dword dwProcKey;//����Key
	
	CHAOSVMP_JMPTARGET JmpTargetRecord;//��¼��תĿ���ַ
	CHAOSVMP_INSTRUCTION ProtectInstruction[__MAX_PROTECT_INSTRUCTION__];//��ǰ����������ָ����Ϣ
	__integer iInstCount;//ָ�����

	__byte ModRmTbl[0x03];
	__byte SibTbl[0x03];
} CHAOSVMP_PROCEDURE, *PCHAOSVMP_PROCEDURE;

// ������Կ
#define __GenProcedureKey__(p, s)				PolyXorKey(crc32(p, s))

// Ҫ�ⲿ��д�Ļص�����
#define __VMP_INST_NOT_SUPPORT__				0x81								//��⵽��ָ��ChaosVmp��ǰ�汾��֧��
__void __INTERNAL_FUNC__ VmpThisInstructionErrorNotification(__integer iError, __char *pInstHex, __char *pInst, __integer iInstLength, __offset ofOffset);

// ���ܺ�����ת�������ĺ������Ͷ���
typedef __byte (__INTERNAL_FUNC__ *FPOpcodeExchange)(__byte bOpcode);
typedef __byte* (__INTERNAL_FUNC__ *FPModRmSibExchage)(__byte *bOpcode);
typedef __dword (__INTERNAL_FUNC__ *FPVmHash)(__memory pPoint, __integer iSize);
typedef __integer (__INTERNAL_FUNC__ *FPVmEncrypt)(__memory pIn, __integer iSize, __dword dwKey, __memory pOut);
typedef __integer (__INTERNAL_FUNC__ *FPVmDecrypt)(__memory pIn, __integer iSize, __dword dwKey, __memory pOut);
typedef __integer (__INTERNAL_FUNC__ *FPVmInstRemainEncrypt)(__memory pIn, __integer iSize, __dword dwKey, __memory pOut);
typedef __integer (__INTERNAL_FUNC__ *FPVmInstRemainDecrypt)(__memory pIn, __integer iSize, __dword dwKey, __memory pOut);

__void __API__ GenerateRandOpcodeTable(__byte *pOpcode, FPOpcodeExchange pOpcodeExchange);
__byte * __API__ GenerateRandModRmTable(__byte *pModRm, FPModRmSibExchage pModRmSibExchage);
__byte * __API__ GenerateRandSibTable(__byte *pSib, FPModRmSibExchage pModRmSibExchage);

PCHAOSVMP_PROCEDURE __API__ VmpThisProcedure(__memory pProcedure, __integer iSize, __offset ofProcedureRVA, FPVmHash pVmHash, \
											FPVmEncrypt pVmEncrypt, FPVmInstRemainEncrypt pInstRemainEncrypt, __byte *pOpcode1Tbl, \
											__byte *pOpcode2Tbl, __byte *pModRmTbl, __byte *pSibTbl, __memory pInvokeThunkCode, __integer iInvokeThunkCodeSize, \
											__dword dwKey, __bool bHeader, __dword *pNextKey, \
											__memory pKeyProcedure, __integer iKeyProcedureSize, __offset ofKeyProcedureRVA);

#if defined(__cplusplus)
}
#endif

#endif
