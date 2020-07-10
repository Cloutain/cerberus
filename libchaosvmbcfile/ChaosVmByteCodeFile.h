#if !defined(__CHAOSVM_BYTECODE_FILE_H__)
#define __CHAOSVM_BYTECODE_FILE_H__

#include "Common.h"

#define __CHAOSVM_BYTECODE_SIGN__							0xDEADDAED//�ֽ����ļ��ı�ʶ	
#define __IsChaosVmByteCodeFile__(x)						((x)->dwSign == __CHAOSVM_BYTECODE_SIGN__)//ȷ���Ƿ��ǻ�����������ֽ����ļ�

// ���������ѡ��,2012.2.9 ����
typedef struct _CHAOSVM_EMULATION_CONFIGURE_DEBUG_CONFIGURE {
	__bool bDebugChaosVm;
	__bool bBreakPoint;
} CHAOSVM_EMULATION_CONFIGURE_DEBUG_CONFIGURE, *PCHAOSVM_EMULATION_CONFIGURE_DEBUG_CONFIGURE;

// ����ģʽ���ýṹ
typedef struct _CHAOSVM_EMULATION_CONFIGURE {
	__dword dwTargetOrigImageBase;//����������ԭʼ�Ļ���ַ
	__dword dwTargetOrigSizeOfImage;//����������ԭʼ��ӳ�䳤��
	__dword dwTargetNowImageBase;//����������ǰ�Ļ���ַ
	__dword dwTargetNowSizeOfImage;//����������ĵ�ǰӳ�䳤��

	//////////////////////////////////////////////////////////////////////////
	// ������������������ѡ��,2012.2.9 ����
	CHAOSVM_EMULATION_CONFIGURE_DEBUG_CONFIGURE DebugConfigure;
} CHAOSVM_EMULATION_CONFIGURE, *PCHAOSVM_EMULATION_CONFIGURE;

/*
 * ByteCode�ļ��ṹ
 * CHAOSVM_EMULATION_BYTECODE_FILE
 * N��(CHAOSVM_EMULATION_BYTECODE_PROCEDURE + VmpProcedure)
 */
typedef struct _CHAOSVM_EMULATION_BYTECODE_PROCEDURE {
	__dword dwSize;//���Ͻ�ͷ�˽ڵĳ���
	__dword dwAttribute;//����������
} CHAOSVM_EMULATION_BYTECODE_PROCEDURE, *PCHAOSVM_EMULATION_BYTECODE_PROCEDURE;

// ���滷����BYTECODE�ṹ
typedef struct _CHAOSVM_EMULATION_BYTECODE_FILE {
	__dword dwSign;
	__dword dwSize;
	__dword dwProcedureCount;
	__dword dwCrc32;//�����ֽ����ļ���CRC32ֵ,��ͷ����
} CHAOSVM_EMULATION_BYTECODE_FILE, *PCHAOSVM_EMULATION_BYTECODE_FILE;

PCHAOSVM_EMULATION_BYTECODE_FILE __API__ ChaosVmByteCodeFileGenByteCodeFile();
__void __API__ ChaosVmByteCodeFileReleaseByteCodeFile(PCHAOSVM_EMULATION_BYTECODE_FILE *pFilePoint);
PCHAOSVM_EMULATION_BYTECODE_PROCEDURE __API__ ChaosVmByteCodeFileAddByteCodeProcedureToFile(PCHAOSVM_EMULATION_BYTECODE_FILE *pFilePoint, __memory pProcedure, __integer iProcSize);
__bool __API__ ChaosVmByteCodeFileOutputByteCodeFileToFile(__tchar *szFilePath, PCHAOSVM_EMULATION_BYTECODE_FILE pFile);
__bool __API__ ChaosVmByteCodeFileVerifySign(PCHAOSVM_EMULATION_BYTECODE_FILE pFile, __dword *pdwCrc32);
__integer __API__ ChaosVmByteCodeFileGetProcedureCount(PCHAOSVM_EMULATION_BYTECODE_FILE pByteCodeFile);
__memory __API__ ChaosVmByteCodeFileGetProcedureByteCode(__integer iIndex, PCHAOSVM_EMULATION_BYTECODE_FILE pByteCodeFile);

#endif