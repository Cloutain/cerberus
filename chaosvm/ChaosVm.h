#if !defined(__CHAOSVM_H__)
#define __CHAOSVM_H__

#include "Common.h"
#include "logic.h"
#include "ChaosVmp.h"
#include "ChaosVmCpu.h"
#include "Support.h"
#include "ChaosVmByteCodeFile.h"
#include "PowerProtecter.h"

#if defined(__cplusplus)
extern "C"
{
#endif

//#define __CHAOSVM_INTERNAL_DEBUG__						0x01
#define __CHAOSVM_XFILE_DNA__								0xFBAC837E//�ô�ֵ��Ψһ��ʶ

// ����ģʽ
typedef enum _CHAOSVM_RUN_MODE {
	CRM_INFECT = 0,
	CRM_EMULATION,
	CRM_EMULATION_SYS,
	CRM_NETWORK
} CHAOSVM_RUN_MODE;

// �ļ�����
typedef enum _CHAOSVM_FILETYPE {
	CFT_NONE = 0,
	CFT_EXE,
	CFT_DLL,
	CFT_SYS
} CHAOSVM_FILETYPE;

/*
 * ������������ýṹ
 * �˽ṹĿǰֻ���ڸ�Ⱦģʽ
 */
typedef struct _CHAOSVM_CONFIGURE {
	CHAOSVM_FILETYPE FileType;//Ҫ�������ļ�����
	__bool bUseByteCodeFile;//ʹ���ֽ����ļ�
	__tchar szChaosVmByteCodeFileName[0x20];//�ֽ����ļ�������
	__dword dwTmp;

	//////////////////////////////////////////////////////////////////////////
	// �������������ýṹ
	POWER_PROTECTER_CONFIGURE PowerProtecterConfigure;//�˰汾ûɶ����

	//////////////////////////////////////////////////////////////////////////
	// ��ʾ��ѡ��,��cbc�ļ�ȷʵʱ����
	__tchar szMessageBoxTitle[0x40];
	__tchar szMessageBoxContext[0x100];
	__dword dwMessageStyle;//��Ϣ����
} CHAOSVM_CONFIGURE, *PCHAOSVM_CONFIGURE;

// �������������ʱ
#define __CHAOSVM_DECRYPT_OPCODE_TABLE_KEY_INFECT_MODE__							0xBD7115AB
#define __CHAOSVM_DECRYPT_OPCODE_TABLE_KEY_EMULATION_MODE__							0xACFC7763
typedef struct _CHAOSVM_RUNTIME {
	CHAOSVM_CPU Cpu;//CPU
	__memory pStack;//ִ�ж�ջ
	__integer iStackSize;//��ջ����
	CPU_BASIC_REGISTERS LastCheckPoint;//���һ�ε��üĴ�����ֵ
	CHAOSVMP_PROCEDURE Procedure;//ָ������¼�ṹ
	__integer iIndex;//������Index
	__integer iInstIndex;//ָ���Index
	__dword dwKey;//���ڽ���
	__address addrPrevProcedureAddress;//��һ�������ĵ�ַ
	__integer iPrevProcedureSize;//��һ�������ĳ���

	__integer iDispSize;//��ǰָ��Modλ��ƫ�ƴ�С

	/*
	 * ��¼�˵�ǰ�ı������
	 * �ڸ�Ⱦģʽ����ƪ�ڴ汻������,�ڸ�Ⱦģʽ�����ڷ����ģʽ�µ���Կ��һ��
	 */
	__byte Opcode1Tbl[0x100];
	__byte Opcode2Tbl[0x100];
} CHAOSVM_RUNTIME, *PCHAOSVM_RUNTIME;

#define __GetRunTime__(pCPU)								((PCHAOSVM_RUNTIME)((pCPU)->pUserContext))
#define __GetKey__(pCPU)									(__GetRunTime__(pCPU)->dwKey)
#define __GetInitKey__(pCPU)								((__GetRunTime__(pCPU)->Procedure).dwKey)

// ����ָ������
#if defined(__CHAOSVM_MODE_EMULATION__)
typedef __void (__API__ *FPChaosVmEmulationEntry)(PCHAOSVM_EMULATION_CONFIGURE pConfigure, PCHAOSVM_EMULATION_BYTECODE_FILE pByteCodeFile, PCHAOSVM_RUNTIME pRuntime);
#else
typedef __void (__API__ *FPChaosVmEntry)();
typedef __void (__API__ *FPChaosVmInit)();
#endif

#if defined(__cplusplus)
}
#endif

#endif
