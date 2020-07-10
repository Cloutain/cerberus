#if !defined(__MATHEXPGEN_H__)
#define __MATHEXPGEN_H__

#include "Common.h"
#include "logic.h"
#include "Assembler.h"
#include "RandRoll.h"
#include "ProtoTypeConfigure.h"

#if defined(__cplusplus)
extern "C"
{
#endif

#define __DEF_GEN_MATHEXP_PROB__		50

/*
 * ��ѧ��������
 */
typedef enum _MATH_OPT {
	MATH_NONE = 0,
	MATH_ADD,
	MATH_SUB,
	MATH_MUL,
	MATH_DIV,
	MATH_MOD,
	MATH_OR,
	MATH_AND,
	MATH_XOR,
	MATH_NOT
} MATH_OPT;
#define __MAX_MATH_OPT__				9

typedef enum _LOGIC_OPT {
	LOGIC_NONE = 0,
	LOGIC_XOR,
	LOGIC_ROL,
	LOGIC_ROR,
	LOGIC_NOT
} LOGIC_OPT;
#define __MAX_LOGIC_OPT__				3

typedef enum _ASM_BLOCK_TYPE {
	ASM_BLOCK_TYPE_NONE = 0,
	ASM_BLOCK_TYPE_NOT_SUB_PASSIV,
	ASM_BLOCK_TYPE_SUB_PASSIV,
	ASM_BLOCK_TYPE_NOT_SUB,
	ASM_BLOCK_TYPE_SUB,
	ASM_BLOCK_TYPE_NOT_CONST_PASSIV,
	ASM_BLOCK_TYPE_CONST_PASSIV,
	ASM_BLOCK_TYPE_NOT_VAR_PASSIV,
	ASM_BLOCK_TYPE_VAR_PASSIV,
	ASM_BLOCK_TYPE_NOT_CONST,
	ASM_BLOCK_TYPE_NOT_VAR,
	ASM_BLOCK_TYPE_CONST,
	ASM_BLOCK_TYPE_VAR
} ASM_BLOCK_TYPE;

/*
 * ��ѧ������ʽ�ڵ�
 */
typedef struct _MATHEXP {
	struct _MATHEXP *pPassivityExp;//�������ʽ
	struct _MATHEXP *pSubExp;//�ӱ��ʽ

	__bool bNot;//�Ƿ����NOT����
	MATH_OPT Opt;//������
	__bool bVariable;//�Ǳ���
	ASMER_BIT ValBit;//ֵ��λ��
	__dword dwVal;//������
} MATHEXP, *PMATHEXP;

/*
 * �����ӱ��ʽ�ڵ�
 */
typedef struct _CIPHEREXP {
	struct _CIPHEREXP *pNextExp;//�������ʽ

	__bool bNot;//NOT����
	LOGIC_OPT Opt;//������
	__bool bKey;//����Կ
	ASMER_BIT ValBit;//ֵ��λ��
	__dword dwVal;//������
} CIPHEREXP, *PCIPHEREXP;

PMATHEXP __INTERNAL_FUNC__ GenerateMathExpression(PPROTOTYPE_CONFIGURE pProtoTypeConfigure);
__memory __INTERNAL_FUNC__ GenerateMathExpCode(PMATHEXP pMathExp, __integer *piOutBinSize, PPROTOTYPE_MATHEXP_ASM_CONFIGURE pMathExpAsmConfigure, __integer iImageBase);
__void __INTERNAL_FUNC__ ReleaseMathExp(PMATHEXP *pMathExpPoint);

PCIPHEREXP __INTERNAL_FUNC__ GenerateCipherExpression(PPROTOTYPE_CONFIGURE pProtoTypeConfigure);
PCIPHEREXP __INTERNAL_FUNC__ ReverseCipherExpression(PCIPHEREXP pCipherExp);
__memory __INTERNAL_FUNC__ GenerateCipherExpCode(PCIPHEREXP pCipherExp, __integer *piOutBinSize, PPROTOTYPE_CIPHEREXP_ASM_CONFIGURE pCipherExpAsmConfigure, \
												 __integer iImageBase, __bool bReverse);
__void __INTERNAL_FUNC__ ReleaseCipherExp(PCIPHEREXP *pCipherExpPoint);

#if defined(__cplusplus)
}
#endif

#endif
