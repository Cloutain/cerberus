// ��תָ��ṹ
typedef enum _JMPBIT {
	JMP_NONE = 0x00,
	JMP_BIT8 = 0x08,
	JMP_BIT16 = 0x10,
	JMP_BIT32 = 0x20
} JMPBIT;

typedef enum _MOVTYPE {
	MOV_NONE = 0,
	MOV_REG_IMM,
	MOV_REG_MEM,
	LEA_REG_MEM
} MOVTYPE, *PMOVTYPE;

typedef enum _JMPTYPE {
	JMP_TYPE_NOT_FIX = 0,
	JMP_TYPE_JMP_OFFSET,
	JMP_TYPE_CALL_OFFSET,
	JMP_TYPE_JCC_OFFSET,
	JMP_TYPE_JMP_IMEM,
	JMP_TYPE_JMP_MOV_ADDR_REG,
	JMP_TYPE_JMP_MOV_ADDR_MREG,
	JMP_TYPE_JMP_MOV_MEM_REG,
	JMP_TYPE_JMP_MOV_MEM_MREG,
	JMP_TYPE_CALL_IMEM,
	JMP_TYPE_CALL_MOV_ADDR_REG,
	JMP_TYPE_CALL_MOV_ADDR_MREG,
	JMP_TYPE_CALL_MOV_MEM_REG,
	JMP_TYPE_CALL_MOV_MEM_MREG
} JMPTYPE;

typedef enum _JMPWAY {
	JMP_WAY_UNKNOW = 0,
	JMP_WAY_UP,
	JMP_WAY_DOWN
} JMPWAY;

typedef struct _INSTRUCTION_JMP {
	struct _INSTRUCTION_JMP *pNext;//��һ���ڵ�

	__bool bTargetAddressIsInvalid;//��תĿ���ַ�Ƿ���Ч
	__bool bJmpOut;//�Ƿ�����������ĺ�����
	__bool bIndirectJmp;//�Ƿ��Ǽ����ת

	JMPWAY JmpWay;//��ת����
	JMPBIT JmpBit;//��ת��λ��
	JMPTYPE JmpType;//��ת������

	union {
		__address addrTargetMemoryAddress;//��ת����ڴ��ַ
		__address addrIndirectMemoryAddress;//��ӵ��ڴ��ַ
	};

	union {
		__memory pTargetFileAddress;//��ת����ļ���ַ
		__memory pIndirectFileAddress;//��ӵ��ڴ��ַ
	};

	union {
		__byte bVal;
		__word wVal;
		__dword dwVal;
	} Offset;//ƫ��

	__void *pParents;//���ڵ�,ָ��X86INSTRUCTION�ṹ
} INSTRUCTION_JMP, *PINSTRUCTION_JMP;

// ���õ�����
typedef struct _REFERENCE_DATA {
	__bool bSet;//�����ò���,����Ϊ��ȡ����
	__bool bMaybeCrash;//���ܲ��ǵ�ַ,����������

	__address addrMemoryAddress;//���ݵ��ڴ��ַ
	__memory pFileAddress;//���ݵ��ļ���ַ

	PIMPORT_TABLE_REFERENCE pImportTableReference;//���������
} REFERENCE_DATA, *PREFERENCE_DATA;

// ָ��ṹ
typedef enum _HOOK_SUCCESS_LEVEL {
	HOOK_LEVEL_NOT = 0,
	HOOK_LEVEL_MAYBE,
	HOOK_LEVEL_OK
} HOOK_SUCCESS_LEVEL;

// ָ��ṹ
typedef struct _X86INSTRUCTION {
	struct _X86INSTRUCTION *pPrev;//��һ���ڵ�
	struct _X86INSTRUCTION *pNext;//��һ���ڵ�


	/* ��������ָ����Ϣ */
	__bool bInvalid;//����Ч��ָ��
	__bool bJmp;//�Ƿ�����ת
	__bool bReferenceData;//�Ƿ����ñ���
	__bool bExchange;//�Ƿ���Խ��н���,���ڵ���5�ֽڲ�����תָ���ָ��

	HOOK_SUCCESS_LEVEL HookSuccessLevel;//HOOK�ɹ��ļ���
	__memory pHookSaveAddress;//����HOOK��ַ���ļ���ַ

	REFERENCE_DATA ReferenceData;//���õ�����
	INSTRUCTION_JMP InstructionJmp;//��תָ��
	ud_t ud_obj;//�����ṹ
	__address addrCurrMemoryAddress;//��ǰָ���ڴ��ַ
	__memory pCurrFileAddress;//��ǰָ���ļ���ַ
	__offset ofOffsetByProcedureStart;//�뺯��ͷ��ƫ��
	__offset ofOffsetByImageBase;//���ַ��ƫ��

	__bool EffectRegister[8];//��Ӱ��ļĴ���
	__bool EffectEFlag[32];//��Ӱ��ı�־�Ĵ���
	__bool ConditionEFlag[32];//������־�Ĵ���

	__void *pParents;//���ڵ�,ָ��PROCEDURE�ṹ


	/* �������������߼�ָ����� */
	__bool bErrorLogicInstruction;//�����߼���ָ��
	__memory pMem;//pMem��ָ��
	__address addrMemoryBlockStart;//��������ʼ��ָ��
	__integer iBlockSize;//��������ĳ���

} X86INSTRUCTION, *PX86INSTRUCTION;
#define __ThisInstIsJmp__(pInstruction)			((pInstruction)->bJmp)
#define __ThisInstNotHook__(pInstruction)		((pInstruction)->HookSuccessLevel == HOOK_LEVEL_NOT)
#define __ThisInstMayHook__(pInstruction)		((pInstruction)->HookSuccessLevel == HOOK_LEVEL_MAYBE)
#define __ThisInstCanHook__(pInstruction)		((pInstruction)->HookSuccessLevel == HOOK_LEVEL_OK)
#define __ThisInstCanExchange__(pInstruction)	((pInstruction)->bExchange)

#define __GetInstructionJmpFromHeader__(pInstructionJmpFrom)			((PX86INSTRUCTION)((pInstructionJmpFrom)->pInstruction))

