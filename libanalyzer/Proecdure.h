// ���ú�����
typedef struct _PROCEDURE_REFFROM {
	struct _PROCEDURE_REFFROM *pNext;//��һ���ڵ�
	__void *pProcedure;//�����ڵ�,PROCEDURE�ṹ
} PROCEDURE_REFFROM, *PPROCEDURE_REFFROM;

typedef enum _ALLOC_STACK_TYPE {
	ALLOC_BY_NOT = 0,
	ALLOC_BY_ENTER,
	ALLOC_BY_SUB_ESP
} ALLOC_STACK_TYPE, *PALLOC_STACK_TYPE;
#define __INVALID_ALLOC_STACK_SIZE__			0xFFFFFFFF

// �����ṹ
typedef struct _PROCEDURE {
	struct _PROCEDURE *pNext;//��һ������

	__bool bBlock;//ֻ��һ����
	__bool bEntry;//��ڵ㺯��
	__bool bInline;//����������

	struct _PROCEDURE *pFatherProcedure;//������������������ĸ�����

	__integer iSize;//����
	__integer iInstCount;//ָ�������
	__integer iInvalidOpcodeCount;//��Чָ�����

	__address addrAllocStackMemoryAddress;//����ջ��С���ڴ��ַ
	__memory pAllocStackFileAddress;//����ջ��С���ļ���ַ
	ALLOC_STACK_TYPE AllocStackType;//����ջָ������
	__integer iStackSize;//ջ��С

	__address addrMemoryStartAddress;//�������ڴ�����ʼ�ĵ�ַ
	__memory pFileStartAddress;//�������ļ�����ʼ�ĵ�ַ

	PPROCEDURE_REFFROM pProcedureRefForm;//�ĸ������������������,�����Ҫ�ͷſռ�
	PINSTRUCTION_JMP pInstructionJmp;//��תָ����
	PX86INSTRUCTION pInstruction;//ָ����,�����Ҫ�ͷſռ�
	__void *pParents;//���ڵ�,ָ��PROGRAM�ṹ
} PROCEDURE, *PPROCEDURE;
#define __GetProcedureRefFromHeader__(pProcedureRefFrom)	((PPROCEDURE)((pProcedureRefFrom)->pProcedure))

#define __MAX_PROCEDURE_LENGTH__		4096
