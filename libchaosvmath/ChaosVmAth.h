/*
 * �������������DLLֱ�����ļ���ʹ���ڴ�����չ����������Ϊ�½ڸ��ӵ�Ҫ�����ĳ�����
 *
 * �ָ����:
 * '-':��ʾĿ���ļ��ķָ���
 * '*':��ʾNT�½ڵķָ���
 * '*-'��ʾ��һ��NT����,ֻ�����ǻ��ֿռ����Եı��
 * 'x':��ʾX�ڵķָ���
 * '#':��ʾ�������������ָ���
 *
 * �����������Ŀ���ļ��е��ڴ沼��
 * --------------------
 * |                  |
 * |   ԭʼĿ���ļ�   | 
 * |                  |
 * ********************
 * |   ĩβ����
 * *-*-*-*-*-*-*-*-*-*-*
 * |   XFILE_HEADER   |
 * xxxxxxxxxxxxxxxxxxxx
 * |   XFILE_SECTION_HEADER(0)  |
 * |   XFILE_SECTION_HEADER(1)  |
 * |             .              |
 * |             .              |
 * |             .              |
 * |   XFILE_SECTION_HEADER(n)  |
 * xxxxxxxxxxxxxxxxxxxx
 * |   ����X�ڵĽڶ�    |
 * xxxxxxxxxxxxxxxxxxxx
 * |   ���������(�Ѿ�ͨ���ڴ����չ��)   |
 * ####################
 * |   �����������������   |
 * ####################
 * |   ������������л���ʱ���нṹ   |
 * xxxxxxxxxxxxxxxxxxxx
 * |   ����X�ڵĽڶ�  |
 * ********************
 * |   ����NT�ڵĽڶ� |
 * --------------------
 */

#if !defined(__CHAOSVMATH_H__)
#define __CHAOSVMATH_H__

#include "Common.h"
#include "ChaosVm.h"
#include "ChaosVmLdr.h"
#include "ChaosVmp.h"
#include "DISAth.h"
#include "PowerProtecterAth.h"

#if defined(__cplusplus)
extern "C"
{
#endif

#define __CHAOSVMP_SECTION_NAME__				"CHAOSVMP"
#define __CHAOSATH_MAX_SECTION_NUMBER__			5
#define __CHAOSATH_XFILE_SECTION_PLUS_SIZE		0x1000

typedef __memory (__API__ *FPGenChaosVmInvokeThunkCode)(__integer iSize);

// �������ĺ����ṹ
typedef struct _CHAOSVMATH_PROCEDURE {
	__memory pAddress;//�������ļ���ַ
	__address addrMemoryAddress;//�������ڴ��ַ
	__integer iSize;//�����ĳ���

	__memory pKeyAddress;//Key�������ļ���ַ
	__address addrKeyMemoryAddress;//Key�������ڴ��ַ
	__integer iKeySize;//Key�ĺ�������
} CHAOSVMATH_PROCEDURE, *PCHAOSVMATH_PROCEDURE;

// �˽ṹ�������û�������������ı���ѡ��
typedef struct _CHAOSVMATH_CONFIGURE {
	//////////////////////////////////////////////////////////////////////////
	// ͨ�õ�ѡ��
	CHAOSVM_RUN_MODE Mode;//����ʲôģʽ

	// ���������
	CHAOSVMATH_PROCEDURE ProceduceList[__MAX_VMP_PROCEDURE__];//Ҫ�����ĺ�������
	__integer iProcedureCount;//�������еļ���

	// �Զ��廨ָ������
	FPGenChaosVmInvokeThunkCode pGenChaosVmInvokeThunkCode;//�����������������ͷ��ָ��ĺ���ָ��

	// �ֽ����ļ�������
	__tchar szChaosVmByteCodeFileName[0x20];

	//////////////////////////////////////////////////////////////////////////
	// ��Ⱦģʽ�µ�����ѡ��
	__bool bUseByteCodeFile;//ʹ���ֽ����ļ�

	//////////////////////////////////////////////////////////////////////////
	// ����ģʽ�µ�����ѡ��
	__bool bUseChaosVmEmulationInResource;//ʹ����Դ�еĻ��������������
	__tchar szChaosVmEmulationFileName[0x20];

	//////////////////////////////////////////////////////////////////////////
	// ������������
	POWER_PROTECTER_ATTACH_CONFIGURE PowerProtecterAttachConfigure;

	//////////////////////////////////////////////////////////////////////////
	// ��ʾ��ѡ��
	__tchar szMessageBoxTitle[0x40];
	__tchar szMessageBoxOnByteCodeFileNotExist[0x100];
	__tchar szMessageBoxOnEmulationFileNotExist[0x100];
	__dword dwMessageStyle;//��Ϣ����

	//////////////////////////////////////////////////////////////////////////
	// ���������ѡ��
	// 2012.2.9 ����
	CHAOSVM_EMULATION_CONFIGURE_DEBUG_CONFIGURE DebugConfigure;
} CHAOSVMATH_CONFIGURE, *PCHAOSVMATH_CONFIGURE;

// �ļ�����
typedef CHAOSVM_FILETYPE	CHAOSVMATH_FILETYPE;

/*
 * ����������������ṹ
 * �˽ṹ���ڼ�¼�û����ڻ�������������ò����Լ����ýṹ
 */
typedef struct _CHAOSVMATH {
	//////////////////////////////////////////////////////////////////////////
	// ������Ϣ
	PCHAOSVMATH_CONFIGURE pConfigure;//���ýṹ
	CHAOSVMATH_FILETYPE FileType;//�ļ�����
	PXFILE_ATTACH_INFO pXFileAttachInfo;//X�ļ���������Ϣ�ṹ
	XFILE_SECTION_HEADER ChaosVmXSectionHeader;//������������ڵ�X�ڵĸ���
	PCHAOSVM_EMULATION_BYTECODE_FILE pByteCodeFile;//������������������ֽ����ļ�

	__memory pChaosVm;//������������ڴ�ӳ��
	__memory pOrigChaosVm;//ԭʼ�Ļ�����������ڴ�ӳ��
	__memory pChaosVmInTarget;//�����������Ŀ���ļ��е�ӳ��
	__integer iChaosVmSize;//����������ļ���С
	__integer iChaosVmSizeOfImage;//���������ӳ���С

	__offset ofChaosVmInTargetOffsetByXSectionBody;//��Ŀ���ļ��еĻ���������뵱ǰX��ͷ��ƫ��

	__offset ofChaosVmInTargetRVA;//�����������Ŀ���ļ��е�RVA
	__offset ofChaosVmInTargetOffset;//�����������Ŀ���ļ��е��ļ�ƫ��

	__offset ofChaosVmDataRvaByTarget;//�������������������Ŀ���ļ�ͷ��RVA
	__offset ofChaosVmDataOffsetByTarget;//�������������������Ŀ���ļ�ͷ��ƫ��

	__offset ofChaosVmRuntimeRvaByTarget;//�������������ʱ������Ŀ���ļ�ͷ��RVA
	__offset ofChaosVmRuntimeOffsetByTarget;//�������������ʱ������Ŀ���ļ�ͷ��ƫ��

	__offset ofPowerProtecterEncryptProceduresRvaByTarget;//�������������ܺ����ṹ��Ŀ���ļ�ͷ��ƫ��
	__offset ofPowerProtecterEncryptProceduresOffsetByTarget;//�������������ܺ����ṹ��Ŀ���ļ�ͷ��ƫ��

	__integer iChaosVmSectionSize;//����������ڵ��ܴ�С
	__integer iChaosVmDataSize;//����������������Ĵ�С
	__integer iChaosVmRuntimeSize;//�������������ʱ�ṹ���еĴ�С
	__integer iPowerProtecterEncryptProceduresSize;//�����������ܺ����ṹ���ܳ���

	__address addrTargetImageBase;//Ŀ���ļ���ԭʼӳ�����ַ
	__address addrChaosVmOrigImageBase;//���������ԭ����ӳ�����ַ
	__address addrChaosVmImageBaseInTarget;//�����������Ŀ���ļ��е�ӳ�����ַ = addrTargetImageBase + ofChaosVmInTargetRVA

	__memory pProcedureWriteTo;//��ǰ����Ҫд��Ŀ����������ָ��
	__dword dwKey;//���ܺ�����Key

	//////////////////////////////////////////////////////////////////////////
	// ���������Ĵ�ŵص�
	PCHAOSVMP_PROCEDURE pChaosVmpProcedure[__MAX_VMP_PROCEDURE__];

	//////////////////////////////////////////////////////////////////////////
	// һЩ�û��Զ���ĺ����ӿ�
	FPOpcodeExchange pOpcodeExchange;//OPת���Զ��庯��
	FPModRmSibExchage pModRmSibExchange;//MODRM/SIBλת���Զ��庯��
	FPVmHash pVmHash;//�Զ����ϣ����
	FPVmEncrypt pVmEncrypt;//�Զ�����ܺ���
	FPVmInstRemainEncrypt pVmInstRemainEncrypt;//�Զ���ָ��ʣ�����ݼ��ܺ���

	//////////////////////////////////////////////////////////////////////////
	// ������������߻�������������������ĺ���ָ����ȫ�ֱ���
	FPChaosVmEntry pChaosVmEntry;//������������
	FPChaosVmInit pChaosVmInit;//�����������ʼ������
	PCHAOSVM_RUNTIME pChaosVmRuntimeList;//�������л���ʱ����
	__integer *piVmpProcedureCount;//ָ�򱣻��˶��ٸ�����������ָ��
	union {
		PCHAOSVM_CONFIGURE pChaosVmConfigure;//������������ýṹ
		PCHAOSVM_LOADER_CONFIGURE pChaosVmLdrConfigure;//������������������ýṹ
	};

	__integer *piPowerProtecterEncryptProceduresCount;//�������������ܺ����ṹ�ļ���
	PPOWER_PROTECTER_ENCRYPT_PROCEDURE pPowerProtecterEncryptProcedures;//�������������ܺ���


	// ���¶����˸���ָ�������ʹ�õķ��ɺ�����
	PPARGS_DISPATCH_TABLE_ENTRY pOpcodeExtensionTableForOne_FF;
	PPARGS_DISPATCH_TABLE_ENTRY pOpcodeExtensionTableForOne_FE;
	PPARGS_DISPATCH_TABLE_ENTRY pOpcodeExtensionTableForOne_C6;
	PPARGS_DISPATCH_TABLE_ENTRY pOpcodeExtensionTableForOne_C7;
	PPARGS_DISPATCH_TABLE_ENTRY pOpcodeExtensionTableForOne_F6;
	PPARGS_DISPATCH_TABLE_ENTRY pOpcodeExtensionTableForOne_F7;
	PPARGS_DISPATCH_TABLE_ENTRY pOpcodeExtensionTableForOne_80;
	PPARGS_DISPATCH_TABLE_ENTRY pOpcodeExtensionTableForOne_81;
	PPARGS_DISPATCH_TABLE_ENTRY pOpcodeExtensionTableForOne_83;
	PPARGS_DISPATCH_TABLE_ENTRY pOpcodeExtensionTableForOne_8F;
	PPARGS_DISPATCH_TABLE_ENTRY pOpcodeExtensionTableForOne_C0;
	PPARGS_DISPATCH_TABLE_ENTRY pOpcodeExtensionTableForOne_C1;
	PPARGS_DISPATCH_TABLE_ENTRY pOpcodeExtensionTableForOne_D0;
	PPARGS_DISPATCH_TABLE_ENTRY pOpcodeExtensionTableForOne_D1;
	PPARGS_DISPATCH_TABLE_ENTRY pOpcodeExtensionTableForOne_D2;
	PPARGS_DISPATCH_TABLE_ENTRY pOpcodeExtensionTableForOne_D3;
	PPARGS_DISPATCH_TABLE_ENTRY pOpcodeExtensionTableForTwo_0FBA;
	PONE_OPCODE_TABLE_ENTRY pOneByteOpcodeTable;
	PTWO_BYTE_OPCODE_TABLE_ENTRY pTwoByteOpcodeTable;
	PEFLAG_CONDITION_TABLE_ENTRY pEFlagContionTable;
	PMODRM_BYTE_ANALYSE_ROUTINE_TABLE_ENTRY pModRMAnalyseRoutineTable;

	// ���¶����˸��ֹ��ܺ������������׵�ƫ����
	__offset ofChaosVmEntry;
	__offset ofChaosVmInit;
	__offset ofChaosVmRuntimeList;
	__offset ofVmpProcedureCount;
	union {
		__offset ofChaosVmConfigure;
		__offset ofChaosVmLdrConfigure;
	};

	__offset ofPowerProtecterEncryptProceduresCount;//�������������ܺ����ṹ����ƫ��
	__offset ofPowerProtecterEncryptProcedures;//�����������ܺ���ָ���ƫ��

	// ���¶����˸��ַ��ɱ��������׵�ƫ����
	__offset ofOpcodeExtensionTableForOne_FF;
	__offset ofOpcodeExtensioTableForOne_FE;
	__offset ofOpcodeExtensionTableForOne_C6;
	__offset ofOpcodeExtensionTableForOne_C7;
	__offset ofOpcodeExtensionTableForOne_F6;
	__offset ofOpcodeExtensionTableForOne_F7;
	__offset ofOpcodeExtensionTableForOne_80;
	__offset ofOpcodeExtensionTableForOne_81;
	__offset ofOpcodeExtensionTableForOne_83;
	__offset ofOpcodeExtensionTableForOne_8F;
	__offset ofOpcodeExtensionTableForOne_C0;
	__offset ofOpcodeExtensionTableForOne_C1;
	__offset ofOpcodeExtensionTableForOne_D0;
	__offset ofOpcodeExtensionTableForOne_D1;
	__offset ofOpcodeExtensionTableForOne_D2;
	__offset ofOpcodeExtensionTableForOne_D3;
	__offset ofOpcodeExtensionTableForTwo_0FBA;
	__offset ofOneByteOpcodeTable;
	__offset ofTwoByteOpcodeTable;
	__offset ofEFlagContionTable;
	__offset ofModRMAnalyseRoutineTable;

} CHAOSVMATH, *PCHAOSVMATH;

// Ŀ��ṹ������Ϣ
typedef struct _CHAOSVMATH_RET_INFO {
	__integer iTargetCodeSectionIndex;//ԭĿ����������
} CHAOSVMATH_RET_INFO, *PCHAOSVMATH_RET_INFO;

// ��ȡĿ���ļ�������
CHAOSVMATH_FILETYPE __API__ ChaosVmAthGetFileType(__tchar *pTargetFilePath);
// ��ʼ��������������ýṹ
__void __API__ ChaosVmAthInitConfigure(PCHAOSVMATH_CONFIGURE pConfigure);
// ѡ��һ��Ҫ�����ĺ���
__void __API__ ChaosVmAthSelectProcedure(PCHAOSVMATH_CONFIGURE pConfigure, __address addrMemoryAddress, __integer iSize, __address addrKeyMemoryAddress, __integer iKeySize);
// ���������������(�ļ�)
__dword __API__ ChaosVmAthAttach(PDISATH_CONFIGURE pConfigure);

#if defined(__cplusplus)
}
#endif

#endif
