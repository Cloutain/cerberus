#if !defined(__CHAOSVM_CPU_DISASM_H__)
#define __CHAOSVM_CPU_DISASM_H__

/*
 * ���ṹ���ڱ���ǰ׺�����Ľ��
 */
typedef struct _DISASM_CONTEXT {
	__byte bPrefixLength;
	__bool bIsOperandSize16;//�Ƿ���16λ������ģʽ
	__bool bIsAddressSize16;//�Ƿ���16λѰַ
	__bool bIsRep;//�Ƿ���REPǰ׺
} DISASM_CONTEXT, *PDISASM_CONTEXT;

__byte __INTERNAL_FUNC__ AnalysePrefix(PDISASM_CONTEXT pDisasmContext, __byte *pCode, __byte bMaxLength);
__byte __INTERNAL_FUNC__ IsCall(PCHAOSVM_CPU pCPU, __byte *pCode, __byte bMaxLength);
__byte __INTERNAL_FUNC__ IsRepString(PCHAOSVM_CPU pCPU, __byte *pCode, __byte bMaxLength);

#endif
