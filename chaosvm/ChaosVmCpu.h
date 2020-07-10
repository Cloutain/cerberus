#if !defined(__CHAOSVM_CPU_H__)
#define __CHAOSVM_CPU_H__

#include "Common.h"
#include "ChaosVmCpuStatus.h"
#include "ChaosVmCpuInternalStructs.h"

#if defined(__cplusplus)
extern "C"
{
#endif

//#define __CPU_INTERNAL_DEBUG__//�����ڲ�����״̬
//#define __USE_INSTRUCTION_INFORMATION__//ʹ��ָ����Ϣ
typedef CPU_STATUS (__INTERNAL_FUNC__ *FPMmReadRoutine)(__void *pCPU, __void *pUserContext, __word wSeg, __address addrAddress, __byte bSizeToReadByte, __byte *pOutputBuffer, __byte *pSizeReturnedByte);
typedef CPU_STATUS (__INTERNAL_FUNC__ *FPMmWriteRoutine)(__void *pCPU, __void *pUserContext, __word wSeg, __address addrAddress, __byte bSizeToWriteByte, __byte *pInputBuffer);
typedef CPU_STATUS (__INTERNAL_FUNC__ *FPMmCodeReadRoutine)(__void *pCPU, __void *pUserContext, __word wSeg, __address addrAddress, __byte bSizeToReadByte, __byte *pOutputBuffer, __byte *pSizeReturnedByte);
typedef __dword (__INTERNAL_FUNC__ *FPMmReadDataPartRoutine)(__void *pCPU, __void *pUserContext, __byte bImmIndexInOpcodeBuffer, __byte bSizeToReadByte);
typedef __dword (__INTERNAL_FUNC__ *FPMmReadDispPartRoutine)(__void *pCPU, __void *pUserContext, __memory pDispStart, __byte bSizeToReadByte);
typedef __void (__INTERNAL_FUNC__ *FPCpuErrorOutputRoutine)(__void *pCPU, __void *pUserContext, __char *Info);
typedef CPU_STATUS (__INTERNAL_FUNC__ *FPCpuInterruptHandleRoutine)(__void *pCPU, __void *pUserContext);
typedef __byte (__INTERNAL_FUNC__ *FPCpuBuildModrmRoutine)(__void *pCPU, __void *pUserContext, __byte bModrmByte);
typedef __byte (__INTERNAL_FUNC__ *FPCpuBuildSibRoutine)(__void *pCPU, __void *pUserContext, __byte bSibByte);

typedef CPU_STATUS (__INTERNAL_FUNC__ *FPCpuFlowControlHookRoutine)(__void *pCPU, __void *pUserContext, __word wNewSeg, __address addrNewAddress, __byte bOpcodeLength);
typedef CPU_STATUS (__INTERNAL_FUNC__ *FPCpuJccFlowControlHookRoutine)(__void *pCPU, __void *pUserContext, __address addrNewAddress, __byte bOpcodeLength);
typedef CPU_STATUS (__INTERNAL_FUNC__ *FPCpuExecuteInstructionHookRoutine)(__void *pCPU, __void *pUserContext);
typedef __void (__INTERNAL_FUNC__ *FPCpuExecuteEachInstructionHookRoutine)(__void *pCPU, __void *pUserContext);
typedef __void (__INTERNAL_FUNC__ *FPCpuExecuteEachInstructionCompletedHookRoutine)(__void *pCPU, __void *pUserContext);

/*
 * CPU HOOK����
 */
typedef enum _CHAOSVM_CPU_HOOKTYPE {
	HOOK_CALL = 1,//δ�����κ��޸�֮ǰ������
	HOOK_RET  = 2,//�Ӷ�ջ�е�����ַ�������޸�ESP��֮�����
	HOOK_JMP  = 3,//δ�����κ��޸�֮ǰ������
	HOOK_JCC  = 4,//δ�����κ��޸�֮ǰ������
	HOOK_EXECUTEINSTRUCTION = 5,//���κ�ָ��ִ��֮ǰ������
	HOOK_EXECUTEEACHINSTRUCTION = 6,//���κ��Խ���ָ��ִ��֮ǰ������
	HOOK_EXECUTEEACHINSTRUCTIONCOMPLETED = 7//���κ��Խ���ָ��ִ�б�����֮���ٵ���
} CHAOSVM_CPU_HOOKTYPE;

/*
 * EFLAGE��־״̬
 */
#define __EFLAG_SET__						1
#define __EFLAG_RESET__						0

/*
 * Ѱַģʽ
 */
#define __CPU_ADDRESSING_MODE_16__			1
#define __CPU_ADDRESSING_MODE_32__			0

/*
 * ����������
 */
#define __CPU_OPERAND_TYPE_16__			    1
#define __CPU_OPERAND_TYPE_32__			    0

/*
 * �ظ�ǰ׺
 */
#define __REP_PREFIX_NULL__					0
#define __REP_PREFIX_REP__					0xF3
#define __REP_PREFIX_REPE__					0xF3
#define __REP_PREFIX_REPNE__				0xF2

typedef enum _CONTROLREGISTERINDEX {
	CHAOSVM_R_CR0 = 0,
	CHAOSVM_R_CR1 = 1,
	CHAOSVM_R_CR2 = 2,
	CHAOSVM_R_CR3 = 3,
	CHAOSVM_R_CR4 = 4,
} CONTROLREGISTERINDEX;

typedef enum _DEBUGREGISTERINDEX {
	CHAOSVM_R_DR0 = 0,
	CHAOSVM_R_DR1 = 1,
	CHAOSVM_R_DR2 = 2,
	CHAOSVM_R_DR3 = 3,
	CHAOSVM_R_DR4 = 4,
	CHAOSVM_R_DR5 = 5,
	CHAOSVM_R_DR6 = 6,
	CHAOSVM_R_DR7 = 7,
} DEBUGREGISTERINDEX;

typedef enum _GENERALREGISTER32INDEX {
	CHAOSVM_R_EAX = 0,
	CHAOSVM_R_ECX = 1,
	CHAOSVM_R_EDX = 2,
	CHAOSVM_R_EBX = 3,
	CHAOSVM_R_ESP = 4,
	CHAOSVM_R_EBP = 5,
	CHAOSVM_R_ESI = 6,
	CHAOSVM_R_EDI = 7
} GENERALREGISTER32INDEX;

typedef enum _GENERALREGISTER16INDEX {
	CHAOSVM_R_AX = 0,
	CHAOSVM_R_CX = 1,
	CHAOSVM_R_DX = 2,
	CHAOSVM_R_BX = 3,
	CHAOSVM_R_SP = 4,
	CHAOSVM_R_BP = 5,
	CHAOSVM_R_SI = 6,
	CHAOSVM_R_DI = 7
} GENERALREGISTER16INDEX;

typedef enum _GENERALREGISTER8INDEX {
	CHAOSVM_R_AL = 0,
	CHAOSVM_R_CL = 1,
	CHAOSVM_R_DL = 2,
	CHAOSVM_R_BL = 3,
	CHAOSVM_R_AH = 4,
	CHAOSVM_R_CH = 5,
	CHAOSVM_R_DH = 6,
	CHAOSVM_R_BH = 7
} GENERALREGISTER8INDEX;

typedef enum _SEGMENTREGISTERINDEX {
	CHAOSVM_SR_ES = 0,
	CHAOSVM_SR_CS = 1,
	CHAOSVM_SR_SS = 2,
	CHAOSVM_SR_DS = 3,
	CHAOSVM_SR_FS = 4,
	CHAOSVM_SR_GS = 5
} SEGMENTREGISTERINDEX;

typedef enum _EFLAGINDEX {
	CHAOSVM_ER_CF = 0,
	CHAOSVM_ER_PF = 2,
	CHAOSVM_ER_AF = 4,
	CHAOSVM_ER_ZF = 6,
	CHAOSVM_ER_SF = 7,
	CHAOSVM_ER_TF = 8,
	CHAOSVM_ER_IF = 9,
	CHAOSVM_ER_DF = 10,
	CHAOSVM_ER_OF = 11,

	CHAOSVM_ER_IOPL = 12,/*ռ2λ*/
	CHAOSVM_ER_NT = 14,
	CHAOSVM_ER_RF = 16,
	CHAOSVM_ER_VM = 17,
	CHAOSVM_ER_AC = 18,
	CHAOSVM_ER_VIF = 19,
	CHAOSVM_ER_VIP = 20,
	CHAOSVM_ER_ID = 21
} EFLAGINDEX;

/*
 * ָ����Ϣ��ǰ׺����
 */
/*0xF0*/
#define __CPU_INSTRUCTION_PREFIX_LOCK__                         0x0001
/*0xF2*/
#define __CPU_INSTRUCTION_PREFIX_REPNZ__                        0x0002
/*0xF3*/
#define __CPU_INSTRUCTION_PREFIX_REPZ__                         0x0004
/*0x2E*/
#define __CPU_INSTRUCTION_PREFIX_SEGMENTOVERRIDE_CS__           0x0008
/*0x36*/
#define __CPU_INSTRUCTION_PREFIX_SEGMENTOVERRIDE_SS__           0x0010
/*0x3E*/
#define __CPU_INSTRUCTION_PREFIX_SEGMENTOVERRIDE_DS__           0x0020
/*0x26*/
#define __CPU_INSTRUCTION_PREFIX_SEGMENTOVERRIDE_ES__           0x0040
/*0x64*/
#define __CPU_INSTRUCTION_PREFIX_SEGMENTOVERRIDE_FS__           0x0080
/*0x65*/
#define __CPU_INSTRUCTION_PREFIX_SEGMENTOVERRIDE_GS__           0x0100
/*0x66*/
#define __CPU_INSTRUCTION_PREFIX_CHANGE_OPERAND_SIZE__          0x0200
/*0x67*/
#define __CPU_INSTRUCTION_PREFIX_CHANGE_ADDRESS_SIZE__          0x0400

/*
 * ָ����Ϣ����Ӱ��ļĴ�������ض���
 */
#define __CPU_INSTRUCTION_REG_TYPE_R8__           1
#define __CPU_INSTRUCTION_REG_TYPE_R16__          2
#define __CPU_INSTRUCTION_REG_TYPE_R32__          3
#define __CPU_INSTRUCTION_REG_TYPE_SR__           4
#define __CPU_INSTRUCTION_REG_TYPE_CONTROL__      5
#define __CPU_INSTRUCTION_REG_TYPE_R16_ALL__      11
#define __CPU_INSTRUCTION_REG_TYPE_R32_ALL__      11
#define __CPU_INSTRUCTION_REG_TYPE_NONE__         0xFF
#define __CPU_INSTRUCTION_DISASM_BUFFER_SIZE__    24

typedef struct _CPU_INTERRUPT_INFOMATION {
	__byte bInterruptId;
	__dword dwErrorCode;
	union {
		struct {
			__word wSeg;
			__address addrMemoryAddress;
			__dword dwRequestOpreation;
		} GP;
	} Infomation;
} CPU_INTERRUPT_INFOMATION, *PCPU_INTERRUPT_INFOMATION;

typedef struct _INSTRUCTION_TYPE_INFO {
	__word wInstructionType;
	union {
		struct {
			__byte bSubType;
			struct {
				__word wSeg;
				__offset ofOffset;
			} PrevAddress;
		} FlowControl;
	} Context;
} INSTRUCTION_TYPE_INFO, *PINSTRUCTION_TYPE_INFO;

typedef union _CPU_REGISTER_32 {
	__dword dwValue32;
	__word wValue16;
	__byte Value8[2];
} CPU_REGISTER_32, *PCPU_REGISTER_32;

typedef union _CPU_SEGMENT_REGISTER {
	__word wValue16;
} CPU_SEGMENT_REGISTER, *PCPU_SEGMENT_REGISTER;

typedef struct _CPU_INSTRUCTION_INFOMATION {
	/*
	 * ��ִ��ָ���ͬʱ����ǰָ��������Ϣ����������
	 * ǰ׺���ȡ�ǰ׺���͡�ָ��ȡ�Ѱַ���͡��Ƿ���ָ����ֱ�Ӹ����˵�ַ����ʹ��ƫ�ƣ���������ؽ�PE�ض�λ��ʱʹ�ã���
	 *ָ���漰�����ڴ��ַ��
	 */
	__byte bInstructionLength;//ָ���
	__byte bPrefixLength;//ǰ׺����
	__word wPrefix;//ǰ׺״̬��ÿλ��ʾһ��ǰ׺
	struct {
		__byte bType;//�Ĵ�������
		__byte bIndex;
	} AffectedRegister;//��Ӱ��ļĴ���
	__bool bIsHasDirectAddress;//�Ƿ����ֱ�ӵ�ַ
} CPU_INSTRUCTION_INFOMATION, *PCPU_INSTRUCTION_INFOMATION;

typedef struct _CPU_BASIC_REGISTERS {
	union {
		__dword BitArray;
		struct {
			__dword CF:1;//0
			__dword EFlag_Reserve_1:1;//1
			__dword PF:1;//2
			__dword EFlag_Reserve_3:1;//3
			__dword AF:1;//4
			__dword EFlag_Reserve_5:1;//5
			__dword ZF:1;//6
			__dword SF:1;//7
			__dword TF:1;//8
			__dword IF:1;//9
			__dword DF:1;//10
			__dword OF:1;//11
			__dword EFlag_Reserve:20;
		} Interface;
	} EFlag;

	union {
		CPU_REGISTER_32	Array[8];
		struct {
			CPU_REGISTER_32 EAX;
			CPU_REGISTER_32 ECX;
			CPU_REGISTER_32 EDX;
			CPU_REGISTER_32 EBX;
			CPU_REGISTER_32 ESP;
			CPU_REGISTER_32 EBP;
			CPU_REGISTER_32 ESI;
			CPU_REGISTER_32 EDI;
		} Interface;
	} GeneralRegister;
} CPU_BASIC_REGISTERS, *PCPU_BASIC_REGISTERS;

typedef struct _REGISTERS_STATUS {
	CPU_BASIC_REGISTERS BasicRegister;

	union {
		CPU_SEGMENT_REGISTER Array[6];
		struct {
			CPU_SEGMENT_REGISTER ES;
			CPU_SEGMENT_REGISTER CS;
			CPU_SEGMENT_REGISTER SS;
			CPU_SEGMENT_REGISTER DS;
			CPU_SEGMENT_REGISTER FS;
			CPU_SEGMENT_REGISTER GS;
		} Interface;
	} SegmentRegister;

	__address EIP;
} REGISTERS_STATUS, *PREGISTERS_STATUS;

/*
 * ���������CPU�ṹ����
 */
#define __CPU_CONTEXT_OPCODE_BUFFER_SIZE__    0x10
typedef struct _CHAOSVM_CPU {
	REGISTERS_STATUS CurrentRegistersStatus;

	__bool bIsSingleStepMode;//�Ƿ��ǵ���ģʽ
	__byte bRepPrefixEnable;//�ظ�ǰ׺
	__byte bDefaultAddressingMode;//Ĭ��Ѱַģʽ
	__byte bAddressingMode;//��ǰָ���Ѱַģʽ
	__byte bDefaultOperandType;//Ĭ�ϲ�����ģʽ
	__byte bOperandType;//��ǰָ��Ĳ�����ģʽ
	__byte bDefaultDataSegmentRegister;//Ĭ�����ݶζμĴ�������
	__byte bDataSegmentRegister;//��ǰָ������ݶζμĴ�������

	__byte OpcodeBuffer[__CPU_CONTEXT_OPCODE_BUFFER_SIZE__];//��ǰʹ�õı��뻺��
	__byte bOpcodeMaxLength;//�������ڵ���Ч�����볤��
	__byte bOpcodeLength;//��ǰָ���ֽڵ�����
	CPU_STATUS LastStatus;//�ϴβ����Ľ��

	CPU_INTERRUPT_INFOMATION InterruptInfomation;//�ж���Ϣ�������쳣�����ж�ʱ�ɲ����쳣�����������д
	__bool bIsInterruptOccur;

#if defined(__USE_INSTRUCTION_INFORMATION__)
	CPU_INSTRUCTION_INFOMATION InstructionInfomation;//ָ��������Ϣ����ָ��ִ��ʱ��д
#endif

	__void *pUserContext;//�û����Ӳ���
	FPCpuInterruptHandleRoutine pInterruptHandler;//�жϴ���ص�����
	FPMmReadRoutine pMmRead;//�ڴ�����ӿ�
	FPMmWriteRoutine pMmWrite;//�ڴ��д�ӿ�
	FPMmCodeReadRoutine pMmCodeRead;//�����ȡ�ӿ�
	FPMmReadDataPartRoutine pMmReadDataPart;//���ݲ��ֶ�ȡ�ӿ�
	FPMmReadDispPartRoutine pMmReadDispPart;//��ȡƫ�ƽӿ�
	FPCpuErrorOutputRoutine pErrorOut;//������Ϣ����ӿ�

	FPCpuBuildModrmRoutine pBuildModrm;//�ؽ�Modrm
	FPCpuBuildSibRoutine pBuildSib;//�ؽ�Sib

#if defined(__CPU_INTERNAL_DEBUG__)
	__byte UnCheckFlag[16];
#endif

	INSTRUCTION_TYPE_INFO InstructionTypeInfo;
	struct _HOOK_ROUTINE {
		FPCpuFlowControlHookRoutine pCall;
		FPCpuFlowControlHookRoutine pRet;
		FPCpuFlowControlHookRoutine pJmp;
		FPCpuJccFlowControlHookRoutine pJcc;
		FPCpuExecuteInstructionHookRoutine pExecuteInstruction;
		FPCpuExecuteEachInstructionHookRoutine pExecuteEachInstruction;
		FPCpuExecuteEachInstructionCompletedHookRoutine pExecuteEachInstructionCompleted;
	} HookRoutine;

	/*
	 * ���ֱ�
	 */
	PARGS_DISPATCH_TABLE_ENTRY PargsDispatchTableEntryFF[0x08];//FF��ͷ����ǲ������
	PARGS_DISPATCH_TABLE_ENTRY PargsDispatchTableEntryFE[0x08];//FE��ͷ����ǲ������
	PARGS_DISPATCH_TABLE_ENTRY PargsDispatchTableEntryC6[0x08];//C6��ͷ����ǲ������
	PARGS_DISPATCH_TABLE_ENTRY PargsDispatchTableEntryC7[0x02][0x08];//C7��ͷ����ǲ������
	PARGS_DISPATCH_TABLE_ENTRY PargsDispatchTableEntryF6[0x08];//F6��ͷ����ǲ������
	PARGS_DISPATCH_TABLE_ENTRY PargsDispatchTableEntryF7[0x08];//F7��ͷ����ǲ������
	PARGS_DISPATCH_TABLE_ENTRY PargsDispatchTableEntry80[0x08];//80��ͷ����ǲ������
	PARGS_DISPATCH_TABLE_ENTRY PargsDispatchTableEntry81[0x08];//81��ͷ����ǲ������
	PARGS_DISPATCH_TABLE_ENTRY PargsDispatchTableEntry83[0x08];//83��ͷ����ǲ������
	PARGS_DISPATCH_TABLE_ENTRY PargsDispatchTableEntry8F[0x08];//8F��ͷ����ǲ������
	PARGS_DISPATCH_TABLE_ENTRY PargsDispatchTableEntryC0[0x08];//C0��ͷ����ǲ������
	PARGS_DISPATCH_TABLE_ENTRY PargsDispatchTableEntryC1[0x08];//C1��ͷ����ǲ������
	PARGS_DISPATCH_TABLE_ENTRY PargsDispatchTableEntryD0[0x08];//D0��ͷ����ǲ������
	PARGS_DISPATCH_TABLE_ENTRY PargsDispatchTableEntryD1[0x08];//D1��ͷ����ǲ������
	PARGS_DISPATCH_TABLE_ENTRY PargsDispatchTableEntryD2[0x08];//D2��ͷ����ǲ������
	PARGS_DISPATCH_TABLE_ENTRY PargsDispatchTableEntryD3[0x08];//D3��ͷ����ǲ������
	PARGS_DISPATCH_TABLE_ENTRY PargsDispatchTableEntry0FBA[0x08];//0FBA��ͷ����ǲ������

	ONE_OPCODE_TABLE_ENTRY OneOpcodeTableEntry[0x100];//��һ�����
	TWO_BYTE_OPCODE_TABLE_ENTRY TwoByteOpcodeTableEntry[0x100];//�ڶ������
	EFLAG_CONDITION_TABLE_ENTRY EFlagConditionTableEntry[0x10];//EFlag�������Ա�
	MODRM_BYTE_ANALYSE_ROUTINE_TABLE_ENTRY ModrmByteAnalyseRoutineTableEntry[0x02];//Modrm�������̱�
} CHAOSVM_CPU, *PCHAOSVM_CPU;

#if defined(__cplusplus)
}
#endif

#endif
