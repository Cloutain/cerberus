#if !defined(__CHAOSVM_CPU_INTERNAL_STRUCTS_H__)
#define __CHAOSVM_CPU_INTERNAL_STRUCTS_H__

// �������1�ֽڵķ��ɱ�ı���
typedef CPU_STATUS (__INTERNAL_FUNC__ *FPOneByteOpcodeDispatchRoutine)(__void *);
typedef struct _ONE_OPCODE_TABLE_ENTRY {
	FPOneByteOpcodeDispatchRoutine DispatchFunction;
} ONE_OPCODE_TABLE_ENTRY, *PONE_OPCODE_TABLE_ENTRY;

// ˫�ֽ�ָ��(OFΪ��1�ֽڣ�������ĵ�2�ֽڵķ��ɱ�ı���
typedef CPU_STATUS (__INTERNAL_FUNC__ *FPTwoByteOpcodeDispatchRoutine)(__void *);
typedef struct _TWO_BYTE_OPCODE_TABLE_ENTRY {
	FPTwoByteOpcodeDispatchRoutine DispatchFunction;
} TWO_BYTE_OPCODE_TABLE_ENTRY, *PTWO_BYTE_OPCODE_TABLE_ENTRY;

// ����ָ����������ı���
typedef struct _EFLAG_CONDITION_TABLE_ENTRY {
	__byte (__INTERNAL_FUNC__ *FlagTest)(__void *);//�������Խ��Ϊ�淵��1�����򷵻�0
} EFLAG_CONDITION_TABLE_ENTRY, *PEFLAG_CONDITION_TABLE_ENTRY;

// ���ڱ������ModR/M��SIBλ�Ľ��
#define __DISP_INVALID__		0
#define __DISP_SIZE_8__			1
#define __DISP_SIZE_16__		2
#define __DISP_SIZE_32__		4
#define __REG_INVALID__			0xF0
typedef struct _MODRM_TABLE_ENTRY_32 {
	__bool bIsSibExist;
	__byte bRegisterIndex;
	__byte bDisp;
} MODRM_TABLE_ENTRY_32, *PMODRM_TABLE_ENTRY_32;

typedef struct _MODRM_TABLE_ENTRY_16 {
	__byte RegisterIndex[2];
	__byte bDisp;
} MODRM_TABLE_ENTRY_16, *PMODRM_TABLE_ENTRY_16;

typedef struct _CPU_INSTRUCTION_ARGS {
	__byte bLength;//ModR/M,SIB��ƫ�Ƶĳ���
	__byte bRegOpcode;//��odR/M�ֽڵļĴ�������
	__byte bIsEAReg;//1 -> �Ĵ���, 0 -> �ڴ��ַ
	union {
		__byte bRegIndex;
		__address addrAddress;
	} EffectiveAddress;
} CPU_INSTRUCTION_ARGS, *PCPU_INSTRUCTION_ARGS;

typedef CPU_STATUS (__INTERNAL_FUNC__ *FPModrmAnalyseRoutine)(__void *, __byte *, __byte, PCPU_INSTRUCTION_ARGS);
typedef struct _MODRM_BYTE_ANALYSE_ROUTINE_TABLE_ENTRY {
	FPModrmAnalyseRoutine AnalyseRoutine;
} MODRM_BYTE_ANALYSE_ROUTINE_TABLE_ENTRY, *PMODRM_BYTE_ANALYSE_ROUTINE_TABLE_ENTRY;

#if (MSVC_VER > 8)
#define __M_CALL_MODRM_ANALYSE_ROUNTINE__(pCPU, ...)    ((PMODRM_BYTE_ANALYSE_ROUTINE_TABLE_ENTRY)((pCPU)->ModrmByteAnalyseRoutineTableEntry)[(pCPU)->bAddressingMode]).AnalyseRoutine(__VA_ARGS__)
#else
#define __M_CALL_MODRM_ANALYSE_ROUNTINE__(pCPU, p, b, d, a)    ((PMODRM_BYTE_ANALYSE_ROUTINE_TABLE_ENTRY)((pCPU)->ModrmByteAnalyseRoutineTableEntry)[(pCPU)->bAddressingMode]).AnalyseRoutine(p, b, d, a)
#endif

/*
 * Ԥ�������ֲ�ͬ�����ĺ����ķ��ɱ����. 
 * ��Ҫ����չָ��ʹ��
 */

typedef CPU_STATUS (__INTERNAL_FUNC__ *FPBytePtrDispatch)(__void *, __byte *);
typedef struct _PUINT8_DISPATCH_TABLE_ENTRY {
	FPBytePtrDispatch DispatchFunction;
} PUINT8_DISPATCH_TABLE_ENTRY, *PPUINT8_DISPATCH_TABLE_ENTRY;

typedef CPU_STATUS (__INTERNAL_FUNC__ *FPPargsDispatch)(__void *, PCPU_INSTRUCTION_ARGS);
typedef struct _PARGS_DISPATCH_TABLE_ENTRY {
	FPPargsDispatch DispatchFunction;
} PARGS_DISPATCH_TABLE_ENTRY, *PPARGS_DISPATCH_TABLE_ENTRY;

typedef CPU_STATUS (__INTERNAL_FUNC__ *FPPargsBytePtrDispatch)(__void *, PCPU_INSTRUCTION_ARGS, __byte *);
typedef struct _PARGS_PUINT8_DISPATCH_TABLE_ENTRY {
	FPPargsBytePtrDispatch DispatchFunction;
} PARGS_PUINT8_DISPATCH_TABLE_ENTRY, *PPARGS_PUINT8_DISPATCH_TABLE_ENTRY;

#define __FUNCTION_NULL__			0

#endif

